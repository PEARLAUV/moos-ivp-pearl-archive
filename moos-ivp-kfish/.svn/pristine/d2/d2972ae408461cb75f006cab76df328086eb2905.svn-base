/*********************************************************************/
/* GPSInstrument.cpp                                                 */ 
/*                                                                   */
/*********************************************************************/

#include <cstring>
#include "MBUtils.h"
#include "XYCircle.h"
#include "GPSInstrument.h"


using namespace std;


CGPSInstrument::CGPSInstrument()
{
    m_sType              = "VANILLA";
    bShowCEP             = false;
    bShowSummary         = false;
    bRawGPS              = false;
    initIsComplete       = 0;
    m_prefix             = "GPS_";
    lastXYUpdateTime     = 0.0;
    curGPSTime           = 0.0;
    lastGPSTime          = 0.0;
    bPublishHeading      = false;
    curWarning           = "";
    bHaveMagVarToPublish = false;
    bValidXY             = false;
    curNMEA              = "";
    bNMEAfromMsg         = false;
}


bool CGPSInstrument::Iterate()
{
    if (!bNMEAfromMsg) {
        if (initIsComplete == 1)
            GetData(); }
    return true;

}


bool CGPSInstrument::OnStartUp()
{
    CMOOSInstrument::OnStartUp();

    double dfLatOrigin, dfLongOrigin;
    string sVal;

    if (m_MissionReader.GetConfigurationParam("PREFIX", sVal))        // Blank prefix is valid
        m_prefix = sVal;
    m_prefix = toupper(m_prefix);
    MOOSTrace("%s: Output variable prefix: %s\n", GetAppName().c_str(), m_prefix.c_str());

    if (m_MissionReader.GetConfigurationParam("SHOW_CEP", sVal)) {
        if (sVal.size() > 0) {
            if (sVal.at(0) == 'T' || sVal.at(0) == 't')
                bShowCEP = true; } }
    MOOSTrace("%s: CEP view circle will%s be published.\n", GetAppName().c_str(), (bShowCEP ? " not" : ""));

    if (m_MissionReader.GetConfigurationParam("SHOW_SUMMARY", sVal)) {
        if (sVal.size() > 0) {
            if (sVal.at(0) == 'T' || sVal.at(0) == 't')
                bShowSummary = true; } }
    MOOSTrace("%s: Summary message will%s be published.\n", GetAppName().c_str(), (bShowSummary ? " not" : ""));

    if (m_MissionReader.GetConfigurationParam("PUBLISH_HEADING", sVal)) {
        if (sVal.size() > 0) {
            if (sVal.at(0) == 'T' || sVal.at(0) == 't')
                bPublishHeading = true; } }
    MOOSTrace("%s: GPS-computed heading will%s be published.\n", GetAppName().c_str(), (bPublishHeading ? " not" : ""));

    if (m_MissionReader.GetConfigurationParam("RAW_GPS", sVal)) {
        if (sVal.size() > 0) {
            if (sVal.at(0) == 'T' || sVal.at(0) == 't')
                bRawGPS = true; } }
    MOOSTrace("%s: Raw GPS output will%s be published.\n", GetAppName().c_str(), (bRawGPS ? " not" : ""));
  
    if (m_MissionReader.GetValue("LatOrigin", sVal))
        dfLatOrigin = atof(sVal.c_str());
    else {
        MOOSTrace("%s: LatOrigin not set - FAIL\n", GetAppName().c_str());
        return false; }

    if (m_MissionReader.GetValue("LongOrigin", sVal))
        dfLongOrigin = atof(sVal.c_str());
    else {
        MOOSTrace("%s: LongOrigin not set - FAIL\n", GetAppName().c_str());
        return false; }
  
    if (!m_Geodesy.Initialise(dfLatOrigin, dfLongOrigin)) {
        MOOSTrace("%s: Geodesy Init failed - FAIL\n", GetAppName().c_str());
        return false; }
    MOOSTrace("%s: Geodesy using LatOrigin = %f and LongOrigin = %f\n",
            GetAppName().c_str(), m_Geodesy.GetOriginLatitude(), m_Geodesy.GetOriginLongitude());

    if (m_MissionReader.GetConfigurationParam("NMEA_FROM_MSG", sVal)) {
        if (sVal.size() > 0) {
            if (sVal.at(0) == 'T' || sVal.at(0) == 't')
                bNMEAfromMsg = true; } }
    if (!bNMEAfromMsg) {
        m_MissionReader.GetConfigurationParam("TYPE", m_sType);
        MOOSTrace("%s: Connecting with GPS of type %s.\n", GetAppName().c_str(), m_sType.c_str()); }
    else {
        MOOSTrace("%s: Reading NMEA from message NMEA_MSG instead of from serial GPS.\n", GetAppName().c_str());
        m_Comms.Register("NMEA_MSG", 0); }

    RegisterMOOSVariables();
    InitNotifyNames();

    if (!bNMEAfromMsg) {
        if (!SetupPort())
            return false;
        if (!InitialiseSensorN(10, "GPS")) // Try 10 times to initialize sensor
            return false; }
    return true;
}



bool CGPSInstrument::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;
    for (p=NewMail.begin(); p!=NewMail.end(); ++p) {
        CMOOSMsg & rMsg = *p;

        if (MOOSStrCmp(rMsg.GetKey(), "NMEA_MSG")) {
            string sVal = p->GetString();
            if (!sVal.empty())
                curNMEA = sVal;
                ParseNMEAString(); } }

    return UpdateMOOSVariables(NewMail);
}


bool CGPSInstrument::OnConnectToServer()
{
    RegisterMOOSVariables();
    return true;
}


bool CGPSInstrument::InitNotifyNames()
{
    string pre = m_prefix;
    if (!pre.empty()) {
        if (pre.at(pre.length() - 1) != '_')
            pre += '_'; }
    nameLat     = pre + "LAT";
    nameLon     = pre + "LON";
    nameLong    = pre + "LONG";
    nameX       = pre + "X";
    nameY       = pre + "Y";
    nameN       = pre + "N";
    nameE       = pre + "E";
    nameSpeed   = pre + "SPEED";
    nameHeading = pre + "HEADING";
    nameSatNum  = pre + "SAT";
    nameHDOP    = pre + "HDOP";
    nameQuality = pre + "QUALITY";
    nameMagVar  = pre + "MAGVAR";
    nameHPE     = pre + "HPE";
    nameSummary = pre + "SUMMARY";
    nameRaw     = pre + "RAW";
    nameWarning = pre + "WARNING";
    return true;
}


bool CGPSInstrument::InitialiseSensor()
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


void CGPSInstrument::GetData()
{
    double dfWhen;
    
    if (m_Port.IsStreaming()) {
        while (m_Port.GetEarliest(curNMEA, dfWhen))
            ParseNMEAString(); }
    else {
        while (m_Port.GetTelegram(curNMEA, 0.5))
            ParseNMEAString(); }
}

bool CGPSInstrument::HandleLatLon(string nmea, string lat, string latH, string lon, string lonH)
{
    bool bGood  = true;
    double dLat = atof(lat.c_str());
    double dLon = atof(lon.c_str());
    curLat      = DMS2DecDeg(dLat);
    curLon      = DMS2DecDeg(dLon);
    if (curLat < 0.0 || curLat > 90.0) {
        AddWarning("Latitude value out of range. " + nmea);
        bGood = false; }
    if (curLon < 0.0 || curLon > 180.0) {
        AddWarning("Longitude value out of range. " + nmea);
        bGood = false; }
    if (latH.empty())
        latH = 'X';
    switch (latH.at(0)) {
        case 'N':   break;
        case 'S':   curLon *= -1; break;
        default:
            AddWarning("Unrecognized latitude hemisphere. " + nmea);
            bGood = false; break; }
    if (lonH.empty())
        lonH = 'X';
    switch (lonH.at(0)) {
        case 'E':   break;
        case 'W':   curLon *= -1; break;
        default:
            AddWarning("Unrecognized longitude hemisphere. " + nmea);
            bGood = false; break; }
    if (bGood) {
            bValidXY = GeodeticConversion(); }
    else
        bValidXY = false;
    return bGood;
}

bool CGPSInstrument::GeodeticConversion()
{
    double dfELocal, dfNLocal;
    double dfXLocal, dfYLocal;
    if (m_Geodesy.LatLong2LocalUTM(curLat, curLon, dfNLocal, dfELocal)) {
        curN = dfNLocal;
        curE = dfELocal;
        if (m_Geodesy.LatLong2LocalUTM(curLat, curLon, dfYLocal, dfXLocal)) {
            curX = dfXLocal;
            curY = dfYLocal;
            lastXYUpdateTime = MOOSTime();
            return true; } }
    return false;
}

bool CGPSInstrument::HandleGPSQuality(string qStr)
{
    if (qStr.empty())
        return false;
    switch (qStr.at(0)) {
        case '1':
        case 'A':   curQuality = "NO_D";  break;
        case '2':
        case 'D':   curQuality = "DIFF";  break;
        case '6':
        case 'E':   curQuality = "EST";   break;
        default:    curQuality = "BAD";   break; }
    return true;
}

bool CGPSInstrument::HandleMagVar(string mvStr, string mvStrH)
{
    if (!mvStr.empty() && !mvStrH.empty()) {
        double mv = atof(mvStr.c_str());
        if (mvStrH.empty())
            mvStrH = 'X';
        switch (mvStrH.at(0)) {
            case 'W':
                bHaveMagVarToPublish = true;
                curMagVar = mv; break;
            case 'E':
                bHaveMagVarToPublish = true;
                curMagVar = mv * -1; break;
            default:    break; }
        return true; }
    return false;
}

bool CGPSInstrument::ParseNMEAString()
{
    if (curNMEA.empty())
        return false;

    string strNMEA = curNMEA;
    curNMEA = "";

    if (bRawGPS)
        m_Comms.Notify(nameRaw, strNMEA.c_str());

    // Verify checksum
    if (!DoNMEACheckSum(strNMEA)) {
        AddWarning("GPS Failed NMEA check sum. " + strNMEA);
        return false; }

    // Break the sentence apart
    vector<string> parts = parseString(strNMEA, ',');
    unsigned int pSize = parts.size();

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
    if (parts[0] == "$GPGGA") {
        if (pSize < 15) {
            AddWarning("Malformed NMEA sentence. " + strNMEA);
            return false; }
        if (!HandleLatLon(strNMEA, parts[2], parts[3], parts[4], parts[5]))
            return false;
        if (!HandleGPSQuality(parts[6]))        // MUST HAVE quality report on GPGGA
            return false;
        curSatNum  = atoi(parts[7].c_str());
        curHDOP    = atof(parts[8].c_str());
        curGPSTime = atof(parts[1].c_str());
        PublishData();
        return true; }

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
    else if (parts[0] == "$GPRMC") {
        if (pSize < 12) {
            AddWarning("Malformed NMEA sentence. " + strNMEA);
            return false; }
        if (!HandleLatLon(strNMEA, parts[3], parts[4], parts[5], parts[6]))
            return false;
        curSpeed   = atof(parts[7].c_str()) * KNOTS2METERSperSEC;
        curHeading = atof(parts[8].c_str());
        HandleMagVar(parts[10], parts[11]);
        if (pSize > 12)
            HandleGPSQuality(parts[12]);        // It's ok if no quality report on GPRMC
        curGPSTime = atof(parts[1].c_str());
        PublishData();
        return true; }

//     $GPGST,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>*hh<CR><LF>
//     <1>  UTC time, format hhmmss.s
//     <2>  RMS value in meters of the std deviation of the ranges
//     <3>  StdDev of semimajor axis, not used
//     <4>  StdDev of semiminor axis, not used
//     <5>  Orientation of semimajor axis, not used
//     <6>  StdDev of latitude error in meters
//     <7>  StdDev of longitude error in meters
//     <8>  StdDev of altitude error in meters
    else if (parts[0] == "$GPGST") {
        if (pSize < 8) {
            AddWarning("Malformed NMEA sentence. " + strNMEA);
            return false; }
        double latError = atof(parts[6].c_str());
        double lonError = atof(parts[7].c_str());
        if (latError > lonError)
            curPosError = latError;
        else
            curPosError = lonError;
        return true; }


//     $GPRME,<1>,<2>,<3>*hh<CR><LF>
//     <1>  Estimated horizontal position error, 0.0 to 999.99 meters
//     <2>  Horizontal error units, always M=meters
//     <3>  Estimated vertical position error, 0.0 to 999.99 meters
//     <4>  Vertical error units, always M=meters
//     <5>  Estimated position error, 0.0 to 999.99 meters
//     <6>  Position error units, always M=meters
    else if (parts[0] == "$GPRME") {
        if (pSize < 6) {
            AddWarning("Malformed NMEA sentence. " + strNMEA);
            return false; }
        double hError = atof(parts[1].c_str());
        double vError = atof(parts[3].c_str());
        double pError = atof(parts[5].c_str());
        if (hError > vError)
            vError = hError;
        if (vError > pError)
            curPosError = vError;
        else
            curPosError = pError;
        return true; }

    AddWarning("Unparsed NMEA sentence. " + strNMEA);
    return false;
}

bool CGPSInstrument::PublishData()
{
    if (!curWarning.empty()) {
        m_Comms.Notify(nameWarning, curWarning);
        curWarning = ""; }

    bool bNeedToPublish = (lastGPSTime != curGPSTime);
    if (bValidXY && bNeedToPublish) {
        lastGPSTime = curGPSTime;

        m_Comms.Notify(nameLat,     curLat);
        m_Comms.Notify(nameLon,     curLon);
        m_Comms.Notify(nameLong,    curLon);
        m_Comms.Notify(nameX,       curX);
        m_Comms.Notify(nameY,       curY);
        m_Comms.Notify(nameN,       curN);
        m_Comms.Notify(nameE,       curE);
        m_Comms.Notify(nameSpeed,   curSpeed);
        m_Comms.Notify(nameSatNum,  (double) curSatNum);
        m_Comms.Notify(nameHDOP,    curHDOP);
        m_Comms.Notify(nameQuality, curQuality);
        m_Comms.Notify(nameHPE,     curPosError);

        if (bPublishHeading)
            m_Comms.Notify(nameHeading, curHeading);

        if (bHaveMagVarToPublish) {
            m_Comms.Notify(nameMagVar, curMagVar);
            bHaveMagVarToPublish = false; }

        if (bShowSummary) {
            char tmp[160];
            snprintf(tmp, 160, "time=%lf,n=%lf,e=%lf,x=%lf,y=%lf,lat=%lf,lon=%lf,sat=%i,hdop=%lf, hpe=%lf",
                    lastXYUpdateTime, curN, curE, curX, curY, curLat, curLon, (int)curSatNum, curHDOP, curPosError);
            m_Comms.Notify(nameSummary, tmp); }

        ShowCEP(); }

    return true;
}


void CGPSInstrument::AddWarning(string wStr)
{
    if (!curWarning.empty())
        curWarning.append (" ");
    curWarning.append(wStr);
}


void CGPSInstrument::ShowCEP()
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

double CGPSInstrument::DMS2DecDeg(double dfVal)
{
        int nDeg = (int)(dfVal/100.0);

        double dfTmpDeg = (100.0*(dfVal/100.0-nDeg))/60.0;

        return  dfTmpDeg+nDeg;
}




//
