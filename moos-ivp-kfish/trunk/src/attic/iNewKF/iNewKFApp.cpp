/************************************************************/
/*    NAME: Alon Yaari
/*    FILE: iNewKFApp.cpp
/*    DATE: November 22, 2012
/************************************************************/

#include <iostream>
#include <cmath>
#include "iNewKFApp.h"
#include "MBUtils.h"

using namespace std;


double clamp(double v, double minv, double maxv)
{
    return min(maxv,max(minv, v));
}


NewKF::NewKF()
{
    // Initialize state Variables
    m_thrustCommanded = 0;
    m_thrustTime      = 0;
    m_rudderCommanded = 0;
    m_rudderTime      = 0;
    m_compassOffset   = 0;
    
    // Initialize configuration variables
    m_verbose         = false;
    m_appName         = "iNewKF";
    m_port            = 9090;

    // MOOS Message names
    msgCmdThrust      = string("DESIRED_THRUST");
    msgCmdRudder      = string("DESIRED_RUDDER");
    msgCompass        = string("COMPASS_HEADING");
    msgVoltage        = string("BATTERY_VOLTS");

    m_MaxThrustValue    = 100.0;    // Maximum thrust to be commanded to the motors
    m_MinThrustValue    = 100.0;    // Minimum thrust to be commanded to the motors
    
    // Direct thrust control
    m_DirectControlMode = false;    // When true, command the motors directly
    m_ThrustL           = 0.0;      // Direct thrust value to command the left motor
    m_ThrustR           = 0.0;      // Direct thrust value to commamnd the right motor
    m_DirectThrustTime  = 0.0;      // Last time direct thrust was commanded
    
    m_timeout = 6;  // seconds
    lastVNotifyTime     = 0.0;
    curTime             = MOOSTime();
    m_PublishVTime      = 15.0;

    m_Offset_LT10       = 0;
    m_Offset_GTE10_LT20 = 0;
    m_Offset_GTE20_LT30 = 0;
    m_Offset_GTE30_LT40 = 0;
    m_Offset_GTE40_LT50 = 0;
    m_Offset_GTE50_LT60 = 0;
    m_Offset_GTE60_LT70 = 0;
    m_Offset_GTE70_LT80 = 0;
    m_Offset_GTE80_LT90 = 0;
    m_Offset_GTE90      = 0;

    testMode = false;
}

NewKF::~NewKF()
{
	clearpath_destroy();
}


bool NewKF::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;
    for (p=NewMail.begin(); p!=NewMail.end(); ++p) {
        CMOOSMsg & rMsg = *p;

        if (MOOSStrCmp(rMsg.GetKey(), msgCmdThrust )) {
            m_thrustCommanded = rMsg.GetDouble();
            m_thrustTime = rMsg.GetTime();
        	if (m_verbose)
        		MOOSTrace("DESIRED_THRUST: %.1f\n"); }
        
        else if (MOOSStrCmp(rMsg.GetKey(), msgCmdRudder )) {
            m_rudderCommanded = rMsg.GetDouble();
            m_rudderTime = rMsg.GetTime();
        	if (m_verbose)
        		MOOSTrace("DESIRED_RUDDER: %.1f\n");}
        
        else if (MOOSStrCmp(rMsg.GetKey(), "GPS_MAGNETIC_DECLINATION")) {
            double magdec = rMsg.GetDouble();
            if (m_compassOffset != magdec) {
                MOOSTrace("Overriding any prior compass offset, setting to magnetic declination reported by compass: %lf\n", magdec);
                m_compassOffset = magdec; } }

        else if (MOOSStrCmp(rMsg.GetKey(), "DIRECT_THRUST_CONTROL")) {
            string sVal = p->GetString();
            MOOSToUpper(sVal);
            if (MOOSStrCmp (sVal, "TRUE")) {
                m_DirectControlMode = true;
                MOOSTrace ("\n------------------\n");
                MOOSTrace ("%s: DIRECT THRUST CONTROL MODE ON (ignoring DESIRED_THRUST/RUDDER\n", m_appName.c_str());
                MOOSTrace ("------------------\n\n"); }
            else {
                m_DirectControlMode = false;
                MOOSTrace ("\n------------------\n");
                MOOSTrace ("%s: NORMAL DESIRED_THRUST/RUDDER CONTROL (direct thrust control mode off)\n", m_appName.c_str());
                MOOSTrace ("------------------\n\n"); } }
        
        else if (MOOSStrCmp(rMsg.GetKey(), "DIRECT_THRUST_L")) {
            m_ThrustL = p->GetDouble();
            m_DirectThrustTime = MOOSTime(); }
        
        else if (MOOSStrCmp(rMsg.GetKey(), "DIRECT_THRUST_R")) {
            m_ThrustR = p->GetDouble();
            m_DirectThrustTime = MOOSTime(); } }
    return true;
}


bool NewKF::OnConnectToServer()
{
    m_MissionReader.GetConfigurationParam("PORT", m_port);
    m_MissionReader.GetConfigurationParam("TIMEOUT", m_timeout);
    m_MissionReader.GetConfigurationParam("VERBOSE", m_verbose);
    m_MissionReader.GetConfigurationParam("COMPASS_OFFSET", m_compassOffset);

    // Read parameters for progressive offsets
    bool usingOffsets = false;
    usingOffsets |= m_MissionReader.GetConfigurationParam("OFFSET_LT10", m_Offset_LT10);
    usingOffsets |= m_MissionReader.GetConfigurationParam("OFFSET_GTE10_LT20", m_Offset_GTE10_LT20);
    usingOffsets |= m_MissionReader.GetConfigurationParam("OFFSET_GTE20_LT30", m_Offset_GTE20_LT30);
    usingOffsets |= m_MissionReader.GetConfigurationParam("OFFSET_GTE30_LT40", m_Offset_GTE30_LT40);
    usingOffsets |= m_MissionReader.GetConfigurationParam("OFFSET_GTE40_LT50", m_Offset_GTE40_LT50);
    usingOffsets |= m_MissionReader.GetConfigurationParam("OFFSET_GTE50_LT60", m_Offset_GTE50_LT60);
    usingOffsets |= m_MissionReader.GetConfigurationParam("OFFSET_GTE60_LT70", m_Offset_GTE60_LT70);
    usingOffsets |= m_MissionReader.GetConfigurationParam("OFFSET_GTE70_LT80", m_Offset_GTE70_LT80);
    usingOffsets |= m_MissionReader.GetConfigurationParam("OFFSET_GTE80_LT90", m_Offset_GTE80_LT90);
    usingOffsets |= m_MissionReader.GetConfigurationParam("OFFSET_GTE90", m_Offset_GTE90);

    if(usingOffsets) {
		MOOSTrace ("%s: m_Offset_LT10       = %.1f\n", m_appName.c_str(), m_Offset_LT10);
		MOOSTrace ("%s: m_Offset_GTE10_LT20 = %.1f\n", m_appName.c_str(), m_Offset_GTE10_LT20);
		MOOSTrace ("%s: m_Offset_GTE20_LT30 = %.1f\n", m_appName.c_str(), m_Offset_GTE20_LT30);
		MOOSTrace ("%s: m_Offset_GTE30_LT40 = %.1f\n", m_appName.c_str(), m_Offset_GTE30_LT40);
		MOOSTrace ("%s: m_Offset_GTE40_LT50 = %.1f\n", m_appName.c_str(), m_Offset_GTE40_LT50);
		MOOSTrace ("%s: m_Offset_GTE50_LT60 = %.1f\n", m_appName.c_str(), m_Offset_GTE50_LT60);
		MOOSTrace ("%s: m_Offset_GTE60_LT70 = %.1f\n", m_appName.c_str(), m_Offset_GTE60_LT70);
		MOOSTrace ("%s: m_Offset_GTE70_LT80 = %.1f\n", m_appName.c_str(), m_Offset_GTE70_LT80);
		MOOSTrace ("%s: m_Offset_GTE80_LT90 = %.1f\n", m_appName.c_str(), m_Offset_GTE80_LT90);
		MOOSTrace ("%s: m_Offset_GTE90      = %.1f\n", m_appName.c_str(), m_Offset_GTE90); }

    double dVal;
    if (m_MissionReader.GetConfigurationParam("MAX_THRUST_VALUE", dVal)) {
        if (dVal <= 0.0 || dVal > 100.0) {
            MOOSTrace ("%s ERROR: MAX_THRUST_VALUE must be in range (0.0, 100.0].  Mission file defines it as %f.\n", m_appName.c_str(), dVal);
            return false; }
        m_MaxThrustValue = dVal; }
    MOOSTrace ("%s: MIN_THRUST_VALUE = %f\n", m_appName.c_str(), m_MaxThrustValue);
    if (m_MissionReader.GetConfigurationParam("MIN_THRUST_VALUE", dVal)) {
        if (dVal < -1000.0 || dVal > 0.0) {
            MOOSTrace ("%s ERROR: MIN_THRUST_VALUE must be in range [-100.0, 0.0].  Mission file defines it as %f.\n", m_appName.c_str(), dVal);
            return false; }
        m_MinThrustValue = dVal; }
    MOOSTrace ("%s: MIN_THRUST_VALUE = %f\n", m_appName.c_str(), m_MinThrustValue);

    if (m_MinThrustValue >= m_MaxThrustValue) {
    	MOOSTrace ("%s: MIN_THRUST_VALUE (%.1f) must be less than MAX_THRUST_VALUE (%.1f).  Quitting.\n",
    			m_appName.c_str(), m_MinThrustValue, m_MaxThrustValue);
    	return false; }

    if (m_MissionReader.GetConfigurationParam("VOLTAGE_PUB_TIME", dVal)) {
    	if (dVal <= 0.0) {
    		MOOSTrace("%s: VOLTAGE_PUB_TIME must be greater than 0.0. Quitting.\n", m_appName.c_str());
    		return false; }
        m_PublishVTime = dVal; }
    MOOSTrace("%s: Publishing voltage each %.1f seconds.\n", m_appName.c_str(), m_PublishVTime);

    string sVal;
    if (m_MissionReader.GetConfigurationParam("THRUST_MSG_NAME", sVal)) {
    	if (sVal.empty()) {
    		MOOSTrace("%s: Mission file has blank name for THRUST_MSG_NAME. Quitting.\n", m_appName.c_str());
    		return false; }
    	MOOSTrace("%s: Thrust command message name set to %s.\n", m_appName.c_str(), sVal.c_str());
    	msgCmdThrust = sVal; }

    if (m_MissionReader.GetConfigurationParam("RUDDER_MSG_NAME", sVal)) {
    	if (sVal.empty()) {
    		MOOSTrace("%s: Mission file has blank name for RUDDER_MSG_NAME. Quitting.\n", m_appName.c_str());
    		return false; }
    	MOOSTrace("%s: Rudder command message name set to %s.\n", m_appName.c_str(), sVal.c_str());
    	msgCmdRudder = sVal; }

    if (m_MissionReader.GetConfigurationParam("COMPASS_HEADING_NAME", sVal)) {
    	if (sVal.empty()) {
    		MOOSTrace("%s: Mission file has blank name for COMPASS_HEADING_NAME. Quitting.\n", m_appName.c_str());
    		return false; }
    	MOOSTrace("%s: Compass heading output message name set to %s.\n", m_appName.c_str(), sVal.c_str());
    	msgCompass = sVal; }

    if (m_MissionReader.GetConfigurationParam("VOLTAGE_OUTPUT_NAME", sVal)) {
    	if (sVal.empty()) {
    		MOOSTrace("%s: Mission file has blank name for VOLTAGE_OUTPUT_NAME. Quitting.\n", m_appName.c_str());
    		return false; }
    	MOOSTrace("%s: Battery voltage output message name set to %s.\n", m_appName.c_str(), sVal.c_str());
    	msgVoltage = sVal; }

    if (m_MissionReader.GetConfigurationParam("TEST_MODE", sVal)) {
    	if (MOOSStrCmp(sVal, "TRUE")) {
    		MOOSTrace("%s:\n--------------- TEST MODE ------------------\n", m_appName.c_str());
    		testMode = true; } }

    // @todo: Read thrust and rudder command names from mission file
    
    DoRegistrations();
    m_appName = GetAppName();

    // Initialize clearpath comms
    MOOSTrace("%s: Initializing comms with vehicle controller... ", m_appName.c_str());
    int cpSuccess = clearpath_init("localhost", m_port);
    MOOSPause(1000);
    if (cpSuccess != 0) {
    	MOOSTrace("\n%s: Clearpath comms initialization failed.  Quitting.\n", m_appName.c_str());
    	return false; }
    MOOSTrace("Done.\n");

    return true;
}


bool NewKF::GetAndPublishLatestClearpathData()
{
	curCompass = clearpath_get_compass() + m_compassOffset;
	m_Comms.Notify("COMPASS_HEADING", curCompass);
	MOOSTrace("\n\nCOMPASS: %f\n\n", curCompass);

	curVoltage = clearpath_get_voltage();
	if (curTime - lastVNotifyTime > m_PublishVTime) {
		lastVNotifyTime = curTime; }
	m_Comms.Notify("BATTERY_VOLTAGE", clearpath_get_voltage());

	return true;
}


bool NewKF::Iterate()
{

    GetAndPublishLatestClearpathData();
	double t = MOOSTime();

    // TWO KINDS OF CONTROL:
    //      1. NORMAL CONTROL using DESIRED_THRUST and DESIRED_RUDDER
    //      2. DIRECT THRUST CONTROL using DIRECT_THRUST_L and DIRECT_THRUST_R
    // Switch between using boolean DIRECT_THRUST_CONTROL

    // 1. DIRECT THRUST CONTROL
    if (m_DirectControlMode) {
        
        // Validate thrust in range [MIN_THRUST, MAX_THRUST]
        double left = clamp(m_ThrustL, m_MinThrustValue, m_MaxThrustValue);
        double right = clamp(m_ThrustR, m_MinThrustValue, m_MaxThrustValue);
        clearpath_set_output(left, right);
        if (m_verbose)
            MOOSTrace ("%s: DIRECT CONTROL Sent to controller   left: %3.1f \tright: %3.1f\n", m_appName.c_str(), left, right); }

    // 2. NORMAL CONTROL
    else {
        double thrustRequestAge = t - m_thrustTime;
        double rudderRequestAge = t - m_rudderTime;

        // Requested thrust and rudder are within timeout
        //      - Convert to left/right command values and push to vehicle
        bool doit = ((thrustRequestAge < m_timeout) && (rudderRequestAge < m_timeout));
        if (!doit && testMode) {
        	m_thrustCommanded = 30.0;
        	m_rudderCommanded = 30.0;
        	doit = true; }
        if (doit) {
            double left = 0.0;
            double right = 0.0;
            CalculateThrust (left, right);
            clearpath_set_output (left, right);
            m_Comms.Notify ("KF_COMMANDED_L", left);
            m_Comms.Notify ("KF_COMMANDED_R", right);
            if (m_verbose)
                MOOSTrace ("%s: Sent to controller   left: %3.1f \tright: %3.1f\n", m_appName.c_str(), left, right);
        }

        // Too long since last requested thrust or rudder
        //      - Stop all
        else {
        	clearpath_set_output (0, 0);
            char timeoutMsg[100];
            snprintf (timeoutMsg, 100, "MaxTimeout=%.1f,ThrustRequestAge=%.1f,RudderRequestAge=%.1f", m_timeout, thrustRequestAge, rudderRequestAge);
            m_Comms.Notify ("KF_TIMEOUT", timeoutMsg); } }



    clearpath_spin();
    return true;
}


bool NewKF::OnStartUp()
{
    DoRegistrations();
    return true;
}


void NewKF::DoRegistrations()
{
    m_Comms.Register("DESIRED_RUDDER", 0);
    m_Comms.Register("DESIRED_THRUST", 0);
    m_Comms.Register("DIRECT_THRUST_L", 0);
    m_Comms.Register("DIRECT_THRUST_R", 0);
    m_Comms.Register("DIRECT_THRUST_CONTROL", 0);
}


void NewKF::CalculateThrust (double &left, double &right)
{
    // 1. Constrain Values
    //      DESIRED_RUDDER value to MAX_RUDDER
    //          - Anything more extreme than +/-50.0 is turn-in-place
    //      DESIRED_THRUST value to MAX_THRUST
    //          - Anything greater than +/-100.0% makes no sense
    double desiredRudder = clamp (m_rudderCommanded, (-1.0 * MAX_RUDDER), MAX_RUDDER);
    if (m_verbose && abs((int) m_rudderCommanded) > MAX_RUDDER) {
        MOOSTrace ("iActuationKF WARNING: DESIRED_RUDDER of %.1f constrained to %.1f\n", m_rudderCommanded, desiredRudder);
    }
    double desiredThrust = clamp (m_thrustCommanded, (-1.0 * MAX_THRUST), MAX_THRUST);
    if (m_verbose && abs((int) m_thrustCommanded) > MAX_THRUST) {
        MOOSTrace ("iActuationKF WARNING: DESIRED_THRUST of %.1f constrained to %.1f\n", m_thrustCommanded, desiredThrust);
    }

    // 2. Calculate turn
    //      - ADD rudder to left thrust
    //      - SUBTRACT rudder from right thrust
    double percentLeft = desiredThrust + desiredRudder;
    double percentRight = desiredThrust - desiredRudder;

    // 3. Map desired thrust values to motor bounds
    //      - Range of DESIRED_THRUST: [-MAX_THRUST, MAX_THRUST]
    //      -          ...map to...
    //      - Range of valid thrust values: [-m_MaxThrustValue, m_MaxThrustValue]
    double fwdOrRevL = (percentLeft > 0.0) ? 1.0 : -1.0;
    double pctThrustL = fabs (percentLeft) / MAX_THRUST;
    double mappedLeft = pctThrustL * m_MaxThrustValue * fwdOrRevL;
    double fwdOrRevR = (percentRight > 0.0) ? 1.0 : -1.0;
    double pctThrustR = fabs (percentRight) / MAX_THRUST;
    double mappedRight = pctThrustR * m_MaxThrustValue * fwdOrRevR;

    // 4. Offset using the progressive offsets
    //      - Based on the original DESIRED_THRUST value
    //      - Add offsets from left side motor
    char cOffset = 'x';
    if (m_thrustCommanded < 10)         { mappedLeft += m_Offset_LT10;          cOffset = '0'; }
    else if (m_thrustCommanded < 20.0)  { mappedLeft += m_Offset_GTE10_LT20;    cOffset = '1'; }
    else if (m_thrustCommanded < 30.0)  { mappedLeft += m_Offset_GTE20_LT30;    cOffset = '2'; }
    else if (m_thrustCommanded < 40.0)  { mappedLeft += m_Offset_GTE30_LT40;    cOffset = '3'; }
    else if (m_thrustCommanded < 50.0)  { mappedLeft += m_Offset_GTE40_LT50;    cOffset = '4'; }
    else if (m_thrustCommanded < 60.0)  { mappedLeft += m_Offset_GTE50_LT60;    cOffset = '5'; }
    else if (m_thrustCommanded < 70.0)  { mappedLeft += m_Offset_GTE60_LT70;    cOffset = '6'; }
    else if (m_thrustCommanded < 80.0)  { mappedLeft += m_Offset_GTE70_LT80;    cOffset = '7'; }
    else if (m_thrustCommanded < 90.0)  { mappedLeft += m_Offset_GTE80_LT90;    cOffset = '8'; }
    else                                { mappedLeft += m_Offset_GTE90;         cOffset = '9'; }

    // 5. Deal with overages
    //      - Any value over m_MaxThrustValue gets subtracted from both sides equally
    //      - Constrain to [-m_MaxThrustValue, m_MaxThrustValue]
    double maxThrustNeg = -1.0 * m_MaxThrustValue;
    if (mappedLeft > m_MaxThrustValue)  mappedRight -= (mappedLeft - m_MaxThrustValue);
    if (mappedLeft < maxThrustNeg)      mappedRight -= (mappedLeft + m_MaxThrustValue);
    if (mappedRight > m_MaxThrustValue) mappedLeft  -= (mappedRight - m_MaxThrustValue);
    if (mappedRight < maxThrustNeg)     mappedLeft  -= (mappedRight + m_MaxThrustValue);
    
    left = clamp (mappedLeft, (-1.0 * m_MaxThrustValue), m_MaxThrustValue);
    right = clamp (mappedRight, (-1.0 * m_MaxThrustValue), m_MaxThrustValue);

    if (m_verbose) {
        MOOSTrace ("DES_ T%.1f R%.1f", m_thrustCommanded, m_rudderCommanded);
        MOOSTrace ("\t\tConst: T%.1f R%.1f", desiredThrust, desiredRudder);
        MOOSTrace ("\t\tPct L%.1f R%.1f", percentLeft, percentRight);
        MOOSTrace ("\t\tMap: L%.1f R%.1f", mappedLeft, mappedRight);
        MOOSTrace ("\t\tFinal: L%.1f R%.1f", left, right);
        MOOSTrace ("\tOffset: %c\n", cOffset);
    }
}

