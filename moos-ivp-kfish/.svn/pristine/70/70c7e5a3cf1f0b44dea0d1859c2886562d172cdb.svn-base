#include <iostream>
#include <string>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include "MBUtils.h"
#include "M200.h"


using namespace std;


iM200::iM200()
{
    // Prepare for connecting with a TCP socket

    // The MAN page of getaddrinfo() states "All  the other fields in the structure pointed
    // to by hints must contain either 0 or a null pointer, as appropriate." When a struct
    // is created in C++, it will be given a block of memory. This memory is not necessary
    // empty. Therefore we use the memset function to make sure all fields are NULL.
    memset(&host_info, 0, sizeof host_info);
    host_info.ai_family   = AF_UNSPEC;     // IP version not specified. Can be both.
    host_info.ai_socktype = SOCK_STREAM;   // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.

    dDesHeading           = 0.0;
    dDesSpeed             = 0.0;
    dCommandL             = 0.0;
    dCommandR             = 0.0;
    m_IP                  = "localhost";
    m_Port                = "29500";
    m_direct_thrust_mode  = false;
    m_heading_source      = "NAV_UPDATE";
    m_heading_msg_name    = "NAV_HEADING";
    m_bHaveMagValue       = false;
    m_magOffset           = 0.0;
    host_info_list        = NULL;
    socketfd              = 0;
    countSentToM200       = 0;
    countFromM200         = 0;
}

bool iM200::OnNewMail(MOOSMSG_LIST &NewMail)
{
    AppCastingMOOSApp::OnNewMail(NewMail);

	MOOSMSG_LIST::iterator p;
	for (p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg & rMsg = *p;
		if (MOOSStrCmp(rMsg.GetKey(), "DESIRED_HEADING"))
			HandleMail_Des_Heading(rMsg.GetDouble());
		else if(MOOSStrCmp(rMsg.GetKey(), "DESIRED_SPEED"))
			HandleMail_Des_Speed(rMsg.GetDouble());
		else if (MOOSStrCmp(rMsg.GetKey(), "KF_COMMANDED_L"))
		    HandleMail_Commanded_L(rMsg.GetDouble());
        else if (MOOSStrCmp(rMsg.GetKey(), "KF_COMMANDED_R"))
            HandleMail_Commanded_R(rMsg.GetDouble()); }
	return true;
}

bool iM200::HandleMail_Des_Heading(double d)
{
    dDesHeading = d;
    return true;
}

bool iM200::HandleMail_Des_Speed(double d)
{
    dDesSpeed = d;
    return true;
}

bool iM200::HandleMail_Commanded_L(double d)
{
    dCommandL = d;
    return true;
}

bool iM200::HandleMail_Commanded_R(double d)
{
    dCommandR = d;
    return true;
}

bool iM200::OnStartUp()
{
    AppCastingMOOSApp::OnStartUp();

    STRING_LIST sParams;
    if (!m_MissionReader.GetConfiguration(GetAppName(), sParams))
        reportConfigWarning("No config block found for " + GetAppName());

    bool goodParams = true;
    STRING_LIST::iterator p;
    for (p = sParams.begin(); p != sParams.end(); p++) {
        string orig  = *p;
        string line  = *p;
        string param = toupper(biteStringX(line, '='));
        string value = line;

        if (param == "IP_ADDRESS")
            goodParams = SetParam_IP_ADDRESS(value);
        else if (param == "PORT_NUMBER")
            goodParams = SetParam_PORT_NUMBER(value);
        else if (param == "DIRECT_THRUST_MODE")
            goodParams = SetParam_DIRECT_THRUST_MODE(value);
        else if (param == "HEADING_SOURCE")
            goodParams = SetParam_HEADING_SOURCE(value);
        else if (param == "HEADING_MSG_NAME")
            goodParams = SetParam_HEADING_MSG_NAME(value);
        else if (param == "MAG_OFFSET")
            goodParams = SetParam_MAG_OFFSET(value);
        else
            reportUnhandledConfigWarning(orig); }

    if (!OpenConnectionTCP(m_IP, m_Port))
        return false;
    if (!OpenSocket())
        return false;
    if (!Connect())
        return false;
    return true;
}

bool iM200::SetParam_IP_ADDRESS(string sVal)
{
    if (sVal.empty()) {
        reportConfigWarning("Config parameter IP_ADDRESS is blank, defaulting to 'localhost'.");
        return true; }
    //TODO: Validate that the string is a valid IP address
    m_IP = sVal;
    return true;
}

bool iM200::SetParam_PORT_NUMBER(string sVal)
{
    if (sVal.empty()) {
        m_Port = "29500";
        reportConfigWarning("Config parameter PORT_NUBMER is blank, defaulting to '29500'.");
        return true; }
    // Check that the string contains a valid number
    int x = (int) strtod(sVal.c_str(), 0);
    if (!x) {
        reportConfigWarning("Config parameter PORT_NUMBER must contain a number, not " + sVal);
        return false; }
    if (x < 0) {
        reportConfigWarning("Config parameter PORT_NUMBER must contain a positive number, not " + sVal);
        return false; }
    m_Port = sVal;
    return true;
}

bool iM200::SetParam_DIRECT_THRUST_MODE(std::string sVal)
{
    if (sVal.empty()) {
        m_direct_thrust_mode = false;
        reportConfigWarning("Config parameter DIRECT_THRUST_MODE is blank, defaulting to false.");
        return true; }
    if (sVal.at(0) == 'T' || sVal.at(0) == 't')
        m_direct_thrust_mode = true;
    else
        m_direct_thrust_mode = false;
    return true;
}

bool iM200::SetParam_HEADING_SOURCE(std::string sVal)
{
    if (sVal.empty()) {
        m_heading_source = "RAW_COMPASS";
        reportConfigWarning("Config parameter HEADING_SOURCE is blank, defaulting to 'NAV_UPDATE'.");
        return true; }
    m_heading_source = sVal;
    MOOSToUpper(m_heading_source);
    string findSrc = HEADING_OPTIONS;
    size_t found = findSrc.find(m_heading_source);
    if (found == string::npos) {
        strRpt = "In mission file, HEADING_SOURCE must be one of: ";
        strRpt += HEADING_OPTIONS;
        reportConfigWarning(strRpt);
        return false; }
    return true;
}

bool iM200::SetParam_HEADING_MSG_NAME(std::string sVal)
{
    if (sVal.empty()) {
        reportConfigWarning("Config parameter HEADING_MSG_NAME is blank, defaulting to" + m_heading_msg_name);
        return true; }

    m_heading_msg_name = sVal;
    //TODO: Validate that the string is a valid MOOS message name
    return true;
}

bool iM200::SetParam_MAG_OFFSET(std::string sVal)
{
    if (sVal.empty()) {
        m_bHaveMagValue = false;
        reportConfigWarning("Config parameter MAG_OFFSET is blank, defaulting to 0.0.");
        return true; }
    double dVal;
    m_bHaveMagValue = true;
    m_magOffset = strtod(sVal.c_str(), 0);
    return true;
}

bool iM200::OnConnectToServer()
{
    RegisterVariables();
	return true;
}

bool iM200::Iterate()
{
    AppCastingMOOSApp::Iterate();

	// Publish to vehicle on every iteration
	bool sendOK = Send();
	if (!sendOK)
	    reportRunWarning("Failure sending commands to vehicle.");

    // Receive any pending messages
	bool receiveOK = Receive();
	if (!receiveOK)
        reportRunWarning("Failure on receive from vehicle - host is probably down.");

	AppCastingMOOSApp::PostReport();
	return true;
}

void iM200::RegisterVariables()
{
    AppCastingMOOSApp::RegisterVariables();
    Register("DESIRED_HEADING",0);
    Register("DESIRED_SPEED",  0);
    Register("KF_COMMANDED_L", 0);
    Register("KF_COMMANDED_R", 0);
}

bool iM200::OpenConnectionTCP(string addr, string port)
{
    if (addr.empty() || port.empty()) {
        reportRunWarning("Invalid IP and/or port.");
        return false; }

    // Now fill the linked list of host_info struct with the connection info
    int status = getaddrinfo(addr.c_str(), port.c_str(), &host_info, &host_info_list);

    // 0 = success, otherwise failure
    if (status == 0) {
        strRpt = "Successful connection to IP ";
        strRpt += addr;
        strRpt += " Port ";
        strRpt += port;
        reportEvent(strRpt);
        return true; }
    strRpt = "Error in connection to IP ";
    strRpt += addr;
    strRpt += " Port ";
    strRpt += port;
    strRpt += ". ";
    strRpt += gai_strerror(status);
    reportRunWarning(strRpt);
    return true;
}

bool iM200::OpenSocket()
{
	socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
	int tmp = errno;
	if (socketfd == -1) {
	    strRpt = "Socket error: ";
	    strRpt += strerror(tmp);
		reportRunWarning(strRpt);
		return false; }
	else {
	    reportEvent("Socket open"); }
	return true;
}

bool iM200::Connect()
{
	int status = connect(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    int tmp = errno;
	if (status == -1) {
	    strRpt = "Connect error: ";
	    strRpt += strerror(tmp);
		reportRunWarning(strRpt);
		return false; }
	else {
        reportEvent("Connection open"); }
	return true;
}

bool iM200::Send()
{
    string message = "$";
    if (!m_direct_thrust_mode) {

        if (dDesSpeed == 0.0) {
            message += "PYDIR,0.0,0.0*\r\n"; }
        else {
            message += "PYDEV,";
            message += doubleToString(dDesHeading);
            message += ",";
            message += doubleToString(dDesSpeed);
            message += "*\r\n"; } }

    else {
        message += "PYDIR,";
        message += doubleToString(dCommandL);
        message += ",";
        message += doubleToString(dCommandR);
        message += "*\r\n"; }

    reportEvent("Sending to vehicle: " + message);

	int len;
	ssize_t bytes_sent;
	len = strlen(message.c_str());
	bytes_sent = send(socketfd, message.c_str(), len, 0);
	if (bytes_sent == -1)     // -1 == error
		return false;
	return true;
}

bool iM200::Receive()
{
    // Objective is to parse out NMEA strings from the incoming data
    ssize_t numBytes;
	char incoming[1000];
	numBytes = recv(this->socketfd, incoming, 1000, 0);
	//TODO: Handle errors from recv
	if (numBytes < 1)
	    return true;

	// We may have received part of an NMEA string earlier so concatenate to end of earlier
	//      - Check to prevent buffer overflow
	size_t inBuffLen   = strlen(inBuff);
	size_t incomingLen = strlen(incoming);
	if (inBuffLen + incomingLen < MAX_BUFF)
	    strncat(inBuff, incoming, numBytes);
	string toParse = inBuff;
	strcpy(inBuff, "");   // Clear the persistent buffer

	// Everything before the first $ is lost; we're missing the beginning of that NMEA message
	// If no $ in the message then there isn't anything to parse
	unsigned long int startNMEA = toParse.find('$');
	if (startNMEA == string::npos) {
	    strcpy(inBuff, toParse.c_str());
	    return true; }
	if (startNMEA > 0)
	    toParse = toParse.substr(startNMEA);

	// Parse string into a vector of lines
	vector<string> lines = parseString(toParse, "\r\n");
	if (lines.size() < 1)
	    return true;

	// If the last item is not a full sentence, keep it for next time
	string last = lines[lines.size() - 1];
	unsigned int lastSize = last.size();
	if (last.size() < 4 || last.at(lastSize - 3) != '*')
	    strcpy(inBuff, last.c_str());

	// Visit each string item in the vector and deal with it
	for (unsigned int i = 0; i < lines.size(); i++) {
	    string line = lines[i];
	    if (!line.empty()) {
	        if (NMEAChecksumIsValid(line)) {
	            if (line.at(0) == '$') {
	                DealWithNMEA(line); } } } }
	return true;
}

bool iM200::DealWithNMEA(string nmea)
{
    if (nmea.empty())
        return false;
    vector<string> parts = parseString(nmea, ',');
    string nmeaHeader = parts[0];

    if (nmeaHeader == "$CPNVG")
        return ParseCPNVG(nmea);
    else if (nmeaHeader == "$CPRBS")
        return ParseCPRBS(nmea);
    else if (nmeaHeader == "$CPRCM")
        return ParseCPRCM(nmea);
    else if (nmeaHeader == "$GPRMC") {
        m_Comms.Notify("NMEA_MSG", nmea);
        return ParseGPRMC(nmea); }
    else if (nmeaHeader == "$GPGGA" ||
        nmeaHeader == "$GPRMC" ||
        nmeaHeader == "$GPGST" ||
        nmeaHeader == "$GPRME") {
            m_Comms.Notify("NMEA_MSG", nmea); }
    else
        return false;
    return true;
}

// CPNVG: Navigation Update
bool iM200::ParseCPNVG(string nmea)
{
    if (m_heading_source != "NAV_UPDATE")
        return true;
    CPNVGnmea cpnvg;
    cpnvg.ParseSentenceIntoData(nmea, true);
    if (!cpnvg.CriticalDataAreValid()) {
        reportRunWarning("Could not parse CPNVG message: " + nmea);
        return false; }
    double dHeading = BAD_DOUBLE;
    if (cpnvg.Get_headingTrueN(dHeading))
        m_Comms.Notify(m_heading_msg_name, dHeading);
    return true;
}

// CPRBS: Battery voltage
bool iM200::ParseCPRBS(string nmea)
{
    CPRBSnmea cprbs;
    cprbs.ParseSentenceIntoData(nmea, true);
    if (!cprbs.CriticalDataAreValid()) {
        reportRunWarning("Could not parse CPRBS message: " + nmea);
        return false; }
    double battV = BAD_DOUBLE;
    if (cprbs.Get_battStackVoltage(battV))
        m_Comms.Notify("BATT_V", battV);
    return true;
}

// CPRCM: Raw Compass Data
bool iM200::ParseCPRCM(string nmea)
{
    nmea += "";
    if (m_heading_source == "RAW_COMPASS") {
        reportRunWarning("RAW_COMPASS mode not yet implemented");
        return true;
        // m_Comms.Notify(m_heading_msg_name, dHeading);
        }
    return true;
}

// GPRMC: Raw GPS message containing heading and some other details
bool iM200::ParseGPRMC(string nmea)
{
    if (m_heading_source != "RAW_GPS")
        return true;
    GPRMCnmea gprmc;
    gprmc.ParseSentenceIntoData(nmea, true);
    if (!gprmc.CriticalDataAreValid()) {
        reportRunWarning("Could not parse GPRMC message: " + nmea);
        return false; }
    double dHeading = BAD_DOUBLE;
    if (gprmc.Get_headingTrueN(dHeading))
        m_Comms.Notify(m_heading_msg_name, dHeading);
    return true;
}

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
    if (sExpected.length() < 2)
        sExpected = "0" + sExpected;
    return MOOSStrCmp(sExpected,sRxCheckSum);
}

bool iM200::buildReport()
{
    m_msgs << "Messages sent to M200:       " << countSentToM200;
    m_msgs << "Messages received from M200: " << countFromM200;
    return true;
}





































//


