/***************************************************************/
/*  NAME: Alon Yaari                                           */
/*  ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*  FILE: M200.cpp                                             */
/*  DATE: Spring 2014                                          */
/***************************************************************/

#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include "MBUtils.h"
#include "AngleUtils.h"
#include "M200.h"

using namespace std;

//----------------------------------------------------
// Constructor

iM200::iM200()
{
  // Prepare for connecting with a TCP socket
  // The MAN page of getaddrinfo() states "All the other fields in the
  // structure pointed to by hints must contain either 0 or a null
  // pointer, as appropriate." When a struct is created in C++, it
  // will be given a block of memory. This memory is not necessary
  // empty. Therefore we use the memset function to make sure all
  // fields are NULL.
  memset(&host_info, 0, sizeof host_info);

  // IP version not specified. Can be both.
  host_info.ai_family   = AF_UNSPEC;     
  // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
  host_info.ai_socktype = SOCK_STREAM;   

  // When Rudder/speed is implemented this should default to "rudder-thrust"
  m_thrust_mode         = "direct-thrust";
  m_post_raw_msgs       = true;
 
  m_IP                  = "localhost";
  m_Port                = "29500";
  m_connected           = false;

  m_heading_source      = "NAV_UPDATE";
  m_heading_msg_name    = "NAV_HEADING";
  m_mag_offset_using    = false;
  m_mag_offset          = 0;
  host_info_list        = 0;
  socketfd              = 0;

  m_batt_voltage        = 0;
  m_stale_detections    = 0;
  m_stale_threshold     = 1.5; // seconds
  m_stale_mode          = true;
  
  // For operation in thrust_mode = "direct-thrust"
  m_des_thrust_lft        = 0;
  m_des_thrust_rgt        = 0;
  m_des_thrust_lft_tstamp = 0;
  m_des_thrust_rgt_tstamp = 0;

  // For operation in thrust_mode = "rudder-thrust"
  m_des_rudder          = 0;
  m_des_thrust          = 0;
  m_des_rudder_tstamp   = 0;
  m_des_thrust_tstamp   = 0;

  // For operation in thrust_mode = "heading-speed"
  m_des_hdg             = 0;
  m_des_spd             = 0;
  m_des_hdg_tstamp      = 0;
  m_des_spd_tstamp      = 0;

  // Values actually sent to front if in either the 
  // direct-thrust or rudder-thrust modes
  m_command_thrust_lft  = 0;
  m_command_thrust_rgt  = 0;

  m_msgs_to_front       = 0;
  m_msgs_from_front     = 0;
}

//----------------------------------------------------
// Procedure: OnNewMail()

bool iM200::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  cout << "==================================" << endl;
  cout << "In OnNewMail" << endl;

  double curr_time = MOOSTime();

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    p->Trace();
    string key = msg.GetKey();
    double dval = msg.GetDouble();
    string sval = msg.GetString();

    if(key == "KF_COMMANDED_L") 
      cout << "KEY: " << key << ":" << dval << endl;
    if(key == "KF_COMMANDED_R") 
      cout << "KEY: " << key << ":" << dval << endl;

    if(m_thrust_mode == "direct-thrust") {
      if(key == "KF_COMMANDED_L" || key == "COMMANDED_L") {
	m_des_thrust_lft = dval;
	m_des_thrust_lft_tstamp = curr_time;
      }
      else if(key == "KF_COMMANDED_R" || key == "COMMANDED_R") {
	m_des_thrust_rgt = dval; 
	m_des_thrust_rgt_tstamp = curr_time;
      }
    }
    else if(m_thrust_mode == "rudder-thrust") {
      if(key == "DESIRED_RUDDER") {
	m_des_rudder = dval;
	m_des_rudder_tstamp = curr_time;
      }
      else if(key == "DESIRED_THRUST") {
	m_des_thrust = dval;
	m_des_thrust_tstamp = curr_time;
      }
    } 
    else if(m_thrust_mode == "heading-speed") {
      if(key == "DESIRED_HEADING") {
	m_des_hdg = dval;
	m_des_hdg_tstamp = curr_time;
      }
      else if(key == "DESIRED_SPEED") {
	m_des_spd = dval; 
	m_des_spd_tstamp = curr_time; 
      } 
    }

    if(key == "THRUST_MODE_DIFFERENTIAL") {
      if(tolower(sval) == "true")
	handleSetThrustMode("direct-thrust");
      else
	handleSetThrustMode("rudder-thrust");
    }
  }
  return(true);
}

//----------------------------------------------------
// Procedure: OnStartUp()

bool iM200::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  
  STRING_LIST sParams;
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());
  
  bool goodParams = true;
  STRING_LIST::iterator p;
  for (p = sParams.begin(); p != sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;
    
    bool handled = false;
    if (param == "ip_address")
      handled = SetParam_IP_ADDRESS(value);
    else if (param == "port_number")
      handled = SetParam_PORT_NUMBER(value);
    else if (param == "thrust_mode")
      handled = handleSetThrustMode(value);
    else if (param == "heading_source")
      handled = SetParam_HEADING_SOURCE(value);
    else if (param == "heading_msg_name")
      handled = SetParam_HEADING_MSG_NAME(value);
    else if (param == "mag_offset")
      handled = SetParam_MAG_OFFSET(value);

    if(!handled)
      reportUnhandledConfigWarning(orig); 
  }
  
  if (OpenConnectionTCP(m_IP, m_Port))
      if (OpenSocket())
          Connect();

  // OnStartup() must always return true
  //    - Or else it will quit during launch
  //        and appCast info will be unavailable
  return(true);

}

//----------------------------------------------------
// Procedure: OnConnectToServer()

bool iM200::OnConnectToServer()
{
  RegisterVariables();
  return(true);
}

//----------------------------------------------------
// Procedure: Iterate()

bool iM200::Iterate()
{
  AppCastingMOOSApp::Iterate();
  
  // Part 1: Check for staleness of input from payload community
  bool stale_input = staleModeCheck();
  
  // Part 2: Case where we transition from un-stale to stale mode
  bool transitioned_to_stale = false;
  if(stale_input && !m_stale_mode) {
    m_stale_detections++;
    transitioned_to_stale = true;
    reportRunWarning("Stale Command Input Detected");
  }

  // Part 3: Case where we transition from stale to un-stale mode
  if(!stale_input && m_stale_mode)
    retractRunWarning("Stale Command Input Detected");

  m_stale_mode = stale_input;

  // Part 4: Send info to front seat
  if(!m_stale_mode || transitioned_to_stale) {
    bool send_ok = Send();
    if(send_ok)
      m_msgs_to_front++;
    else
      reportRunWarning("Failure sending commands to vehicle.");
  }  

  // Receive any pending messages
  bool receive_ok = Receive();
  if(receive_ok)
    m_msgs_from_front++;
  else
    reportRunWarning("Failure on receive from vehicle - host may be down.");
  
  

  AppCastingMOOSApp::PostReport();
  return(true);
}


//----------------------------------------------------
// Procedure: RegisterVariables()

void iM200::RegisterVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("KF_COMMANDED_L", 0);
  Register("KF_COMMANDED_R", 0);
  Register("THRUST_MODE_DIFFERENTIAL", 0);
}

//----------------------------------------------------
// Procedure: SetParam_IP_ADDRESS()

bool iM200::SetParam_IP_ADDRESS(string sVal)
{
  if(!isValidIPAddress(sVal)) {
    string msg = "Invalid IP address [" + sVal + "]. Keeping: [" + m_IP + "]";
    reportConfigWarning(msg);
    return(false);
  }

  m_IP = sVal;
  return(true);
}

//----------------------------------------------------
// Procedure: SetParam_PORT_NUMBER()

bool iM200::SetParam_PORT_NUMBER(string sVal)
{
  if(!isNumber(sVal)) {
    string msg = "Invalid port number [" + sVal + "]. Keeping: [" + m_Port + "]";
    reportConfigWarning(msg);
    return(false); 
  }

  // Check that the string contains a valid number
  int x = (int) strtod(sVal.c_str(), 0);
  if(x < 0) {
    string msg = "Invalid port number [" + sVal + "]. Keeping: [" + m_Port + "]";
    reportConfigWarning(msg);
    return(false); 
  }
  m_Port = sVal;
  return(true);
}

//----------------------------------------------------
// Procedure: handleSetThrustMode

bool iM200::handleSetThrustMode(string param)
{
  if((param == "direct-thrust") || 
     (param == "rudder-thrust") ||
     (param == "heading-speed"))
    m_thrust_mode = param;
  else 
    return(false); 

  return(true);
}


//----------------------------------------------------
// Procedure: SetParam_HEADING_SOURCE

bool iM200::SetParam_HEADING_SOURCE(string sVal)
{
  if(sVal.empty()) {
    m_heading_source = "NAV_UPDATE";
    string msg = "Config param HEADING_SOURCE is blank. ";
    msg += "Defaulting to 'NAV_UPDATE'.";
    reportConfigWarning(msg);
    return(false); 
  }
  
  m_heading_source = sVal;
  MOOSToUpper(m_heading_source);
  string findSrc = HEADING_OPTIONS;
  size_t found = findSrc.find(m_heading_source);
  if(found == string::npos) {
    string warning = "In mission file, HEADING_SOURCE must be one of: ";
    warning += HEADING_OPTIONS;
    reportConfigWarning(warning);
    return(false); 
  }
  return(true);
}

//----------------------------------------------------
// Procedure: SetParam_HEADING_MSG_NAME

bool iM200::SetParam_HEADING_MSG_NAME(string sVal)
{
  if(sVal.empty()) {
    string msg = "Config parameter HEADING_MSG_NAME is blank, defaulting to";
    reportConfigWarning(msg + m_heading_msg_name);
    return(true); }

  // Enforce that MOOS variables should not contain white space
  if(strContainsWhite(sVal)) {
    reportConfigWarning("heading_msg_name param should not contain wht space");
    return(false); }

  m_heading_msg_name = sVal;
  return(true);
}

//----------------------------------------------------
// Procedure: SetParam_MAG_OFFSET

bool iM200::SetParam_MAG_OFFSET(string sVal)
{
  if(!isNumber(sVal)) {
    m_mag_offset_using = false;
    m_mag_offset = 0;
    string msg = "Param MAG_OFFSET is not a number, defaulting to 0.";
    reportConfigWarning(msg);
    return(true); 
  }
  
  m_mag_offset_using = true;
  m_mag_offset = strtod(sVal.c_str(), 0);
  return(true);
}

//----------------------------------------------------
// Procedure: OpenConnectionTCP

bool iM200::OpenConnectionTCP(string addr, string port)
{
  if(!isValidIPAddress(addr) || !isNumber(port)) {
    reportRunWarning("Invalid IP and/or port.");
    return(false); 
  }
  
  // Now fill the linked list of host_info struct with the connection info
  int status = getaddrinfo(addr.c_str(), port.c_str(), 
			   &host_info, &host_info_list);
  
  // 0 = success, otherwise failure
  if(status == 0) {
    string event = "Successful connection to IP ";
    event += addr + " Port " + port;
    reportEvent(event);
    m_connected = true;
    return(true); 
  }

  string warning = "Error in connection to IP ";
  warning += addr + " Port " + port + ". ";
  warning += gai_strerror(status);
  reportRunWarning(warning);
  m_connected = false;
  return(false);  
}

//----------------------------------------------------
// Procedure: OpenSocket

bool iM200::OpenSocket()
{
  socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, 
		    host_info_list->ai_protocol);

  
  int tmp = errno;     // Alon - where does errno get declared, set?
  if(socketfd == -1) {
    string warning = "Socket error: ";
    warning += strerror(tmp);
    reportRunWarning(warning);
    return(false); 
  }
  else {
    reportEvent("Socket open"); 
  }

  return(true);
}

//----------------------------------------------------
// Procedure: Connect()

bool iM200::Connect()
{
  int status = connect(socketfd, host_info_list->ai_addr, 
		       host_info_list->ai_addrlen);
  int tmp = errno;
  if(status == -1) {
    string warning = "Connect error: ";
    warning += strerror(tmp);
    reportRunWarning(warning);
    return(false);
  }
  else 
    reportEvent("Connection open"); 

  return(true);
}

//----------------------------------------------------
// Procedure: Send()

bool iM200::Send()
{
  string message = "$";

  if(m_thrust_mode == "direct-thrust") {
    m_command_thrust_lft = m_des_thrust_lft;
    m_command_thrust_rgt = m_des_thrust_rgt;
    message += "PYDIR,";
    message += doubleToString(m_command_thrust_lft);
    message += ",";
    message += doubleToString(m_command_thrust_rgt);
    message += "*\r\n"; 
  }

  if(m_thrust_mode == "heading-speed") {
    m_command_thrust_lft = 0;
    m_command_thrust_rgt = 0;
#if 0
    if(m_des_spd == 0) {
      message += "PYDIR,0.0,0.0*\r\n"; }
    else {
      message += "PYDEV,";
      message += doubleToString(m_des_hdg);
      message += ",";
      message += doubleToString(m_des_spd);
      message += "*\r\n";
    }
#endif
  }
  
#if 0
  if(m_thrust_mode == "rudder-thrust") {
    makeMapping(m_des_rudder, m_des_thrust, 
      m_command_thrust_lft, m_command_thrust_rgt);
    message += "PYDIR,";
    message += doubleToString(m_command_thrust_lft);
    message += ",";
    message += doubleToString(m_command_thrust_rgt);
    message += "*\r\n"; 
  }
#endif
  
  reportEvent("Sending to vehicle: " + message);

  if(m_post_raw_msgs)
    Notify("M200_RAW_SEND", message);

  ssize_t bytes_sent;
  int len = strlen(message.c_str());
  bytes_sent = send(socketfd, message.c_str(), len, 0);
  if(bytes_sent == -1)     // -1 == error
    return(false);
  return(true);
}

//----------------------------------------------------
// Procedure: SendHeartBeat()

bool iM200::SendHeartBeat()
{
  // Part 1: Build the PYSTS Status message
  string message = "$";
  message += "PYSTS,";
  message += doubleToString(MOOSTime(),2);
  message += ",1,,";
  message += "*\r\n"; 

  // Part 2: Try to send the message, check if it succeeded
  ssize_t bytes_sent;
  int len = strlen(message.c_str());
  bytes_sent = send(socketfd, message.c_str(), len, 0);

  bool send_ok = true;
  if(bytes_sent == -1)     // -1 == error
    send_ok = false;

  // Part 3: Perhaps report the message for debugging to MOOSDB
  if(m_post_raw_msgs) {
    string debug_msg = message;
    if(!send_ok)
      debug_msg = "FAILED " + message;
    Notify("M200_RAW_SEND", debug_msg);
  }

  // Part 4: Add the message, result to appcasting events
  string event = "Sending: " + message; 
  if(!send_ok)
    event = "Sending(F): " + message;
  reportEvent(event);

  return(send_ok);
}

//----------------------------------------------------
// Procedure: Receive()

#if 0
bool iM200::Receive()
{
  // Objective is to parse out NMEA strings from the incoming data
  ssize_t numBytes;
  char incoming[1000];
  numBytes = recv(this->socketfd, incoming, 1000, 0);

  //TODO: Handle errors from recv
  if(numBytes < 1)
    return(false);
  
  // We may have received part of an NMEA string earlier so
  // concatenate to end of earlier
  //      - Check to prevent buffer overflow
  size_t inBuffLen   = strlen(inBuff);
  size_t incomingLen = strlen(incoming);
  if(inBuffLen + incomingLen < MAX_BUFF)
    strncat(inBuff, incoming, numBytes);
  string toParse = inBuff;
  strcpy(inBuff, "");   // Clear the persistent buffer
  
  // Everything before the first $ is lost; we're missing the
  // beginning of that NMEA message
  // If no $ in the message then there isn't anything to parse

  unsigned long int startNMEA = toParse.find('$');
  if(startNMEA == string::npos) {
    strcpy(inBuff, toParse.c_str());
    return(true); 
  }
  if(startNMEA > 0)
    toParse = toParse.substr(startNMEA);
  
  // Parse string into a vector of lines
  vector<string> lines = parseString(toParse, "\r\n");
  if(lines.size() < 1)
    return(true);
  
  // If the last item is not a full sentence, keep it for next time
  string last = lines[lines.size() - 1];
  unsigned int lastSize = last.size();
  if (last.size() < 4 || last.at(lastSize - 3) != '*')
    strcpy(inBuff, last.c_str());
  
  // Visit each string item in the vector and deal with it
  for (unsigned int i = 0; i < lines.size(); i++) {
    string line = lines[i];
    if(!line.empty()) {
      if(NMEAChecksumIsValid(line)) {
	if(line.at(0) == '$') {
	  DealWithNMEA(line); 
	} 
      } 
    } 
  }
  return(true);
}
#endif

bool iM200::Receive()
{
  // Objective is to parse out NMEA strings from the incoming data
  ssize_t numBytes;
  char incoming[1000];
  numBytes = recv(this->socketfd, incoming, 1000, 0);

  if(m_post_raw_msgs) {
    string raw_msg = incoming;
    raw_msg = findReplace(raw_msg, (char)(13), " ");
    raw_msg = findReplace(raw_msg, (char)(10), " ");
    Notify("M200_RAW_RCV", raw_msg);
  }

  if(numBytes == 0)
    return(false);
  
  // We may have received part of an NMEA string earlier so
  // concatenate to end of earlier
  // - Check to prevent buffer overflow
  size_t inBuffLen   = strlen(inBuff);
  size_t incomingLen = strlen(incoming);
  if(inBuffLen + incomingLen < MAX_BUFF)
    strncat(inBuff, incoming, numBytes);
  string toParse = inBuff;
  strcpy(inBuff, "");   // Clear the persistent buffer
  
  // Everything before the first $ is lost; we're missing the
  // beginning of that NMEA message
  // If no $ in the message then there isn't anything to parse

  unsigned long int startNMEA = toParse.find('$');
  if(startNMEA == string::npos) {
    strcpy(inBuff, toParse.c_str());
    return(true); 
  }
  if(startNMEA > 0)
    toParse = toParse.substr(startNMEA);
  
  // Parse string into a vector of lines
  vector<string> lines = parseString(toParse, "\r\n");
  if(lines.size() < 1)
    return(true);
  
  // If the last item is not a full sentence, keep it for next time
  string last = lines[lines.size() - 1];
  unsigned int lastSize = last.size();
  if (last.size() < 4 || last.at(lastSize - 3) != '*')
    strcpy(inBuff, last.c_str());
  
  // Visit each string item in the vector and deal with it
  for (unsigned int i = 0; i < lines.size(); i++) {
    string line = lines[i];
    if(!line.empty()) {
      if(NMEAChecksumIsValid(line)) {
	if(line.at(0) == '$') {
	  DealWithNMEA(line); 
	} 
      } 
    } 
  }
  return(true);
}

//----------------------------------------------------
// Procedure: staleModeCheck()
//   Returns: true if the relevant input IS stale
//            false if NOT stale

bool iM200::staleModeCheck()
{
  double curr_time = MOOSTime();
  bool stale_input = false;

  // Part 1: Handle the Direct Thrust case
  if(m_thrust_mode == "direct-thrust") {
    // If either the left or right commands never received, return false
    if((m_des_thrust_lft_tstamp == 0) || (m_des_thrust_rgt_tstamp == 0))
      stale_input = true;
    // Now check the left / right gaps
    double command_left_gap = curr_time - m_des_thrust_lft_tstamp;
    if(command_left_gap > m_stale_threshold)
      stale_input = true;
    double command_right_gap = curr_time - m_des_thrust_rgt_tstamp;
    if(command_right_gap > m_stale_threshold)
      stale_input = true;
  }

  // Part 2: Handle the rudder-thrust mode
  if(m_thrust_mode == "rudder-thrust") {
    // If either deisred rudder or thrust never received, return false
    if((m_des_rudder_tstamp == 0) || (m_des_thrust_tstamp == 0))
      stale_input = true;
    double des_rudder_gap = m_des_rudder_tstamp - curr_time;
    if(des_rudder_gap > m_stale_threshold)
      stale_input = true;
    double des_thrust_gap = m_des_thrust_tstamp - curr_time;
    if(des_thrust_gap > m_stale_threshold)
      stale_input = true;
  }
  
  // Part 3: Handle the heading-speed mode
  if(m_thrust_mode == "heading-speed") {
    // If either desired heading or speed never received, return false
    if((m_des_hdg_tstamp == 0) || (m_des_spd_tstamp == 0))
      stale_input = true;
    double des_hdg_gap = m_des_hdg_tstamp - curr_time;
    if(des_hdg_gap > m_stale_threshold)
      stale_input = true;
    double des_spd_gap = m_des_spd_tstamp - curr_time;
    if(des_spd_gap > m_stale_threshold)
      stale_input = true;
  }
  
  if(stale_input) {
    m_des_thrust_lft = 0;
    m_des_thrust_rgt = 0;
    m_des_rudder  = 0;
    m_des_thrust  = 0;
    m_des_hdg     = 0;
    m_des_spd     = 0;
  }

  return(stale_input);
}

//----------------------------------------------------
// Procedure: PublishHeading

void iM200::PublishHeading(double dHeading)
{
  if(dHeading == BAD_DOUBLE)
    return;
  dHeading += m_mag_offset;
  dHeading = angle360(dHeading);

  Notify(m_heading_msg_name, dHeading);
}

//----------------------------------------------------
// Procedure: DealWitNMEA

bool iM200::DealWithNMEA(string nmea)
{
  if(nmea.empty())
    return(false);

  vector<string> parts = parseString(nmea, ',');
  string nmeaHeader = parts[0];
  
  if(nmeaHeader == "$CPNVG")
    return(ParseCPNVG(nmea));
  else if(nmeaHeader == "$CPRBS")
    return(ParseCPRBS(nmea));
  else if(nmeaHeader == "$CPRCM")
    return(ParseCPRCM(nmea));
  else if(nmeaHeader == "$GPRMC") {
    Notify("NMEA_MSG", nmea);
    return(ParseGPRMC(nmea)); 
  }
  else if((nmeaHeader == "$GPGGA") || (nmeaHeader == "$GPRMC") ||
	  (nmeaHeader == "$GPGST") || (nmeaHeader == "$GPRME")) {
    Notify("NMEA_MSG", nmea); 
  }
  else
    return(false);
  
  return(true);
}

//----------------------------------------------------
// Procedure: ParseCPNVG
//      Note: CPNVG: Navigation Update

bool iM200::ParseCPNVG(string nmea)
{
  if(m_heading_source != "NAV_UPDATE")
    return(true);
  CPNVGnmea cpnvg;
  cpnvg.ParseSentenceIntoData(nmea, true);
  if(!cpnvg.CriticalDataAreValid()) {
    reportRunWarning("Could not parse CPNVG message: " + nmea);
    return(false); 
  }
  
  double dHeading = BAD_DOUBLE;
  if(cpnvg.Get_headingTrueN(dHeading))
      PublishHeading(dHeading);
  return(true);
}

//----------------------------------------------------
// Procedure: ParseCPRBS
//      Note: CPRBS: Battery voltage

bool iM200::ParseCPRBS(string nmea)
{
  CPRBSnmea cprbs;
  cprbs.ParseSentenceIntoData(nmea, true);
  if(!cprbs.CriticalDataAreValid()) {
    reportRunWarning("Could not parse CPRBS message: " + nmea);
    return(false); 
  }

  if(cprbs.Get_battStackVoltage(m_batt_voltage))
    Notify("M200_BATT_VOLTAGE", m_batt_voltage);

  return(true);
}

//----------------------------------------------------
// Procedure: ParseCPRCM
//      Note: CPRCM: Raw Compass Data

bool iM200::ParseCPRCM(string nmea)
{
    nmea += "";
  if(m_heading_source == "RAW_COMPASS") {
    reportRunWarning("RAW_COMPASS mode not yet implemented");
    return(true);
    // PublishHeading(dHeading);
  }
  return(true);
}

//----------------------------------------------------
// Procedure: ParseGPRMC
//      Note: GPRMC: Raw GPS message containing heading, other details

bool iM200::ParseGPRMC(string nmea)
{
  if(m_heading_source != "RAW_GPS")
    return(true);

  GPRMCnmea gprmc;
  gprmc.ParseSentenceIntoData(nmea, true);

  if(!gprmc.CriticalDataAreValid()) {
    reportRunWarning("Could not parse GPRMC message: " + nmea);
    return false; 
  }
  
  double dHeading = BAD_DOUBLE;
  if(gprmc.Get_headingTrueN(dHeading))
    PublishHeading(dHeading);

  return(true);
}

//----------------------------------------------------
// Procedure: NMEAChecksumIsValid()

bool iM200::NMEAChecksumIsValid(string nmea)
{
  unsigned char xCheckSum = 0;
  string sToCheck;
  MOOSChomp(nmea,"$");
  sToCheck = MOOSChomp(nmea,"*");
  string sRxCheckSum = nmea;

  //now calculate what we think check sum should be...
  string::iterator p;
  for (p = sToCheck.begin(); p != sToCheck.end(); p++)
    xCheckSum^=*p;
  ostringstream os;
  os.flags(ios::hex);
  os << (int) xCheckSum;
  string sExpected = os.str();

  if(sExpected.length() < 2)
    sExpected = "0" + sExpected;

  bool result = MOOSStrCmp(sExpected,sRxCheckSum);

  return(result);
}

//----------------------------------------------------
// Procedure: buildReport

bool iM200::buildReport()
{
  // Build Configuration Block Strings
  string s_ip_port = m_IP + "/" + m_Port;
  string s_mag_off = boolToString(m_mag_offset_using) + " (" + 
    doubleToString(m_mag_offset, 2) + ")";
  string s_connect = " (CONNECTED)";
  if(!m_connected)
    s_connect = " (NO-CONNECT)";

  // Build BackSeat Info Block Strings
  string s_thrust_mode = m_thrust_mode;

  string s_stale_info = toupper(boolToString(m_stale_mode)) + " (" + 
    uintToString(m_stale_detections) + ")";

  string s_detects  = uintToString(m_stale_detections);
  string s_msgs_out = uintToString(m_msgs_to_front);
  string s_msgs_in  = uintToString(m_msgs_from_front);

  string s_dthrust_info = "---";
  string s_drudder_info = "---";
  string s_dheading_info = "---";

  if(m_thrust_mode == "heading-speed") {
    s_dheading_info = "(" + doubleToString(m_des_hdg,2) + ", ";
    s_dheading_info += doubleToString(m_des_spd,2) + ")";
  }
  else if(m_thrust_mode == "rudder-thrust") {
    s_drudder_info = "(" + doubleToString(m_des_rudder,2) + ", ";
    s_drudder_info += doubleToString(m_des_thrust,2) + ")";
  }
  else if(m_thrust_mode == "direct-thrust") {
    s_dthrust_info = "(" + doubleToStringX(m_des_thrust_lft,2) + ", "; 
    s_dthrust_info += doubleToStringX(m_des_thrust_rgt,2) + ")";
  }


  string s_commanded = "---";
  if((m_thrust_mode=="direct-thrust") || (m_thrust_mode=="rudder-thrust")) {
    s_commanded = "(" + doubleToString(m_command_thrust_lft,2) + ", ";
    s_commanded += doubleToString(m_command_thrust_rgt,2) + ")";
  }
  
  // Build FrontSeat Info Block Strings
  string s_battv = doubleToString(m_batt_voltage, 3);

  // Produce the report
  m_msgs << "Configuration: " << endl;
  m_msgs << "   IP/Port:                  " << s_ip_port << s_connect << endl;
  m_msgs << "   Magnetic Offset:          " << s_mag_off << endl << endl;
  
  m_msgs << "BackSeat Info:" << endl;
  m_msgs << "   Messages sent to M200:    " << s_msgs_out      << endl;
  m_msgs << "   Stale Mode:               " << s_stale_info    << endl;
  m_msgs << "   Thrust Mode:              " << s_thrust_mode   << endl;
  m_msgs << "   Heading, Speed:           " << s_dheading_info << endl;
  m_msgs << "   Rudder, Thrust:           " << s_drudder_info  << endl;
  m_msgs << "   Thrust-L, Thrust-R:       " << s_dthrust_info  << endl;
  m_msgs << "   ------------------------- " << endl;
  m_msgs << "   Commanded-L, Commanded-R: " << s_commanded     << endl << endl;

  m_msgs << "FrontSeat Info:" << endl;
  m_msgs << "   Messages rcvd from M200: " << s_msgs_in  << endl;
  m_msgs << "   Battery voltage:         " << s_battv    << endl;

  return(true);
}
