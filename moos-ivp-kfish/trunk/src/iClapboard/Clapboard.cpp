/*********************************************************************/
/* Clapboard.cpp                                                     */
/*                                                                   */
/*********************************************************************/

#include <cstring>		// Required so that Linux knows about the strlen command
#include "MBUtils.h"
#include "Clapboard.h"

using namespace std;


Clapboard::Clapboard()
{
    outMsgName = "TRIGGER";
    bVerbose    = true;
}


bool Clapboard::Iterate()
{
    CheckSerial();
    return true;
}


bool Clapboard::OnStartUp()
{
    string sVal;

    if (m_MissionReader.GetConfigurationParam("VERBOSE", sVal)) {
        if (!sVal.empty()) {
            sVal = toupper(sVal);
            bVerbose = sVal.at(0) == 'T'; } }
    if (bVerbose)
        MOOSTrace("%s: Verbose mode is on.\n", m_sAppName.c_str());

    if (m_MissionReader.GetConfigurationParam("OUT_MSG_NAME", sVal)) {
        if (sVal.empty()) {
            MOOSTrace("%s: OUT_MSG_NAME incorrectly defined in mission file.\n", m_sAppName.c_str());
            return false; }
        sVal        = toupper(sVal);
        outMsgName = sVal; }
    if (bVerbose)
        MOOSTrace("%s: Triggers will be published in message %s.\n", m_sAppName.c_str(), outMsgName.c_str());

    // Set up serial port
    if (!SetupPort())
        return false;
    InitialiseSensor();
    m_Port.Flush();

    if (bVerbose)
    	MOOSTrace("%s: Serial setup complete.\n", m_sAppName.c_str());
    return true;
}


bool Clapboard::InitialiseSensor()
{
	MOOSPause(2000);
	const char *sInitA = "\n\n\n";
	MOOSTrace("Initializing comms with the device\n", sInitA);
	m_Port.Write(sInitA, strlen(sInitA));
	return true;
}


bool Clapboard::OnNewMail(MOOSMSG_LIST &NewMail)
{
    return UpdateMOOSVariables(NewMail);
}


bool Clapboard::OnConnectToServer()
{
    return true;
}


bool Clapboard::CheckSerial()
{
    string sWhat = "";
    double dfWhen;

    // Get recent data from the serial port thread
    if (m_Port.IsStreaming()) {
        if (!m_Port.GetLatest(sWhat, dfWhen)) {
            return false; } }
    else {
        if (!m_Port.GetTelegram(sWhat, 0.5)) {
        	return false; } }
    if (sWhat.empty())
        return false;

    // Something was received over the serial port
    //      - If it has 'X' in it, this was a trigger
    if (sWhat.find('X') != string::npos) {
        string outMsg =  "TRIGGER: MOOSTime ";
        outMsg        += doubleToString(MOOSTime(), 10);
        outMsg        += ", UTCTime ";
        outMsg        += doubleToString(MOOSLocalTime());
        m_Comms.Notify(outMsgName, outMsg);
        if (bVerbose)
            MOOSTrace("%s: Trigger message: %s\n", m_sAppName.c_str(), outMsg.c_str()); }
	MOOSTrace ("sWhat: %s\n", sWhat.c_str());
    return true;
}
