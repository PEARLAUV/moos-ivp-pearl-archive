/*
 * uModemMiniPacket.cpp
 *
 *  Created on: Jun 11, 2015
 *      Author: yar
 */

#include "MBUtils.h"
#include "CAREVnmea.h"
#include "CCCFGnmea.h"
#include "minipacket.h"
#include "uModemMiniPacket.h"

using namespace std;


double miniPacketIn::soundSpeed = 0.0;

umodemMP::umodemMP()
{
  m_bValidSerialConn  = false;
  m_bShowViewCircle   = true;
  m_serial            = NULL;
  m_serial_port       = "";
  m_baudrate          = 19200;
  m_dSoundspeed       = 0.0;
  m_SRCid             = 0u;
  m_revision          = "";
  m_ident             = "";
  m_timeOn            = "";
  m_uModemState       = UMODEM_WAITING;
  m_LastRangeNote     = "";
  m_X = m_Y           = 0.0;
}

bool umodemMP::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
  MOOSMSG_LIST::iterator p;
  for (p=NewMail.begin(); p!=NewMail.end(); ++p) {
      CMOOSMsg & rMsg = *p;
      if (MOOSStrCmp(rMsg.GetKey(), "UMODEM_CMD"))
        HandleUmodemCommand(p->GetString());
      else if (MOOSStrCmp(rMsg.GetKey(), "NAV_X"))
        m_X = rMsg.GetDouble();
      else if (MOOSStrCmp(rMsg.GetKey(), "NAV_Y"))
        m_Y = rMsg.GetDouble(); }
  return UpdateMOOSVariables(NewMail);
}

void umodemMP::HandleUmodemCommand(string cmd)
{
  if (m_uModemState == UMODEM_READY) {
    retractRunWarning(NOTREADYMSG);
    minipacketOut mpo = minipacketOut(cmd);
    string toModem    = mpo.GenerateNMEAforModem(m_SRCid);
    if (toModem.empty())
      reportRunWarning("Bad UMODEM_CMD: " + cmd);
    else
      SendMessageToModem(toModem); }
  else
    reportRunWarning(NOTREADYMSG);
}

void umodemMP::SendMessageToModem(string str)
{
  if (!str.empty()) {
    string key = NMEAbase::GetKeyFromSentence(str);
    if (key.empty()) {
      key = "  BAD";
      outPacketCount[key] = outPacketCount[key] + 1;
      return; }
    else
      outPacketCount[key] = outPacketCount[key] + 1; }
  while (str.length() > 0
         && (str.at(str.length() - 1) == '\r'
         || str.at(str.length() - 1) == '\n'))
    str = str.substr(0, str.length() - 1);
  str.append("\x0D\x0A");
  stringstream ss;
  ss << "[" << str << "]  (len " << str.length() << ")";
  if (str.length() == 2)
    reportEvent("toModem: blank line");
  else
    reportEvent("toModem: " + NiceString(str));
  m_serial->WriteToSerialPort(str);
}

void umodemMP::ParseSentencesFromModem()
{
  while (m_serial->DataAvailable()) {
    string fromModem = m_serial->GetNextSentence();
    string key = NMEAbase::GetKeyFromSentence(fromModem);
    if (key != "CAREV" && key != "CATXF" && key != "CATXP" && key != "CACST")
      reportEvent("nmea string: " + NiceString(fromModem) + "   key: " + NiceString(key));
    if (key == "CAREV")
      HandleIncomingCAREV(fromModem);
    else {
      miniPacketIn mpi = miniPacketIn(fromModem);
      string toPublish = mpi.GenerateMOOSmsg();
      if (key == "CAMPR") {
        reportEvent("TO PUBLISH: " + toPublish);
        m_LastRangeNote = mpi.GetRangeNote();
        string label = "modem_" + intToString(mpi.GetDstAsInt());
        string view = mpi.Get_VIEW_CIRCLE(m_X, m_Y, label);
        if (m_bShowViewCircle && !view.empty())
          m_Comms.Notify("VIEW_CIRCLE", view); }
      if (!toPublish.empty())
        m_Comms.Notify("UMODEM_IN", toPublish); }
    if (key.empty())
        key = "BAD_in";
      inPacketCount[key] = inPacketCount[key] + 1; }
}

string umodemMP::NiceString(string str)
{
  stringstream ss;
  string::iterator it = str.begin();
  for (; it != str.end(); it++) {
    char c = static_cast<char>(*it);
    if (c < ' ' || c > '~')
      ss << "\\x" << std::hex << static_cast<int>(c);
    else
      ss << c; }
  return ss.str();
}

void umodemMP::HandleIncomingCAREV(string nmea)
{
  CAREVnmea carev;
  carev.ParseSentenceIntoData(nmea, false);
  if (carev.CriticalDataAreValid()) {
    carev.Get_revision(m_revision);
    carev.Get_ident(m_ident);
    utcTime ut;
    carev.Get_timeOn(ut);
    float fs;
    unsigned short h, m, s;
    ut.Get_utcHour(h);
    ut.Get_utcMinute(m);
    ut.Get_utcSecond(fs);
    s = (int) fs;
    m_timeOn = "";
    if (h < 10)
      m_timeOn += "0";
    m_timeOn += intToString(h);
    m_timeOn += ":";
    if (m < 10)
      m_timeOn += "0";
    m_timeOn += intToString(m);
    m_timeOn += ":";
    if (s < 10)
      m_timeOn += "0";
    m_timeOn += intToString(s); }
}

bool umodemMP::Iterate()
{
  AppCastingMOOSApp::Iterate();
  ParseSentencesFromModem();

  // Handle device state
  switch (m_uModemState) {
    case UMODEM_WAITING:
      if (inPacketCount.count("CAREV")) {
        ModemSetup();
        m_uModemState = UMODEM_SENT_CONFIG;}
      break;
    case UMODEM_SENT_CONFIG:
      if (inPacketCount.count("CACFG"))
        m_uModemState = UMODEM_READY;
      break;
    case UMODEM_READY:
      break;
    default:
      reportRunWarning("Device in unknown configuration state. Cannot communicate.");
      break; }

  AppCastingMOOSApp::PostReport();
  return true;
}

void umodemMP::ModemSetup()
{
//  SendMessageToModem("");
  CCCFGnmea cccfg;
  cccfg.Set_paramName("SRC");
  cccfg.Set_newValue((int) m_SRCid);
  string configStr = "";
  if (cccfg.ProduceNMEASentence(configStr)) {
    SendMessageToModem(configStr);
    reportEvent("Configured modem with " + configStr); }
  else
    reportRunWarning(cccfg.GetErrorString());
}

bool umodemMP::OnConnectToServer()
{
  return true;
}

bool umodemMP::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  STRING_LIST sParams;
  if (!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  bool bHandled = true;
  STRING_LIST::iterator p;
  for (p = sParams.begin(); p != sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;

    if (param == "PORT")
      bHandled = SetParam_PORT(value);
    else if (param == "BAUDRATE")
      bHandled = SetParam_BAUDRATE(value);
    else if (param == "SRC_ID")
      bHandled = SetParam_SRC_ID(value);
    else if (param == "SOUNDSPEED")
      bHandled = SetParam_SOUNDSPEED(value);
    else if (param == "TIMEOUT")
      bHandled = SetParam_TIMEOUT(value);
    else if (param == "SHOW_VIEW_CIRCLE")
      bHandled = SetParam_SHOW_VIEW_CIRCLE(value);
    else
      reportUnhandledConfigWarning(orig); }

  if (!bHandled) {
    reportConfigWarning("Invalid mission file parameters. GPS data will not be parsed.");
    m_bValidSerialConn = false;}
  else {
    m_bValidSerialConn = SerialSetup(); }
  RegisterForMOOSMessages();
  MOOSPause(500);
  return true;
}

bool umodemMP::SerialSetup()
{
  string errMsg = "";
  m_serial = new SerialComms(m_serial_port, m_baudrate, errMsg);
  if (m_serial->IsGoodSerialComms()) {
    m_serial->Run();
    string msg = "Serial port opened. ";
    msg       += "Communicating over port ";
    msg       += m_serial_port;
    reportEvent(msg);
    return true; }
  reportConfigWarning("Unable to open serial port: " + errMsg);
  return false;
}

bool umodemMP::RegisterForMOOSMessages()
{
  AppCastingMOOSApp::RegisterVariables();
  m_Comms.Register("UMODEM_CMD", 0);
  m_Comms.Register("NAV_X", 0);
  m_Comms.Register("NAV_Y", 0);
  return RegisterMOOSVariables();
}

bool umodemMP::SetParam_PORT(std::string sVal)
{
  // TODO: Validate that sVal is a valid path to describe a serial port
  m_serial_port = sVal;
  if (m_serial_port.empty())
    reportConfigWarning("Mission file parameter PORT must not be blank.");
  return true;
}

bool umodemMP::SetParam_BAUDRATE(std::string sVal)
{
  if (sVal.empty())
    reportConfigWarning("Mission file parameter BAUDRATE may not be blank.");
  else if (sVal ==   "2400") m_baudrate = 2400;
  else if (sVal ==   "4800") m_baudrate = 4800;
  else if (sVal ==  "19200") m_baudrate = 19200;
  else if (sVal ==  "38400") m_baudrate = 38400;
  else if (sVal ==  "57600") m_baudrate = 57600;
  else if (sVal == "115200") m_baudrate = 115200;
  else
    reportConfigWarning("Mission file parameter BAUDRATE must be one of 2400, 4800, 9600, 19200, 38400, 57600, 115200. Unable to process: " + sVal);
  return true;
}

bool umodemMP::SetParam_SRC_ID(std::string sVal)
{
  if (sVal.empty())
    reportConfigWarning("Mission file parameter SRC_ID must not be blank.");
  else {
    int src = strtol(sVal.c_str(), 0, 0);
    if (src < 1 || src > 127)
      reportConfigWarning("Mission file parameter SRC_ID must be in range 1 to 127, inclusive.");
    else
      m_SRCid = src; }
  return true;
}

bool umodemMP::SetParam_SOUNDSPEED(std::string sVal)
{
  if (sVal.empty()) {
    stringstream ss;
    ss << "Mission file parameter SOUNDSPEED must not be blank. Using default value of ";
    ss << m_dSoundspeed << ".";
    reportConfigWarning(ss.str());
    return true; }
  int iSS = (int) strtod(sVal.c_str(), 0);
  if (iSS <= 0) {
    stringstream ss;
    ss << "Mission file parameter SOUNDSPEED must be > 0. Using default value of ";
    ss << m_dSoundspeed << ".";
    reportConfigWarning(ss.str()); }
  else
    m_dSoundspeed = iSS;
  miniPacketIn::soundSpeed = m_dSoundspeed;
  return true;
}

bool umodemMP::SetParam_TIMEOUT(std::string sVal)
{
  m_serial_port = sVal;
  if (m_serial_port.empty())
    reportConfigWarning("Mission file parameter PORT must not be blank.");
  return true;
}

bool umodemMP::SetParam_SHOW_VIEW_CIRCLE(std::string sVal)
{
  setBooleanOnString(m_bShowViewCircle, sVal, true);
  return true;
}

bool umodemMP::buildReport()
{
  m_msgs <<   "Configuration  |" << "     Serial port:   " << m_serial_port << endl;
  m_msgs <<   "--------------- " << "     Baudrate:      " << m_baudrate << endl;
  m_msgs <<   "                     Soundspeed:          " << m_dSoundspeed << endl;
  m_msgs <<   "                     Modem ID:            " << (int) m_SRCid << endl;
  m_msgs <<   "                     Publish VIEW_CIRCLE: " << boolToString(m_bShowViewCircle) << endl << endl;
  m_msgs <<   "-------------------------------------------------------------------" << endl;
  m_msgs <<   "System Status  |     ";
  switch(m_uModemState) {
    case UMODEM_WAITING:
      m_msgs <<   "Waiting for $CAREV message from device." << endl; break;
    case UMODEM_SENT_CONFIG:
      m_msgs <<   "Received $CAREV; verifying configuration." << endl; break;
    case UMODEM_READY:
      m_msgs <<   "Device is configured and ready to use." << endl; break;
    default:
      m_msgs <<   "Device is in unknown state." << endl; break; }
  m_msgs <<   "--------------- " << endl << endl;

  m_msgs <<   "-------------------------------------------------------------------" << endl;
  m_msgs <<   "Connection Status  |";
  m_msgs <<   "        Serial connection: " << (m_serial->IsGoodSerialComms() ? "CONNECTED" : "Not connected") << endl;
  m_msgs <<   "------------------- ";
  if (!m_timeOn.empty()) {
    m_msgs << "        Time since power on: " << m_timeOn << endl;
    m_msgs << "                            ident: " << m_ident;
    m_msgs << "   revision: " << m_revision << endl; }
  else
    m_msgs << endl;

  m_msgs << endl;
  m_msgs <<   "-------------------------------------------------------------------" << endl;
  m_msgs << endl;
  m_msgs << (!m_LastRangeNote.empty() ? m_LastRangeNote : "") << endl << endl;
  m_msgs <<   "Outgoing Packet Counts" << endl;
  m_msgs <<   "----------------------" << endl;
  map<string, int>::iterator it = outPacketCount.begin();
  for (;it != outPacketCount.end(); it++)
    m_msgs << "   $" << it->first << ":    " << it->second << endl;
  m_msgs << endl;
  m_msgs <<   "Incoming Packet Counts" << endl;
  m_msgs <<   "----------------------" << endl;
  it = inPacketCount.begin();
  for (;it != inPacketCount.end(); it++)
    m_msgs << "   $" << it->first << ":    " << it->second << endl;
  return true;
}
