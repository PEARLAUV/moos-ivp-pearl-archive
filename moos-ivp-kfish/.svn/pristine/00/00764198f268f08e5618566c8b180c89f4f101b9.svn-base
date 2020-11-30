/*
 * os5000.cpp
 *
 *  Major Revision on: July 22, 2014
 *      Author: Alon Yaari
 */
//#include <cstring>
#include "MBUtils.h"
//#include "iostream"
#include "vector"
#include "os5000.h"
#include "stdlib.h"      //strtod

using namespace std;


os5000::os5000()
{
    bValid               = false;
    serialPort           = "";
    baudRate             = 19200;
    m_prefix             = "COMPASS_";
    serial               = NULL;
    msgCounter           = 0;
    magVar               = 0.0;
    curHeading           = BAD_DOUBLE;
    curPitch             = BAD_DOUBLE;
    curRoll              = BAD_DOUBLE;
    curTempC             = BAD_DOUBLE;
}

bool os5000::Iterate()
{
    AppCastingMOOSApp::Iterate();
    GetData();
    AppCastingMOOSApp::PostReport();
    return true;
}

bool os5000::OnStartUp()
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

      if (param == "PREFIX")
          goodParams = SetParam_PREFIX(value);
      else if (param == "PORT")
          goodParams = SetParam_PORT(value);
      else if (param == "BAUDRATE" || param == "SPEED")
          goodParams = SetParam_BAUDRATE(value);
      else if (param == "MAG_VAR"   || param == "PREROTATION")
          goodParams = SetParam_MAGVAR(value);
      else
          reportUnhandledConfigWarning(orig); }

    if (!goodParams) {
        reportConfigWarning("Invalid mission file parameters. No attempt to parse GPS data.");
        bValid = false;}
    else {
        RegisterForMOOSMessages();
        PrefixCleanup();
        bValid = SerialSetup(); }
    return true;
}

bool os5000::SerialSetup()
{
    string errMsg = "";
    serial = new SerialComms(serialPort, baudRate, errMsg);
    if (serial->IsGoodSerialComms()) {
        serial->Run();
        reportEvent("Serial port opened. Listening for incoming serial data on port " + serialPort);
        return true; }
    reportConfigWarning("Unable to open serial port: " + errMsg);
    return false;
}

bool os5000::RegisterForMOOSMessages()
{
    AppCastingMOOSApp::RegisterVariables();
    //m_Comms.Register("NMEA_MSG", 0);
    return RegisterMOOSVariables();
}

bool os5000::OnNewMail(MOOSMSG_LIST &NewMail)
{
    AppCastingMOOSApp::OnNewMail(NewMail);
    MOOSMSG_LIST::iterator p;
    for (p=NewMail.begin(); p!=NewMail.end(); ++p) {
        CMOOSMsg & rMsg = *p;

        // Sample code in case this app ever needs to read moos messages
        if (MOOSStrCmp(rMsg.GetKey(), "NMEA_MSG")) {
            string sVal = p->GetString();
            if (!sVal.empty())
                ParseNMEAString(sVal); } }

    return UpdateMOOSVariables(NewMail);
}

bool os5000::OnConnectToServer()
{
    RegisterForMOOSMessages();
    return true;
}

// PrefixCleanup()
//        Ensures only one '_' after the prefix
//        Allows mission file to have "GPS_" or "GPS" defined
void os5000::PrefixCleanup()
{
    string pre = m_prefix;
    if (!pre.empty()) {
        if (pre.at(pre.length() - 1) != '_')
            pre += '_'; }
    nameHeading = pre + "HEADING";
    namePitch   = pre + "PITCH";
    nameRoll    = pre + "ROLL";
    nameTempC   = pre + "TEMPERATURE";
}

void os5000::GetData()
{
    while (serial->DataAvailable()) {
        string osData = serial->GetNextSentence();
        ParseNMEAString(osData); }
}

bool os5000::ParseNMEAString(string osData)
{
    if (osData.empty()) {
        reportRunWarning("Received empty data string from the OS5000.");
        return false; }
    // Incoming line must start with '$' and with '*cc' where cc is the checksum
    //      - Minimum sentence theoretically is $*00
    //                                          0123
    size_t len = osData.length();
    if (len < 4) {
        reportRunWarning("Poorly formed line from os5000 unit: " + osData);
        return false; }
    if (!(osData.at(0) == '$')) {
        reportRunWarning("Line from os5000 unit does not properly start with '$C': " + osData);
        return false; }
    if (!(osData.at(len - 3) == '*')) {
        reportRunWarning("Line from os5000 unit does not properly terminate with '*': " + osData);
        return false; }
    string checksum = osData.substr(len - 2, 2);
    string calcdsum = ChecksumCalc(osData);
    if (!MOOSStrCmp(checksum, calcdsum)) {
        reportRunWarning("Line from os5000 unit has improper checksum: " + osData);
        return false; }

    string toParse = MOOSChomp(osData, "*");
    len = toParse.length();
    string hValStr = "";
    string pValStr = "";
    string rValStr = "";
    string tValStr = "";
    char   what = '\0';
    for (unsigned int i = 0; i < len; i++) {
        char c = toParse.at(i);
        switch (c) {
            case 'C':
            case 'P':
            case 'R':
            case 'T':
                what = c;
                break;
            default:
                if (c > 'A' && c < 'Z')
                    what = '\0';
                switch (what) {
                    case 'C': hValStr += c; break;
                    case 'P': pValStr += c; break;
                    case 'R': rValStr += c; break;
                    case 'T': tValStr += c; break; } } }
    if (!hValStr.empty()) {
        double dVal = strtod(hValStr.c_str(), 0) + magVar;
        if (dVal > 360.0)
            dVal -= 360.0;
        if (dVal < 0.0)
            dVal += 360.0;
        curHeading = dVal;
        m_Comms.Notify(nameHeading, dVal);
        msgCounter++; }
    if (!pValStr.empty()) {
        double dVal = strtod(pValStr.c_str(), 0);
        curPitch = dVal;
        m_Comms.Notify(namePitch, dVal); }
    if (!rValStr.empty()) {
        double dVal = strtod(rValStr.c_str(), 0);
        curRoll = dVal;
        m_Comms.Notify(namePitch, dVal); }
    if (!tValStr.empty()) {
        double dVal = strtod(tValStr.c_str(), 0);
        curTempC = dVal;
        m_Comms.Notify(namePitch, dVal); }
    return true;
}

bool os5000::SetParam_PREFIX(string sVal)
{
    if (sVal.empty()) {
        reportConfigWarning("Config parameter PREFIX may not be blank.");
        return false; }
    // TODO: Check sVal for legal MOOS message name characters
    m_prefix = toupper(sVal);
    reportEvent("Prefix for MOOS messages set to " + m_prefix);
return true;
}

bool os5000::SetParam_PORT(string sVal)
{
    if (sVal.empty()) {
        reportConfigWarning("Config parameter PORT may not be blank. In MOOS_MSG mode, do not include PORT.");
        return false; }
    serialPort = sVal;
    reportEvent("Serial port set to " + serialPort);
    return true;
}

bool os5000::SetParam_BAUDRATE(string sVal)
{
    if (sVal.empty()) {
        reportConfigWarning("Config parameter BAUDRATE may not be blank. In MOOS_MSG mode, do not include BAUDRATE.");
        return false; }
    sBaudRate = sVal;
    if      (MOOSStrCmp(sVal,   "2400"))    baudRate          = 2400;
    else if (MOOSStrCmp(sVal,   "4800"))    baudRate          = 4800;
    else if (MOOSStrCmp(sVal,  "19200"))    baudRate         = 19200;
    else if (MOOSStrCmp(sVal,  "38400"))    baudRate         = 38400;
    else if (MOOSStrCmp(sVal,  "57600"))    baudRate         = 57600;
    else if (MOOSStrCmp(sVal, "115200"))    baudRate        = 115200;
    else if (MOOSStrCmp(sVal, "230400"))    baudRate        = 230400;
    else if (MOOSStrCmp(sVal, "460800"))    baudRate        = 460800;
    else {
        reportConfigWarning("Config parameter BAUDRATE must be one of 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800. Unable to process: " + sVal);
        return false; }
    reportEvent("Baud rate set to " + sBaudRate);
    return true;
}

bool os5000::SetParam_MAGVAR(string sVal)
{
    if (sVal.empty()) {
        reportConfigWarning("Config parameter MAGVAR may not be blank.");
        return false; }
    magVar = strtod(sVal.c_str(), 0);
    if (magVar < -180.0 || magVar > 180.0) {
        reportConfigWarning("Config parameter MAGVAR must be between -180.0 and 180.0");
        return false; }
    reportEvent("Magnetic variation (to offset magnetic to true north) is set to " + sVal);
    return true;
}

bool os5000::buildReport()
{
    m_msgs << "Serial port status: ";
        if (serial->IsGoodSerialComms())
            m_msgs << "Open and working" << endl;
        else
            m_msgs << "Closed and not working" << endl;
    m_msgs << "Messages from os5000 unit received to date: " << msgCounter << endl;
    m_msgs << "Publishing messages with prefix: " << m_prefix << endl;
    m_msgs << "Most recently published data:" << endl;
    string sVal;
    sVal = curHeading == BAD_DOUBLE ? "---" : doubleToString(curHeading, 1);
    m_msgs << "      " << nameHeading << "      " << sVal << endl;
    sVal = curPitch == BAD_DOUBLE ? "---" : doubleToString(curPitch, 1);
    m_msgs << "      " << namePitch << "        " << sVal << endl;
    sVal = curRoll == BAD_DOUBLE ? "---" : doubleToString(curRoll, 1);
    m_msgs << "      " << nameRoll << "         " << sVal << endl;
    sVal = curTempC == BAD_DOUBLE ? "---" : doubleToString(curTempC, 1);
    m_msgs << "      " << nameTempC <<       "  " << sVal << endl;
    return true;
}


// ChecksumCalc()
//      sentence        Checksum will be created from this string
string os5000::ChecksumCalc(string sentence)
{
    string ck = "00";
    if (sentence.empty())
        return ck;

    unsigned char xCheckSum = 0;
    MOOSChomp(sentence,"$");
    string sToCheck = MOOSChomp(sentence,"*");
    string sRxCheckSum = sentence;
    string::iterator p;
    for (p = sToCheck.begin(); p != sToCheck.end(); p++)
        xCheckSum ^= *p;
    ostringstream os;
    os.flags(ios::hex);
    os << (int) xCheckSum;
    ck = toupper(os.str());
    if (ck.length() < 2)
        ck = "0" + ck;
    return ck;
}











//



