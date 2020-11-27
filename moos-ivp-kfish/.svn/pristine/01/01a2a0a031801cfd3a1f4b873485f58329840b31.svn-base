/*
 * GPSsim.cpp
 *
 *  Created on: Jan 16, 2013
 *      Author: Alon Yaari
 */

#include "MOOS/libMOOS/Utils/MOOSThread.h"
#include "M200Sim.h"
#include "MBUtils.h"
#include "NMEAdefs.h"


using namespace std;

M200Sim::M200Sim()
{
    m_LastPublishModeTime = 0.0;
    m_LastConnectionTime  = 0.0;
    m_LastValidPYDIRtime  = 0.0;
    m_iPort               = 29500;
    m_curSpeed            = BAD_DOUBLE;
    m_curHeading          = BAD_DOUBLE;
    m_curThrustL          = BAD_DOUBLE;
    m_curThrustR          = BAD_DOUBLE;
    m_curLat              = BAD_DOUBLE;
    m_curLon              = BAD_DOUBLE;
    m_CPNVGlastTime       = 0.0;
    m_CPNVGdelay          = 0.20;     // 5 Hz
    m_CPRBSlastTime       = 0.0;
    m_CPRBSdelay          = 2.0;
    m_GPGGAlastTime       = 0.0;
    m_GPGGAdelay          = 0.20;     // 5 Hz
    m_GPRMClastTime       = 0.0;
    m_GPRMCdelay          = 0.20;     // 5 Hz
    m_battStartTime       = MOOSTime() - 1.0;
    m_server              = 0;
    m_bClientConnected    = false;
    m_curThrustMode       = THRUST_MODE_UNKNOWN;

    // Init Appcasting variables
    m_curClientRxNum      = 0;
    m_curClientTxNum      = 0;
    m_lastRxTime          = -1.0;
    m_lastTxTime          = -1.0;
}

bool M200Sim::OnNewMail(MOOSMSG_LIST &NewMail)
{
    AppCastingMOOSApp::OnNewMail(NewMail);
    MOOSMSG_LIST::iterator p;
    for (p = NewMail.begin(); p != NewMail.end(); p++) {
        CMOOSMsg & rMsg = *p;
        string key = rMsg.GetKey();
        double d   = rMsg.GetDouble();
        if      (MOOSStrCmp(key, "NAV_LAT"))        m_curLat     = d;
        else if (MOOSStrCmp(key, "NAV_LONG"))       m_curLon     = d;
        else if (MOOSStrCmp(key, "NAV_LON"))        m_curLon     = d;
        else if (MOOSStrCmp(key, "NAV_HEADING"))    m_curHeading = d;
        else if (MOOSStrCmp(key, "NAV_SPEED"))      m_curSpeed   = d; }
    return UpdateMOOSVariables(NewMail);
}

bool M200Sim::Iterate()
{
  AppCastingMOOSApp::Iterate();
  HandleServerAppCasts();
  double timeNow     = MOOSTime();
  m_bClientConnected = m_server->ClientAttached();
  if (m_bClientConnected) {

    // Incoming
    ReadIncoming();

    // Outgoing
    ParseCurrentTime();
    if (timeNow - m_CPNVGlastTime > m_CPNVGdelay) {
      m_CPNVGlastTime = timeNow;
      PublishCPNVG(); }
    if (timeNow - m_CPRBSlastTime > m_CPRBSdelay) {
      m_CPRBSlastTime = timeNow;
      PublishCPRBS(); }
    if (timeNow - m_GPGGAlastTime > m_GPGGAdelay) {
      m_GPGGAlastTime = timeNow;
      PublishGPGGA(); }
    if (timeNow - m_GPRMClastTime > m_GPRMCdelay) {
      m_GPRMClastTime = timeNow;
      PublishGPRMC(); }
    m_LastConnectionTime = timeNow; }
  else {
    m_curClientTxNum = 0;
    m_curClientRxNum = 0;
    m_lastRxTime     = 0.0;
    m_lastTxTime     = 0.0; }

  switch (m_curThrustMode) {
    case THRUST_MODE_DIFFERENTIAL: {
            if (m_LastValidPYDIRtime) {
              double watchdog = timeNow - m_LastValidPYDIRtime;
              if (watchdog > WATCHDOG_TIME) {
                reportEvent("Watchdog timeout, halting motors.");
                m_LastValidPYDIRtime = 0.0;
                m_curThrustL = 0.0;
                m_curThrustR = 0.0;
                CommandThrust_LR(m_curThrustL, m_curThrustR); } } }
            break;
    case THRUST_MODE_HEADING_SPEED:
            break;
    case THRUST_MODE_NORMAL:
            break;
    case THRUST_MODE_UNKNOWN:
            break;
    default:
            break; }

  AppCastingMOOSApp::PostReport();
  return true;
}

void M200Sim::CommandThrust_LR(double left, double right)
{
  m_Comms.Notify("DESIRED_THRUST_L", left);
  m_Comms.Notify("DESIRED_THRUST_R", right);
}

bool M200Sim::ReadIncoming()
{
    if (!m_bClientConnected)
        return false;
    int i = m_server->DataAvailable();
    while (i > 0) {
        m_curClientRxNum++;
        m_lastRxTime = MOOSTime();
        DealWithNMEA(m_server->GetNextSentence());
        i = m_server->DataAvailable(); }
    return true;
}
bool M200Sim::OnConnectToServer()
{
    return true;
}

bool M200Sim::OnStartUp()
{
    AppCastingMOOSApp::OnStartUp();
    string sVal;
    double dVal;
    bool bHandled = true;
    if (m_MissionReader.GetConfigurationParam("PORT_NUMBER", dVal)
            || m_MissionReader.GetConfigurationParam("PORT", dVal))
        bHandled &= SetParam_PORT(dVal);

    RegisterForMOOSMessages();

    // Initialize and Launch the server
    m_server = new m200SimServer(m_iPort);
    m_server->Run();
    MOOSPause(500);
    if (!m_server->IsListening())
        reportConfigWarning("Server cannot start listening.");
    return true;
}


void M200Sim::RegisterForMOOSMessages()
{
    AppCastingMOOSApp::RegisterVariables();

    m_Comms.Register("NAV_X");
    m_Comms.Register("NAV_Y");
    m_Comms.Register("NAV_LAT");
    m_Comms.Register("NAV_LATITUDE");
    m_Comms.Register("NAV_LONG");
    m_Comms.Register("NAV_LONGITUDE");
    m_Comms.Register("NAV_LON");        // Just in case it gets published this way
    m_Comms.Register("NAV_HEADING");
    m_Comms.Register("NAV_SPEED");
    RegisterMOOSVariables();
}

bool M200Sim::SetParam_PORT(double dVal)
{
  if (dVal > 0.0 && dVal < 65335.0)
    m_iPort = (int) dVal;
  return true;
}

bool M200Sim::DealWithNMEA(string nmea)
{
  string key = NMEAbase::GetKeyFromSentence(nmea);
  if (MOOSStrCmp(key, "PYDEV"))   return ReceivePYDEV(nmea);
  if (MOOSStrCmp(key, "PYDEP"))   return ReceivePYDEP(nmea);
  if (MOOSStrCmp(key, "PYDIR"))   return ReceivePYDIR(nmea);
  reportRunWarning("Received input from client that cannot be processed: "+ nmea);
  return true;
}

bool M200Sim::PublishToClient(string str)
{
    if (!str.empty()) {
        m_curClientTxNum++;
        m_lastTxTime = MOOSTime();
        m_server->SendToClient(str); }
    return true;
}

bool M200Sim::ParseCurrentTime()
{
    timeval tp;
    gettimeofday(&tp, NULL);
    time_t now               = tp.tv_sec;
    tm *t                    = localtime(&now);
    unsigned short hour      = t->tm_hour;
    unsigned short minute    = t->tm_min;
    float second             = (float) t->tm_sec + tp.tv_usec / 1000000.0;
    return m_timeNow.Set_utcTime(hour, minute, second);
}

bool M200Sim::PublishCPRBS()
{
    // Make up some battery voltages
    //      - Battery voltage reduces asymptotically over time
    //      - Max battery voltage is always 1% higher than the batt voltage
    //      - Min battery voltage is inverse to speed
    double timeDiff = (MOOSTime() - m_battStartTime); // / 50.0;
    double battVnow = LOWEST_BATT_V + (25 / timeDiff);
    double battVmax = battVnow * 1.01;
    double battVmin = battVnow - (m_curSpeed / 2.0);

    CPRBSnmea cprbs;
    cprbs.Set_timestamp(m_timeNow);
    cprbs.Set_battStackVoltage(battVnow);
    cprbs.Set_battMin(battVmin);
    cprbs.Set_battMax(battVmax);
    cprbs.Set_tempC(0.0);

    if (cprbs.CriticalDataAreValid()) {
        string outSentence;
        if (cprbs.ProduceNMEASentence(outSentence)) {
            PublishToClient(outSentence);
            return true; } }
    reportRunWarning("Could not produce CPRBS sentence to send to client.");
    return false;

}

bool M200Sim::PublishGPGGA()
{
    // number of satellites is random as a function of the time
    int seed = MOOSTime() / 100.0;
    unsigned short satnum = 8 + (unsigned short) (seed % 4);

    GPGGAnmea gpgga;
    gpgga.Set_timeUTC(m_timeNow);
    gpgga.Set_latlonValues(m_curLat, m_curLon);
    gpgga.Set_gpsQual('2');
    gpgga.Set_satNum(satnum);

    bool bValid = gpgga.CriticalDataAreValid();
    if (bValid) {
        string outSentence;
        if (gpgga.ProduceNMEASentence(outSentence)) {
            PublishToClient(outSentence);
            return true; } }
    reportRunWarning("Could not produce GPGGA sentence to send to client.");
    return false;
}

bool M200Sim::PublishCPNVG()
{
    CPNVGnmea cpnvg;
    cpnvg.Set_utcTime(m_timeNow);
    cpnvg.Set_latlonValues(m_curLat, m_curLon);
    cpnvg.Set_posQual('1');
    cpnvg.Set_altBottom(0.0);
    cpnvg.Set_depthTop(0.0);
    cpnvg.Set_headingTrueN(m_curHeading);
    cpnvg.Set_roll(0.0);
    cpnvg.Set_pitch(0.0);
    cpnvg.Set_navTimestamp(m_timeNow);
    bool bValid = cpnvg.CriticalDataAreValid();
    if (bValid) {
        string outSentence;
        if (cpnvg.ProduceNMEASentence(outSentence)) {
            PublishToClient(outSentence);
            return true; } }
    reportRunWarning("Could not produce CPNVG sentence to send to client.");
    return false;
}


bool M200Sim::PublishGPRMC()
{
    utcDate theDate;
    theDate.Set_utcDate(1, 7, 14);

    GPRMCnmea gprmc;
    gprmc.Set_timeUTC(m_timeNow);
    gprmc.Set_status('A');
    gprmc.Set_latlonValues(m_curLat, m_curLon);
    gprmc.Set_speedKTS(m_curSpeed);
    gprmc.Set_headingTrueN(m_curHeading);
    gprmc.Set_dateUTC(theDate);
    gprmc.Set_magVarValue(0.0);
    gprmc.Set_modeIndicator('D');

    if (gprmc.CriticalDataAreValid()) {
        string outSentence;
        if (gprmc.ProduceNMEASentence(outSentence)) {
            PublishToClient(outSentence);
            return true; } }
        reportRunWarning("Could not produce GPRMC sentence to send to client.");
    return false;
}

//      $PYDEP,<1>,<2>*hh<CR><LF>
//      <1>  [DesYawRate] Desired rate of yaw, in radians/sec.
//      <2>  [DesThrustPct] Desired percent of thrust, -100 to 100.
//           Stopped = 0, positive thrust = forward motion.
bool M200Sim::ReceivePYDEP(string nmea)
{
    // AS OF OCTOBER 2014
    //      There is no MOOS app that attempts to actuate the M200
    //      using the PYDEP command.
    if (m_curThrustMode != THRUST_MODE_HEADING_SPEED)
        m_Comms.Notify("THRUST_MODE_DIFFERENTIAL", "FALSE");
    PYDEPnmea   pydep;
    if (!pydep.ParseSentenceIntoData(nmea, ALLOW_BLANK_CHECKSUM)) {
        reportRunWarning("Error in incoming PYDEP sentence: " + nmea);
        return false; }

    m_Comms.Notify("YAWTHRUSTMODE",    "TRUE");
    m_Comms.Notify("HEADINGSPEEDMODE", "FALSE");
    m_Comms.Notify("DIRECTTHRUSTMODE", "FALSE");
    reportRunWarning("PYDEP not yet implemented; client attempting to use PYDEP to control vehicle.");
    return true;
}

//      $PYDEV,<1>,<2>*hh<CR><LF>
//      <1>  [DesHeading] Desired heading relative to true north, 0 to 359.
//      <2>  [DesSpeed] Desired speed over ground, 0.0 and positive real numbers.
bool M200Sim::ReceivePYDEV(string nmea)
{
    if (m_curThrustMode != THRUST_MODE_HEADING_SPEED)
        m_Comms.Notify("THRUST_MODE_DIFFERENTIAL", "FALSE");
    PYDEVnmea   pydev;
    if (!pydev.ParseSentenceIntoData(nmea, ALLOW_BLANK_CHECKSUM)) {
        reportRunWarning("Error in incoming PYDEV sentence: " + nmea);
        return false; }

    double desHeading = 0.0;
    double desSpeed   = 0.0;
    if (!pydev.Get_desHeadingAndSpeed(desHeading, desSpeed)) {
        reportRunWarning("Error in incoming PYDEV sentence: " + nmea);
        return false; }

    stringstream updateStr;
    m_Comms.Notify("DESIRED_HEADING", desHeading);
    m_Comms.Notify("DESIRED_SPEED", desSpeed);
    return true;
}

//      $PYDIR,<1>,<2>*hh<CR><LF>
//      <1>  [DesThrustPctL] Desired percent of thrust for the portside motor, -100 to 100.
//      <2>  [DesThrustPctR] Desired percent of thrust for the starboard motor, -100 to 100.
bool M200Sim::ReceivePYDIR(string nmea)
{
  bool bRetVal = true;
  if (m_curThrustMode != THRUST_MODE_DIFFERENTIAL) {
    m_Comms.Notify("THRUST_MODE_DIFFERENTIAL", "TRUE");
    m_curThrustMode = THRUST_MODE_DIFFERENTIAL; }
  PYDIRnmea   pydir;
  if (!pydir.ParseSentenceIntoData(nmea, ALLOW_BLANK_CHECKSUM)) {
    reportRunWarning("Error in incoming PYDIR sentence: " + nmea + "  " + pydir.GetErrorString());
    m_curThrustL = 0.0;
    m_curThrustR = 0.0;
    bRetVal      = false; }
  else if (!pydir.Get_desThrustPctLandR(m_curThrustL, m_curThrustR)) {
    reportRunWarning("Error in incoming PYDIR sentence: " + nmea);
    m_curThrustL = 0.0;
    m_curThrustR = 0.0;
    bRetVal      = false; }

  if (!m_LastValidPYDIRtime)
    reportEvent("Differential thrust started.");
  m_LastValidPYDIRtime = MOOSTime();
  CommandThrust_LR(m_curThrustL, m_curThrustR);
  return bRetVal;
}

void M200Sim::HandleServerAppCasts()
{
    string msg;
    int type = m_server->GetNextAppCastMsg(msg);
    while (type != AC_NONE) {
        switch (type) {
            case AC_EVENT:      reportEvent(msg);       break;
            case AC_ERROR:
            case AC_WARNING:    reportRunWarning(msg);  break;
            default:                                    break; }
        type = m_server->GetNextAppCastMsg(msg); }
}


bool M200Sim::buildReport()
{
    // Sample Appcast message when no client connected:
/*
       Frontseat server ready and listening for client on port 29500.
*/

    // Sample Appcast message when client is connected:
/*
       Currently connected to a backseat client on port 29500.
       NMEA Sentences:
         Number Tx: 8675      Sec since last Tx:
         Number Rx: 309       Sec since last Rx:
       Client command uses: PYDIR
       Rx and Tx NMEA sentences are being shown in the console window.
*/

  if (!m_bClientConnected) {
    m_msgs << "NO clients currently connected." << endl;
    m_msgs << "Frontseat server ready and listening for client on port " << m_iPort <<"." << endl;
    if (m_LastConnectionTime) {
      int timeLag = (int) (MOOSTime() - m_LastConnectionTime);
      m_msgs << "Client disconnected " << timeLag << " seconds ago." << endl; } }
  else {
    double rxDiff = MOOSTime() - m_lastRxTime;
    double txDiff = MOOSTime() - m_lastTxTime;
    string rXTime = "never";
    string tXTime = "never";
    if (m_lastRxTime > 0.0) {
      if (m_lastRxTime < 1.0)
        rXTime = "< 1.0";
      else
        rXTime = doubleToString(txDiff, 1); }
    if (m_lastTxTime > 0.0) {
      if (m_lastTxTime < 1.0)
        tXTime = "< 1.0";
      else
        tXTime = doubleToString(txDiff, 1); }

    m_msgs << "Currently connected to a backseat client on port " << m_iPort << "." << endl;
    m_msgs << " ------------ " << endl;
    m_msgs << "NMEA sentences:" << endl;
    m_msgs << "  Number Rx: " << m_curClientRxNum;
    m_msgs << "          Sec since last Rx: " << rXTime << endl;
    m_msgs << "  Number Tx: " << m_curClientTxNum;
    m_msgs << "          Sec since last Tx: " << tXTime << endl;
    m_msgs << "Current thrust mode:     ";
    switch (m_curThrustMode) {
      case THRUST_MODE_DIFFERENTIAL:
        m_msgs << "DIFFERENTIAL" << endl;
    m_msgs << "       THRUST_L: " << m_curThrustL << endl;
    m_msgs << "       THRUST_R: " << m_curThrustR << endl;
        break;
      case THRUST_MODE_HEADING_SPEED: m_msgs << "HEADING-SPEED";          break;
      case THRUST_MODE_NORMAL:        m_msgs << "NORMAL (rudder-thrust)"; break;
      case THRUST_MODE_UNKNOWN:
      default:                        m_msgs << "UNKNOWN";                break; } }
  return true;
}




// ATTIC


// Convert thrust_L and thrust_R into thrust and rudder angle
// double desThrust = (desL + desR) / 2.0;
// double desRudder = desL - desR;
// m_Comms.Notify("DESIRED_RUDDER", desRudder);
// m_Comms.Notify("DESIRED_THRUST", desThrust);














//
