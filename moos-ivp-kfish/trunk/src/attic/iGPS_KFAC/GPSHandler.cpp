/*
 * GPSInstrument.cpp
 *
 *  Major Revision on: July 9, 2014
 *      Author: Alon Yaari
 */
#include <cstring>
#include "MBUtils.h"
#include "XYCircle.h"
#include "GPSHandler.h"
#include "AngleUtils.h"


using namespace std;


GPSHandler::GPSHandler()
{
    bIsSerial            = false;
    bValid               = false;
    bShowCEP             = false;
    bPublishHeading      = true;
    bValidXY             = false;
    bReportNMEA          = false;
    curX                 = 0.0;
    curY                 = 0.0;
    curLat               = 0.0;
    curLon               = 0.0;
    curSpeed             = 0.0;
    curHeading           = 0.0;
    curPosError          = 0.0;
    curRoll              = 0.0;
    curPitch             = 0.0;
    curUTC				 = 0.0;
    headingOffset        = 0.0;
    m_prefix             = "GPS_";
    serialPort           = "";
    baudRate             = 19200;
    triggerMsg           = "";
    bReverseMode         = false;

    bHaveMagVarToPublish = false;
    bNMEAfromMsg         = false;
    bSwitchPitchRoll     = false;

    serial               = NULL;

    totalGPGGA           = 0;
    totalGPGST           = 0;
    totalGPHDT           = 0;
    totalGPRMC           = 0;
    totalGPRME           = 0;
    totalPASHR           = 0;
    totalBAD             = 0;
    nmeaMsgCounter       = 0;
    ignoreFirstMsgs      = 0;
}

// AddToPublishQueue()
//        Adds a double value to the outgoing map for publishing MOOS messages
//        Handles adding the prefix to the message name
//        Overwrites any previously-stored value for the key name
//        Adds heading offset to prefix_HEADING messages
bool GPSHandler::AddToPublishQueue(double dVal, string sMsgName)
{
    gpsPub toPub;
    toPub.isDouble     = true;
    bool bIsHeading = MOOSStrCmp(sMsgName, "HEADING");

    // Heading: headingOffset was applied when heading was parsed from source data. Here we just make sure value is [0, 360].
    if (bIsHeading)
    	dVal           = angle360(dVal);
    toPub.dVal         = dVal;
    toPub.sVal         = "";

    // Publish with GPS_ prefix
    string msgName     = "GPS_" + sMsgName;
    toPublish[msgName] = toPub;

    // Publish with moos-file-specified prefix
    if (!MOOSStrCmp(m_prefix, "GPS_")) {

    	// Heading: this version of heading gets flipped in reverse mode
    	if (bIsHeading && bReverseMode)
    		toPub.dVal = angle360(dVal + 180.0);
    	msgName        = m_prefix + sMsgName;
        toPublish[msgName] = toPub; }
    return true;
}

// AddToPublishQueue()
//        Adds a string value to the outgoing map for publishing MOOS messages
//        Handles adding the prefix to the message name
//        Overwrites any previously-stored value for the key name
bool GPSHandler::AddToPublishQueue(string sVal, string sMsgName)
{
    gpsPub toPub;
    toPub.sVal = sVal;
    toPub.isDouble = false;
    toPub.dVal = BAD_DOUBLE;
    string msgName = m_prefix + sMsgName;
    toPublish[msgName] = toPub;
    return true;
}

// PublishTheQueue()
//        Publishes the list of messages waiting in the map
bool GPSHandler::PublishTheQueue(string nmeaKey)
{
    bool bGood = true;
    if (triggerMsg.empty() || MOOSStrCmp(nmeaKey, triggerMsg)) {
        map<string, gpsPub>::iterator it;
        for (it = toPublish.begin(); it != toPublish.end(); ++it) {
            string msgName = it->first;
            gpsPub gp = it->second;
            if (gp.isDouble)
                bGood &= m_Comms.Notify(msgName, gp.dVal);
            else
                bGood &= m_Comms.Notify(msgName, gp.sVal); }
        toPublish.clear(); }
    return bGood;
}

bool GPSHandler::Iterate()
{
    AppCastingMOOSApp::Iterate();
    if (!bNMEAfromMsg)
        GetData();
    AppCastingMOOSApp::PostReport();
    return true;
}

bool GPSHandler::OnStartUp()
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

      if (param == "TYPE")
          goodParams = SetParam_TYPE(value);
      else if (param == "SHOW_CEP")
          goodParams = SetParam_SHOW_CEP(value);
      else if (param == "PREFIX")
          goodParams = SetParam_PREFIX(value);
      else if (param == "PORT")
          goodParams = SetParam_PORT(value);
      else if (param == "BAUDRATE")
          goodParams = SetParam_BAUDRATE(value);
      else if (param == "TRIGGER_MSG")
          goodParams = SetParam_TRIGGER_MSG(value);
      else if (param == "PUBLISH_HEADING")
          goodParams = SetParam_PUBLISH_HEADING(value);
      else if (param == "REPORT_NMEA")
          goodParams = SetParam_REPORT_NMEA(value);
      else if (param == "HEADING_OFFSET")
          goodParams = SetParam_HEADING_OFFSET(value);
      else if (param == "SWITCH_PITCH_ROLL")
          goodParams = SetParam_SWITCH_PITCH_ROLL(value);
      else
          reportUnhandledConfigWarning(orig); }

    double dfLatOrigin, dfLongOrigin;
    bool geoOK = m_MissionReader.GetValue("LatOrigin", dfLatOrigin);
    if (!geoOK) {
        goodParams = false;
        reportConfigWarning("Latitude origin missing in MOOS file."); }
    else {
        geoOK = m_MissionReader.GetValue("LongOrigin", dfLongOrigin);
        if (!geoOK) {
            goodParams = false;
            reportConfigWarning("Longitude origin missing in MOOS file."); } }

    if (geoOK) {
        geoOK = m_Geodesy.Initialise(dfLatOrigin, dfLongOrigin);
        if (!geoOK) {
            goodParams = false;
            reportConfigWarning("Could not initialize geodesy with given origin."); }
        else
            reportEvent("Geodesy initialized with the given origin."); }

    if (!goodParams) {
        reportConfigWarning("Invalid mission file parameters. No attempt to parse GPS data.");
        bValid = false;}
    else {
        RegisterForMOOSMessages();
        PrefixCleanup();
        bValid = SerialSetup(); }

    return true;
}

bool GPSHandler::SerialSetup()
{
    if (!bIsSerial) {
        reportEvent("Listening for sentences to arrive as NMEA_MSG messages.");
        return true; }
    string errMsg = "";
    serial = new SerialComms(serialPort, baudRate, errMsg);
    if (serial->IsGoodSerialComms()) {
        serial->Run();
        reportEvent("Serial port opened. Listening for incoming serial data on port " + serialPort);
        return true; }
    reportConfigWarning("Unable to open serial port: " + errMsg);
    return false;
}

bool GPSHandler::RegisterForMOOSMessages()
{
    AppCastingMOOSApp::RegisterVariables();
    m_Comms.Register("NMEA_MSG", 0);
    m_Comms.Register("THRUST_MODE_REVERSE", 0);
    return RegisterMOOSVariables();
}

bool GPSHandler::OnNewMail(MOOSMSG_LIST &NewMail)
{
    AppCastingMOOSApp::OnNewMail(NewMail);
    MOOSMSG_LIST::iterator p;
    for (p=NewMail.begin(); p!=NewMail.end(); ++p) {
        CMOOSMsg & rMsg = *p;

        if (MOOSStrCmp(rMsg.GetKey(), "NMEA_MSG")) {
            nmeaMsgCounter++;
            string sVal = p->GetString();
            if (!sVal.empty())
                ParseNMEAString(sVal); }
        if (MOOSStrCmp(rMsg.GetKey(), "THRUST_MODE_REVERSE")) {
        	HandleTHRUST_MODE_REVERSE(p->GetString()); }  }

    return UpdateMOOSVariables(NewMail);
}

bool GPSHandler::HandleTHRUST_MODE_REVERSE(string sVal)
{
	if (sVal.empty())
		return false;
	bReverseMode = (MOOSStrCmp(sVal, "true"));
	return true;
}

bool GPSHandler::OnConnectToServer()
{
    RegisterForMOOSMessages();
    return true;
}

// PrefixCleanup()
//        Ensures only one '_' after the prefix
//        Allows mission file to have "GPS_" or "GPS" defined
void GPSHandler::PrefixCleanup()
{
    string pre = m_prefix;
    if (!pre.empty()) {
        if (pre.at(pre.length() - 1) != '_')
            pre += '_'; }
}

void GPSHandler::GetData()
{
    if (!bIsSerial)
        return;
    while (serial->DataAvailable()) {
        string nmea = serial->GetNextSentence();
        ParseNMEAString(nmea); }
}

bool GPSHandler::GeodeticConversion()
{
    double dfXLocal, dfYLocal;
    bValidXY = m_Geodesy.LatLong2LocalUTM(curLat, curLon, dfYLocal, dfXLocal);
    if (bValidXY) {
        curX = dfXLocal;
        curY = dfYLocal;
        bValidXY = true; }
    return bValidXY;
}

bool GPSHandler::ParseNMEAString(string nmea)
{
	if (ignoreFirstMsgs++ < 10)
		return true;
    if (nmea.empty()) {
        reportRunWarning("Received empty NMEA_MSG.");
        return false; }

    bool bProcessedOK = false;
    string key = NMEAbase::GetKeyFromSentence(nmea);
    if      (MOOSStrCmp(key, "GPGGA"))
        bProcessedOK = HandleGPGGA(nmea);
    else if (MOOSStrCmp(key, "GPGST"))
        bProcessedOK = HandleGPGST(nmea);
    else if (MOOSStrCmp(key, "GPHDT"))
        bProcessedOK = HandleGPHDT(nmea);
    else if (MOOSStrCmp(key, "GPRMC"))
        bProcessedOK = HandleGPRMC(nmea);
    else if (MOOSStrCmp(key, "GPRME"))
        bProcessedOK = HandleGPRME(nmea);
    else if (MOOSStrCmp(key, "HEHDT"))
        bProcessedOK = HandleGPHDT(nmea);
    else if (MOOSStrCmp(key, "GPTXT"))
        bProcessedOK = HandleGPTXT(nmea);
    else if (MOOSStrCmp(key, "PASHR"))
        bProcessedOK = HandlePASHR(nmea);
    else {
        if (bReportNMEA)
            reportRunWarning("Unparsed NMEA_MSG: " + nmea); }
    if (!bProcessedOK) {
        totalBAD++;
        return false; }
    return true;
}


//     $GPGGA,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,M,<10>,M,<11>,<12>*hh<CR><LF>
//     <1>  UTC time, format hhmmss.s
//     <2>  Lat, format ddmm.mmmmm (with leading 0s)
//     <3>  Lat hemisphere, N(+) or S(-)
//     <4>  Lon, format dddmm.mmmmm (with leading 0s)
//     <5>  Lon hemisphere, E(+) or W(-)
//     <6>  GPS Quality, 0=No fix, 1=Non-diff, 2=Diff, 6=estimated
//     <7>  Number of Satellites
//     <8>  HDOP, 0.5 to 99.9
//     <9>  Alt above MSL -9999.99 to 999999.9 meters
//     <10> Alt MSL units, M=meters
//     <11> Geoid separation -999.9 to 9999.9 meters
//     <12> Geoid separation units, M=meters
//     <13> Age of differential correction, <blank> when not using differential
//     <14> Differential station ID, 0000 when not using differential
bool GPSHandler::HandleGPGGA(string nmea)
{
    GPGGAnmea gpgga;
    if (!gpgga.ParseSentenceIntoData(nmea, false)) {
        reportRunWarning("Could not parse GPGGA message: " + nmea + "    " + gpgga.GetErrorString());
        return false; }
    if (bReportNMEA)
        reportEvent("Parsing GPGGA sentence: " + nmea);
    if (!gpgga.Get_latlonValues(curLat, curLon)) {
        reportRunWarning("GPGGA message contains invalid lat/lon: " + nmea);
        return false; }
    GeodeticConversion();
    curUTC = 0.0;
    utcTime aUTC;
    if (gpgga.Get_timeUTC(aUTC)) {
        string strUTC;
        if (aUTC.Get_utcTimeString(strUTC))
            curUTC = strtod(strUTC.c_str(), 0); }
    AddToPublishQueue(curUTC,     "UTC");
    AddToPublishQueue(curLat,     "LATITTUDE");
    AddToPublishQueue(curLon,     "LONGITUDE");
    AddToPublishQueue(curX,       "X");
    AddToPublishQueue(curY,       "Y");
    char q;
    if (gpgga.Get_gpsQual(q)) {
        string curQuality = "X";
        curQuality.at(0) = q;
        AddToPublishQueue(curQuality, "QUALITY"); }
    unsigned short u;
    if (gpgga.Get_satNum(u))
        AddToPublishQueue((double) u, "SAT");
    double h;
    if (gpgga.Get_hdop(h))
        AddToPublishQueue(h, "HDOP");
    totalGPGGA++;
    PublishTheQueue("GPGGA");
    return true;
}

//     $GPGST,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>*hh<CR><LF>
//     <1>  UTC time, format hhmmss.s
//     <2>  RMS value in meters of the std deviation of the ranges
//     <3>  StdDev of semimajor axis, not used
//     <4>  StdDev of semiminor axis, not used
//     <5>  Orientation of semimajor axis, not used
//     <6>  StdDev of latitude error in meters
//     <7>  StdDev of longitude error in meters
//     <8>  StdDev of altitude error in meters
bool GPSHandler::HandleGPGST(string nmea)
{
    //TODO: Debug the parsing of this message
    return true;

    GPGSTnmea gpgst;
    if (!gpgst.ParseSentenceIntoData(nmea, false)) {
        reportRunWarning("Could not parse GPGST message: " + nmea);
        return false; }
    double curErr, latStdDev, lonStdDev;
    if (gpgst.Get_latStdDev(latStdDev) && gpgst.Get_lonStdDev(lonStdDev)) {
        curErr = (latStdDev > lonStdDev ? latStdDev : lonStdDev);
        AddToPublishQueue(curErr, "HPE"); }
    totalGPGST++;
    PublishTheQueue("GPGST");
    return true;
}

//      GPHDT or HEHDT - Hemisphere GPS message for direction bow is pointing (yaw)
//
//      http://www.hemispheregps.com/gpsreference/GPHDT.htm
//      YAW (this is not HEADING)
//      This is the direction that the vessel (antennas) is pointing and is not
//      necessarily the direction of vessel motion (the course over ground).
//
//      $GPHDT,<1>,<2>*hh<CR><LF>
//          or
//      $HEHDT,<1>,<2>*hh<CR><LF>
//      <1>  Current heading in degrees
//      <2>  Always 'T' to indicate true heading
bool GPSHandler::HandleGPHDT(string nmea)
{
    GPHDTnmea gphdt;
    if (!gphdt.ParseSentenceIntoData(nmea, false)) {
        reportRunWarning("Could not parse GPHDT message: " + nmea);
        return false; }
    double yaw;
    if (gphdt.Get_yaw(yaw))
        AddToPublishQueue(yaw, "YAW");
    totalGPHDT++;
    PublishTheQueue("GPHDT");
    return true;
}

//     $GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*hh<CR><LF>
//     <1>  UTC time, format hhmmss.s
//     <2>  Status, A=Valid, V=Receiver warning
//     <3>  Lat, format ddmm.mmmmm (with leading 0s)
//     <4>  Lat hemisphere, N(+) or S(-)
//     <5>  Lon, format dddmm.mmmmm (with leading 0s)
//     <6>  Lon hemisphere, E(+) or W(-)
//     <7>  Speed over ground in KNOTS, format 000.00 (with leading 0s)
//     <8>  Course over ground in deg from true North, format ddd.d (with leading 0s)
//     <9>  UTC date, format ddmmyy
//     <10> Magnetic variation true North, format ddd.d (with leading 0s)
//     <11> Magnetic variation direction, E(-) or W(+)
//     <12> Mode indicator, A=Autonomous, D=Differential, E=Estimated, N=bad
bool GPSHandler::HandleGPRMC(string nmea)
{
    GPRMCnmea gprmc;
    if (!gprmc.ParseSentenceIntoData(nmea, false)) {
        reportRunWarning("Could not parse GPRMC message: " + nmea + "  " + gprmc.GetErrorString());
        return false; }
    if (!gprmc.Get_latlonValues(curLat, curLon)) {
        reportRunWarning("GPRMC message contains invalid lat/lon: " + nmea);
        return false; }
    GeodeticConversion();
    curUTC = 0.0;
    utcTime aUTC;
    if (gprmc.Get_timeUTC(aUTC)) {
        string strUTC;
        if (aUTC.Get_utcTimeString(strUTC))
            curUTC = strtod(strUTC.c_str(), 0); }
    GeodeticConversion();
    AddToPublishQueue(curUTC,     "UTC");
    AddToPublishQueue(curLat,     "LATITUDE");
    AddToPublishQueue(curLon,     "LONGITUDE");
    AddToPublishQueue(curX,       "X");
    AddToPublishQueue(curY,       "Y");
    if (gprmc.Get_speedMPS(curSpeed))
        AddToPublishQueue(curSpeed, "SPEED");
    else
        reportEvent("Not publishing speed");
    double d = BAD_DOUBLE;
//    if (gprmc.Get_headingTrueN(d)) {
//        if (bPublishHeading)
//            AddToPublishQueue(d, "HEADING"); }
    if (gprmc.Get_magVar(d))
        AddToPublishQueue(d, "MAGVAR");
    totalGPRMC++;
    PublishTheQueue("GPRMC");
    return true;
}

//     $GPRME,<1>,<2>,<3>*hh<CR><LF>
//     <1>  Estimated horizontal position error, 0.0 to 999.99 meters
//     <2>  Horizontal error units, always M=meters
//     <3>  Estimated vertical position error, 0.0 to 999.99 meters
//     <4>  Vertical error units, always M=meters
//     <5>  Estimated position error, 0.0 to 999.99 meters
//     <6>  Position error units, always M=meters
bool GPSHandler::HandleGPRME(string nmea)
{
    GPRMEnmea gprme;
    if (!gprme.ParseSentenceIntoData(nmea, false)) {
        reportRunWarning("Could not parse GPRME message: " + nmea);
        return false; }
    double posErr;
    if (gprme.Get_estPOSerr(posErr))
        AddToPublishQueue(posErr, "HPE");
    totalGPRME++;
    PublishTheQueue("GPRME");
    return true;
}

bool GPSHandler::HandleGPTXT(string nmea)
{
	nmea += "";
    //TODO: Debug the parsing of this message
    return true;
}

//      $PASHR,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>*hh<CR><LF>
//      <1>  [UTCtime]      UTC Timestamp of the sentence
//      <2>  [Heading]      Heading in degrees
//      <3>  [Heading_rel]  'T' disolayed if heading is relative to true north
//      <4>  [Roll]         Roll in decimal degrees
//      <5>  [Pitch]        Pitch in decimal degrees
//      <6>  [Heave]        Heave in meters
//      <7>  [Roll_stdDev]  Standard deviation of roll in decimal degrees
//      <8>  [Pitch_stdDev] Standard deviation of pitch in decimal degrees
//      <9>  [Heading_stdDev] Standard deviation of heading in decimal degrees
//      <10> [Quality]      Quality flag (0 = no position, 1 = non-RTK fixed integer pos, 2 = RTK fixed integer pos)
bool GPSHandler::HandlePASHR(string nmea)
{
    PASHRnmea pashr;
    if (!pashr.ParseSentenceIntoData(nmea, false)) {
        reportRunWarning("Could not parse PASHR message: " + nmea + "  " + pashr.GetErrorString());
        return false; }
    if (!pashr.Get_heading(curHeading)) {
        reportRunWarning("PASHR message contains invalid heading value: " + nmea);
        return false; }
    curHeading += headingOffset;
    AddToPublishQueue(curHeading, "HEADING");
    double d = 0.0;
    if (!pashr.Get_roll(d)) {
        reportRunWarning("PASHR message contains invalid roll value: " + nmea); }
    else {
        if (bSwitchPitchRoll) {
            curPitch = d;
            AddToPublishQueue(curPitch, "PITCH"); }
        else {
            curRoll  = d;
            AddToPublishQueue(curRoll, "ROLL"); } }
    if (!pashr.Get_pitch(d)) {
        reportRunWarning("PASHR message contains invalid pitch value: " + nmea); }
    else {
        if (bSwitchPitchRoll) {
                   curRoll = d;
                   AddToPublishQueue(curRoll, "ROLL"); }
       else {
           curPitch  = d;
           AddToPublishQueue(curPitch, "PITCH"); } }
    totalPASHR++;
    PublishTheQueue("PASHR");
    return true;
}

void GPSHandler::ShowCEP()
{
    if (!bShowCEP || curPosError <= 0.0 || !bValidXY)
        return;
    string label = "Pos Err";
    XYCircle circle(curX, curY, curPosError);
    circle.set_label(label);
    circle.set_color("edge", "yellow");
    circle.set_vertex_size(0);
    circle.set_edge_size(1);
    circle.set_active(true);
    string s1 = circle.get_spec();
    Notify("VIEW_CIRCLE", s1);
}

double GPSHandler::DMS2DecDeg(double dfVal)
{
        int nDeg = (int)(dfVal/100.0);

        double dfTmpDeg = (100.0*(dfVal/100.0-nDeg))/60.0;

        return  dfTmpDeg+nDeg;
}

bool GPSHandler::SetParam_TYPE(string sVal)
{
    if (!sVal.empty()) {
        string theType = toupper(sVal);
        if (MOOSStrCmp(theType, "SERIAL")) {
            bIsSerial = true;
            return true; }
        if (MOOSStrCmp(theType, "MOOS_MSG")) {
            bIsSerial = false;
            return true; } }
    reportConfigWarning("Config parameter TYPE must be SERIAL or MOOS_MSG. Unable to process: " + sVal);
    return false;
}

bool GPSHandler::SetParam_SHOW_CEP(string sVal)
{
    if (!sVal.empty()) {
        if (sVal.at(0) == 'T' || sVal.at(0) == 't') {
            bShowCEP = true;
            return true; }
        if (sVal.at(0) == 'F' || sVal.at(0) == 'f') {
            bShowCEP = false;
            return true; } }
    reportConfigWarning("Config parameter SHOW_CEP must be TRUE or FALSE. Unable to process: " + sVal);
    return false;
}

bool GPSHandler::SetParam_PREFIX(string sVal)
{
    if (sVal.empty()) {
        reportConfigWarning("Config parameter PREFIX may not be blank.");
        return false; }
    // TODO: Check sVal for legal MOOS message name characters
    m_prefix = toupper(sVal);
return true;
}

bool GPSHandler::SetParam_PORT(string sVal)
{
    if (sVal.empty()) {
        reportConfigWarning("Config parameter PORT may not be blank. In MOOS_MSG mode, do not include PORT.");
        return false; }
    serialPort = sVal;
    return true;
}

bool GPSHandler::SetParam_BAUDRATE(string sVal)
{
    if (sVal.empty()) {
        reportConfigWarning("Config parameter BAUDRATE may not be blank. In MOOS_MSG mode, do not include BAUDRATE.");
        return false; }
    sBaudRate = sVal;
    if (MOOSStrCmp(sVal,   "2400"))
        baudRate        = 2400;
        return true;
    if (MOOSStrCmp(sVal,   "4800"))
        baudRate        = 4800;
        return true;
    if (MOOSStrCmp(sVal,  "19200"))
        baudRate        = 19200;
        return true;
    if (MOOSStrCmp(sVal,  "38400"))
        baudRate        = 38400;
        return true;
    if (MOOSStrCmp(sVal,  "57600"))
        baudRate        = 57600;
        return true;
    if (MOOSStrCmp(sVal, "115200"))
        baudRate        = 115200;
        return true;
    if (MOOSStrCmp(sVal, "230400"))
        baudRate        = 230400;
        return true;
    if (MOOSStrCmp(sVal, "460800"))
        baudRate        = 2400;
        return true;
    reportConfigWarning("Config parameter BAUDRATE must be one of 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800. Unable to process: " + sVal);
    return false;
}

bool GPSHandler::SetParam_TRIGGER_MSG(string sVal)
{
    sVal = toupper(sVal);

    // Blank TRIGGER_MSG or 'NONE' means no trigger
    if (sVal.empty() || MOOSStrCmp(sVal, "NONE"))
        triggerMsg = "";
    else
        triggerMsg = sVal;
    return true;
}

bool GPSHandler::SetParam_PUBLISH_HEADING(string sVal)
{
    if (!sVal.empty()) {
        if (sVal.at(0) == 'T' || sVal.at(0) == 't') {
            bPublishHeading = true;
            return true; }
        if (sVal.at(0) == 'F' || sVal.at(0) == 'f') {
            bPublishHeading = false;
            return true; } }
    reportConfigWarning("Config parameter PUBLISH_HEADING must be TRUE or FALSE. Unable to process: " + sVal);
    return false;
}

bool GPSHandler::SetParam_REPORT_NMEA(string sVal)
{
    if (!sVal.empty()) {
        if (sVal.at(0) == 'T' || sVal.at(0) == 't') {
            bReportNMEA = true;
            return true; }
        if (sVal.at(0) == 'F' || sVal.at(0) == 'f') {
            bReportNMEA = false;
            return true; } }
    reportConfigWarning("Config parameter REPORT_NMEA must be TRUE or FALSE. Unable to process: " + sVal);
    return false;
}

bool GPSHandler::SetParam_HEADING_OFFSET(string sVal)
{
    if (sVal.empty()) {
        reportConfigWarning("Config parameter HEADING_OFFSET has no value. Defaulting to 0.0.");
        return true; }
    headingOffset = strtod(sVal.c_str(), 0);
    if (headingOffset >= 180.0 || headingOffset <= -180.0) {
        reportConfigWarning("Config parameter HEADING_OFFSET must be between -180.0 and 180.0. Unhandled: " + sVal);
        return false; }
    return true;
}

bool GPSHandler::SetParam_SWITCH_PITCH_ROLL(string sVal)
{
    if (!sVal.empty()) {
        if (sVal.at(0) == 'T' || sVal.at(0) == 't') {
            bSwitchPitchRoll = true;
            return true; }
        if (sVal.at(0) == 'F' || sVal.at(0) == 'f') {
            bSwitchPitchRoll = false;
            return true; } }
    reportConfigWarning("Config parameter SWITCH_PITCH_ROLL must be TRUE or FALSE. Unable to process: " + sVal);
    return false;
}

bool GPSHandler::buildReport()
{
    m_msgs <<     "Publishing with prefix: " << m_prefix << endl;
    if (bReverseMode)
    	m_msgs << "    --- REVERSE THRUST MODE IS ON --- " << endl;
    if (bIsSerial) {
        m_msgs << "Mode:                    SERIAL" << endl;
        m_msgs << "Serial port:            " << serialPort <<  "      Baud rate " << sBaudRate << endl;
        m_msgs << "Port status:            ";
        if (serial->IsGoodSerialComms())
            m_msgs << "Open and working" << endl;
        else
            m_msgs << "Closed and not working" << endl; }
    else {
        m_msgs << "Mode:                    NMEA_MSG" << endl;
        m_msgs << "Messages received:      " << nmeaMsgCounter << endl; }
    m_msgs <<     "Processed NMEA:                   BAD Messages " << totalBAD << endl;
    if (totalGPGGA)
        m_msgs << "                                  GPGGA " << totalGPGGA << endl;
    if (totalGPGST)
        m_msgs << "                                  GPGST " << totalGPGST << endl;
    if (totalGPHDT)
        m_msgs << "                                  GPHDT " << totalGPHDT << endl;
    if (totalGPRMC)
        m_msgs << "                                  GPRMC " << totalGPRMC << endl;
    if (totalGPRME)
        m_msgs << "                                  GPRME " << totalGPRME << endl;
    if (totalPASHR)
        m_msgs << "                                  PASHR " << totalPASHR << endl;
    m_msgs <<     "Most recent data published: " << endl;
    m_msgs <<     "                                  X, Y    " << curX << ", " << curY << endl;
    m_msgs <<     "                              Lat, Lon    " << curLat << ", " << curLon << endl;
    m_msgs <<     "                           Speed (mps)    " << curSpeed << endl;
    if (bPublishHeading) {
        m_msgs <<     "                               Heading    " << curHeading << endl;
        m_msgs <<     "                           Pitch, Roll    " << curPitch << ", " << curRoll << endl; }
    return true;
}











// ATTIC


/*

bool GPSHandler::InitialiseSensor()
{
    if (MOOSStrCmp(m_sType, "ASHTECH")) {
        const char * sInit = "$PASHS,NME,GGA,A,ON\r\n";
        MOOSTrace("Sending %s\n", sInit);
        m_Port.Write(sInit, strlen(sInit));
        MOOSPause(2000);
        string sReply;
        double dfTime;
        if (m_Port.GetLatest(sReply, dfTime))
            MOOSTrace("Rx %s", sReply.c_str());
        else
            MOOSTrace("No reply\n"); }

    else if (MOOSStrCmp(m_sType, "GARMIN")) {
        const char *sInitA = "$PGRMO,,2\r\n";
        MOOSTrace("Sending %s\n", sInitA);
        MOOSPause(2000);
        m_Port.Write(sInitA, strlen(sInitA));
        const char *sInitB = "$PGRMO,GPGGA,1\r\n";
        MOOSTrace("Sending %s\n", sInitB);
        MOOSPause(2000);
        m_Port.Write(sInitB, strlen(sInitB));
        const char *sInitC = "$PGRMO,GPRMC,1\r\n";
        MOOSTrace("Sending %s\n", sInitC);
        MOOSPause(2000);
        m_Port.Write(sInitC, strlen(sInitC));
        if (bShowCEP) {
            const char *sInitD = "$PGRMO,PGRME,1\r\n";
            MOOSTrace("Sending %s\n", sInitD);
            MOOSPause(2000);
            m_Port.Write(sInitD, strlen(sInitD)); }
        else {
            const char *sInitD = "$PGRMO,PGRME,0\r\n";
            MOOSTrace("Sending %s\n", sInitD);
            MOOSPause(2000);
            m_Port.Write(sInitD, strlen(sInitD)); }
        const char *sInit;
        string sReply;
        double dfTime;
        if (m_Port.GetLatest(sReply, dfTime))
            MOOSTrace("Rx %s\n", sReply.c_str());
        else
            MOOSTrace("No reply\n");
        sInit = "$PGRMC,A,10.0,100,,,,,,A,5,1,2,1,0.2\r\n";
        MOOSTrace("Sending %s\n", sInit);
        m_Port.Write(sInit, strlen(sInit));
        sInit = "$PGRMC1,,1,,,,,1,W,N,,,,1,,1\r\n";
        MOOSTrace("Sending %s\n", sInit);
        m_Port.Write(sInit, strlen(sInit));
        if (m_Port.GetLatest(sReply, dfTime))
            MOOSTrace("Rx %s\n", sReply.c_str());
        else
            MOOSTrace("No reply\n"); }

        else if (MOOSStrCmp(m_sType, "UBLOX"))
            MOOSTrace("Using u-Blox GPS, no initialization string sent to device.\n");

    initIsComplete = 1;
    return true;
}




void GPSHandler::AddWarning(string wStr)
{
    if (!curWarning.empty())
        curWarning.append (" ");
    curWarning.append(wStr);
}



 */







//



