/*
 * ChargeController.cpp
 * 
 * Created on: 2/6/2021
 * Author: Ethan Rolland
 *
 */

#include "NMEAdefs.h"
#include "MBUtils.h"
#include "ChargeController.h"

using namespace std;

CHARGE::CHARGE()
{
  //MOOS file parameters
  m_serial_port           = "/dev/tty";
  m_baudrate              = 115200;
  m_prefix                = "CHG";
  m_read_data             = false;
  m_max_thrust            = 0.0;
  m_max_rudder            = 0.0;
  
  m_serial                = NULL;
  m_bValidSerialConn      = false;
  
  currMaxThrust           = 0.0;
  currMaxRudder           = 0.0;
  
  //Publish name
  m_pubNameMaxThrust      = "MAX_THRUST";
  m_pubNameMaxRudder      = "MAX_RUDDER";
  
  //Appcast details
  m_msgs_from_device      = 0;
  m_last_msg_from_device  = "";
  
}

bool CHARGE::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
  MOOSMSG_LIST::iterator p;
  for (p=NewMail.begin(); p!=NewMail.end(); ++p) {
      CMOOSMsg &rMsg = *p;
      string key     = rMsg.GetKey();
      string sVal    = rMsg.GetString();
  }
  return UpdateMOOSVariables(NewMail);
}

void CHARGE::RegisterForMOOSMessages()
{
  AppCastingMOOSApp::RegisterVariables();
}

bool CHARGE::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  STRING_LIST sParams;
  if (!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());
    
  bool handled = true;
  STRING_LIST::iterator p;
  for (p = sParams.begin(); p != sParams.end(); p++) {
    string orig   = *p;
    string line   = *p;
    string param  = toupper(biteStringX(line, '='));
    string value  = line;

    if      (param == "PORT")            handled = SetParam_PORT(value);
    else if (param == "BAUDRATE")        handled = SetParam_BAUDRATE(value);
    else if (param == "PREFIX")          handled = SetParam_PREFIX(value);
    else if (param == "READ_DATA")       handled = SetParam_READ_DATA(value);
    else if (param == "MAX_THRUST")      handled = SetParam_MAX_THRUST(value);
    else if (param == "MAX_RUDDER")      handled = SetParam_MAX_RUDDER(value);
    else
      reportUnhandledConfigWarning(orig); }

  SetPublishNames();

  //m_bValidSerialConn = SerialSetup();
  
  RegisterForMOOSMessages();
  MOOSPause(500);

  return true;
}

bool CHARGE::OnConnectToServer()
{
	RegisterForMOOSMessages();
	return true;
}

bool CHARGE::Iterate()
{
  AppCastingMOOSApp::Iterate();
	
  if (!m_read_data) {
    currMaxThrust = m_max_thrust;
	  m_Comms.Notify(m_pubNameMaxThrust, currMaxThrust); 
    
    currMaxRudder = m_max_rudder;
    m_Comms.Notify(m_pubNameMaxRudder, currMaxRudder);
    }

  //else
  //  read data from charge controller
	
  AppCastingMOOSApp::PostReport();
  
  return true;
}

bool CHARGE::SerialSetup()
{
  string errMsg = "";
  m_serial = new SerialComms(m_serial_port, m_baudrate, errMsg);
  if (m_serial->IsGoodSerialComms()) {
    m_serial->Run();
    string msg = "Serial port opened. ";
    msg       += "Communicating on port ";
    msg       += m_serial_port;
    reportEvent(msg);
    return true; }
  reportConfigWarning("Unable to open serial port: " + errMsg);
  return false;
}

void CHARGE::GetData()
{
  if (!m_bValidSerialConn)
	return;

  //to be done
}

bool CHARGE::SetParam_PORT(std::string sVal)
{
  m_serial_port = sVal;
  if (m_serial_port.empty())
    reportConfigWarning("Mission file parameter PORT must not be blank.");
  
  return true;
}

bool CHARGE::SetParam_BAUDRATE(std::string sVal)
{
  if (sVal.empty())
    reportConfigWarning("Mission file parameter BAUDRATE must not be blank.");
  else if (sVal ==   "2400") m_baudrate = 2400;
  else if (sVal ==   "4800") m_baudrate = 4800;
  else if (sVal ==   "9600") m_baudrate = 9600;
  else if (sVal ==  "19200") m_baudrate = 19200;
  else if (sVal ==  "38400") m_baudrate = 38400;
  else if (sVal ==  "57600") m_baudrate = 57600;
  else if (sVal == "115200") m_baudrate = 115200;
  else
    reportConfigWarning("Mission file parameter BAUDRATE must be one of 2400, 4800, 9600, 19200, 38400, 57600, 115200. Unable to process: " + sVal);
    
    return true;
}

bool CHARGE::SetParam_PREFIX(std::string sVal)
{
  m_prefix = toupper(sVal);
  size_t strLen = m_prefix.length();
  if (strLen > 0 && m_prefix.at(strLen - 1) != '_')
    m_prefix += "_";
  
  return true;
}

bool CHARGE::SetParam_READ_DATA(std::string sVal)
{
  sVal = removeWhite(sVal);
  if (sVal.empty())
    sVal = "blank";
  stringstream ssMsg;
  sVal = tolower(sVal);
  if (sVal == "true" || sVal == "false")
    m_read_data = (sVal == "true");
  else {
    ssMsg << "Param READ_DATA cannot be " << sVal << ". It must be TRUE or FALSE. Defaulting to FALSE.";
    m_read_data = false; }
  string msg = ssMsg.str();
  if (!msg.empty())
    reportConfigWarning(msg);
  
  return true;
}

bool CHARGE::SetParam_MAX_THRUST(std::string sVal)
{
  stringstream ssMsg;
  if (!isNumber(sVal))
    ssMsg << "Param MAX_THRUST must be a number in range (0.0, 100.0].";
  else
    m_max_thrust = strtod(sVal.c_str(), 0);
  if (m_max_thrust <= 0.0 || m_max_thrust > 100)
    ssMsg << "Param MAX_THRUST cannot be " << m_max_thrust << ". Must be in range (0.0, 100.0].";
  string msg = ssMsg.str();
  if (!msg.empty())
    reportConfigWarning(msg);
  return true;
}

bool CHARGE::SetParam_MAX_RUDDER(std::string sVal)
{
  stringstream ssMsg;
  if (!isNumber(sVal))
    ssMsg << "Param MAX_RUDDER must be a number in range (0.0, 50.0].";
  else
    m_max_rudder = strtod(sVal.c_str(), 0);
  if (m_max_rudder <= 0.0 || m_max_rudder > 50)
    ssMsg << "Param MAX_RUDDER cannot be " << m_max_rudder << ". Must be in range (0.0, 50.0].";
  string msg = ssMsg.str();
  if (!msg.empty())
    reportConfigWarning(msg);
  return true;
}

bool CHARGE::SetPublishNames()
{
  m_prefix = toupper(m_prefix);
  size_t strLen = m_prefix.length();
  if (strLen > 0 && m_prefix.at(strLen - 1) != '_')
    m_prefix += "_";
  m_pubNameMaxThrust   = m_prefix + m_pubNameMaxThrust;
  m_pubNameMaxRudder   = m_prefix + m_pubNameMaxRudder;
  
  return true;
}

bool CHARGE::buildReport()
{
  m_msgs << endl << "DEVICE STATUS" << endl << "-------------" << endl;
  if (!m_bValidSerialConn)
    m_msgs << "*** No communications with charge controller. ***" << endl;
  //if (m_serial->IsGoodSerialComms())
  //  m_msgs << "Serial communicating properly on port " << m_serial_port << " at " << m_baudrate << " baud." << endl;
  //else
  //  m_msgs << "Serial not connected to port " << m_serial_port << " at " << m_baudrate << "baud." << endl;
    
  // Format doubles ahead of time
  string sMaxThr    = doubleToString(m_max_thrust, 1);
  string sMaxThrust = doubleToString(currMaxThrust, 1);
  string sMaxRud    = doubleToString(m_max_rudder, 1);
  string sMaxRudder = doubleToString(currMaxRudder, 1);
  
  m_msgs << endl << "SETUP" << endl << "-----" << endl;
  m_msgs << "     PORT (BAUDRATE):         " << m_serial_port << "(" << m_baudrate << ")" << endl;
  m_msgs << "     Publish PREFIX:          " << m_prefix << endl;
  m_msgs << "     MAX THRUST:           +/-" << sMaxThr << "%" << endl;
  m_msgs << "     MAX RUDDER:              " << sMaxRud << endl;
  m_msgs << endl;

  m_msgs << "   Messages from device:         " << m_msgs_from_device << endl;
  m_msgs << "   Last message from device:     " << m_last_msg_from_device << endl;
  m_msgs << "   Current Max Thrust:           " << sMaxThrust << endl;
  m_msgs << "   Current Max Rudder:           " << sMaxRudder << endl;
  m_msgs << endl;
  
  return true;
}
