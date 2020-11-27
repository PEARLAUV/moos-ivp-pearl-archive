/************************************************************/
/*    NAME: Michael "Misha" Novitzky                        */
/*    BASED ON: iActuationKFApp.cpp by Alon Yaari           */
/*    BASED ON: iKFController written by                    */
/*              Hordur Johannsson and Mike Benjamin         */
/*    FILE: iActuationKFACApp.cpp                           */
/*    DATE: May 22, 2013                                    */
/************************************************************/

//This version has been modified to include AppCasting
//
// This version of the KF controller is modified from iKFController as follows:
//
//  1. When turning, if thrust is increased beyond 100 on one motor, the overage
//      is added to the other side
//  2. ALL output variables are published with a KF_ prefix.  Prior they were
//      CP_ or COMPASS_
//  3. When timeouts occur, a message is posted showing what timed out
//  4. Thrust being commanded to left and right are published as messages
//  5. Added control over how DESIRED_RUDDER is mapped to commanded thrust


#include <iostream>
#include <cmath>
#include <stdio.h>
#include "iActuationKFACApp.h"
#include "MBUtils.h"
#include "ACTable.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.14159
#endif

#ifdef WIN32
#define snprintf _snprintf
#endif


// Procedure: clamp
//   Purpose: Clamps the value of v between minv and maxv
double clamp(double v, double minv, double maxv)
{
    return min(maxv,max(minv, v));
}


ActuateKFAC::ActuateKFAC() 
{
    // Initialize state Variables
    m_program_status = "GOOD";
    m_critical_voltage = 11.2;
    m_critical_voltage_triggered = false;
    m_critical_current = 5.0;
    m_critical_current_timeout = 10.0;
    m_critical_current_timer = 0.0;
    m_critical_current_timer_started = false;

    m_port             = "/dev/KINGFISHER";
    m_thrustCommanded  = 0;
    m_thrustTime       = 0;
    m_rudderCommanded  = 0;
    m_rudderTime       = 0;
    m_magdec           = 0;
    m_magdec_set       = false;
    
    m_orientation_received = false;
    m_roll             = 0;
    m_pitch            = 0;
    m_yaw              = 0;
    m_mag_received     = false;
    
    // Initialize configuration variables
    m_heading_offset   = 0;
    m_compute_heading  = false;
    m_verbose          = false;
    m_MaxThrustValue   = 100;    // Maximum thrust to be commanded to the motors
    m_no_comms_with_kf = false;
    
    // Direct thrust control
    m_DirectControlMode = false;    // When true, command the motors directly
    m_ThrustL           = 0.0;      // Direct thrust value to command the left motor
    m_ThrustR           = 0.0;      // Direct thrust value to commamnd the right motor
    m_DirectThrustTime  = 0.0;      // Last time direct thrust was commanded

    for (int i = 0; i < 3; ++i) m_mag_offsets[i] = 0.0;
    for (int i = 0; i < 3; ++i) m_mag_xyz[i]     = 0.0;
    
    m_timeout = 6;                  // seconds
    
    m_process_orientation_data  = false;
    m_process_rotation_data     = false;
    m_process_acceleration_data = false;
    m_process_magnetometer_data = false;
    m_process_systemstatus_data = true;

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
}


bool ActuateKFAC::OnNewMail(MOOSMSG_LIST &NewMail)
{
    AppCastingMOOSApp::OnNewMail(NewMail);

    //If program is in MALCONFIG state do nothing
    if (m_program_status == "MALCONFIG")
        return true;

    MOOSMSG_LIST::iterator p;
    for (p = NewMail.begin(); p != NewMail.end(); ++p) {
        CMOOSMsg & rMsg = *p;

        if (MOOSStrCmp(rMsg.GetKey(), THRUST_COMMAND )) {
            m_thrustCommanded = rMsg.GetDouble();
            m_thrustTime      = rMsg.GetTime(); }

        else if (MOOSStrCmp(rMsg.GetKey(), RUDDER_COMMAND )) {
            m_rudderCommanded = rMsg.GetDouble();
            m_rudderTime      = rMsg.GetTime(); }

        else if (MOOSStrCmp(rMsg.GetKey(), "GPS_MAGNETIC_DECLINATION")) {
            double magdec     = rMsg.GetDouble();
            if (m_magdec != magdec) {
                MOOSTrace("%s: Setting magnetic declination to %lf\n", m_sAppName.c_str(), magdec);
                m_magdec      = magdec;
                m_magdec_set  = true; } }

        else if (MOOSStrCmp(rMsg.GetKey(), "DIRECT_THRUST_CONTROL")) {
            string sVal       = p->GetString();
            if (sVal.size() > 0) {
                if (sVal.at(0) == 'T' || sVal.at(0) == 't')
                    m_DirectControlMode = true;
                else
                    m_DirectControlMode = false; }
            string msg = "Direct thrust control mode is ";
            msg += (m_DirectControlMode ? "ON (ignoring DESIRED_THRUST/RUDDER)" : "OFF (acting on DESIRED_THRUST/RUDDER)");
            reportEvent(msg);
            if (m_verbose)
                MOOSTrace ("\n%s: ------------------\n%s\n------------------\n", m_sAppName.c_str(), msg.c_str()); }

        else if (MOOSStrCmp(rMsg.GetKey(), "DIRECT_THRUST_L")) {
            m_ThrustL          = p->GetDouble();
            m_DirectThrustTime = MOOSTime(); }

        else if (MOOSStrCmp(rMsg.GetKey(), "DIRECT_THRUST_R")) {
            m_ThrustR          = p->GetDouble();
            m_DirectThrustTime = MOOSTime(); } }
    return true;
}


bool ActuateKFAC::OnConnectToServer()
{
    char appName[100];
    unsigned int len = GetAppName().length();
    GetAppName().copy(appName, 100);
    appName[len] = '\0';
    double dVal = 0;
    string sVal;

    // TODO: read from config file instead of declaring these implicitly
    THRUST_COMMAND = std::string("DESIRED_THRUST");
    RUDDER_COMMAND = std::string("DESIRED_RUDDER");

    if (m_MissionReader.GetConfigurationParam("VERBOSE", sVal)) {
            if (sVal.size() > 0) {
                if (sVal.at(0) == 'T' || sVal.at(0) == 't')
                    m_verbose = true; } }
    MOOSTrace("%s: VERBOSE mode is set %s.\n", appName, (m_verbose ? "ON" : "OFF"));

    if (m_MissionReader.GetConfigurationParam("MAX_THRUST_VALUE", dVal)) {
        if (dVal <= 0.0 || dVal > 100.0) {
            string msgErr = "ERROR: MAX_THRUST_VALUE must be in range (0, 100].";
            msgErr       += "Mission file defines it as " + doubleToString(dVal, 1) + ".";
            MOOSTrace("%s: %s.\n", appName, msgErr.c_str());
            reportConfigWarning(msgErr);
            m_program_status = "MALCONFIG"; }
        m_MaxThrustValue = dVal; }
    if (m_verbose)
        MOOSTrace ("iActuationKFAC: MAX_THRUST_VALUE = %f\n", m_MaxThrustValue);

    if (m_MissionReader.GetConfigurationParam("NO_COMMS_TO_KFISHER", sVal)) {
        if (sVal.size() > 0) {
            if (sVal.at(0) == 'T' || sVal.at(0) == 't')
                m_no_comms_with_kf = true; } }
    MOOSTrace("%s: NO_COMMS_TO_KFISHER mode is set %s.\n", appName, (m_no_comms_with_kf ? "ON" : "OFF"));

    // This app can be set to not to talk with the kingfisher
    //      - No attempt to communicate with the vehicle
    //      - App will go through its motions, processing input messages and publishing output
    //        but will not try to talk through the serial port
    if (m_no_comms_with_kf)
        return true;

    if (!m_MissionReader.GetConfigurationParam("Port", m_port)) {
        string msgErr = "ERROR: PORT not defined in mission file.";
        MOOSTrace("%s: %s.\n", appName, msgErr.c_str());
        reportConfigWarning(msgErr);
        return false; }

    if (!m_MissionReader.GetConfigurationParam("CRITICAL_VOLTAGE",m_critical_voltage)) {
        string errMsg = "ERROR: CRITICAL_VOLTAGE not found in Mission File.";
        MOOSTrace("%s: %s\n", appName, errMsg.c_str());
        reportConfigWarning(errMsg);
        m_program_status = "MALCONFIG"; }

    if (!m_MissionReader.GetConfigurationParam("CRITICAL_CURRENT", m_critical_current)) {
        string errMsg = "ERROR: CRITICAL_CURRENT not found in Mission File.";
        MOOSTrace("%s: %s\n", appName, errMsg.c_str());
        reportConfigWarning(errMsg);
        m_program_status = "MALCONFIG"; }

    if (!m_MissionReader.GetConfigurationParam("CRITICAL_CURRENT_TIMEOUT",m_critical_current_timeout)) {
        string errMsg = "ERROR: CRITICAL_CURRENT_TIMEOUT not found in Mission File.";
        MOOSTrace("%s: %s\n", appName, errMsg.c_str());
        reportConfigWarning(errMsg);
        m_program_status = "MALCONFIG"; }

    if (m_MissionReader.GetConfigurationParam("COMPUTEHEADING",  sVal) ||
       (m_MissionReader.GetConfigurationParam("COMPUTE_HEADING", sVal))) {
        if (sVal.size() > 0) {
            if (sVal.at(0) == 'T' || sVal.at(0) == 't')
                m_compute_heading = true; } }
    MOOSTrace("%s: COMPUTE HEADING mode is set %s.\n", appName, (m_compute_heading ? "ON" : "OFF"));

    m_MissionReader.GetConfigurationParam("Timeout",    m_timeout);
    m_MissionReader.GetConfigurationParam("MagOffsetX", m_mag_offsets[0]);
    m_MissionReader.GetConfigurationParam("MagOffsetY", m_mag_offsets[1]);
    m_MissionReader.GetConfigurationParam("MagOffsetZ", m_mag_offsets[2]);
    
    m_MissionReader.GetConfigurationParam("HeadingOffset",           m_heading_offset);
    m_MissionReader.GetConfigurationParam("ComputeHeading",          m_compute_heading);
    m_MissionReader.GetConfigurationParam("ProcessOrientationData",  m_process_orientation_data);
    m_MissionReader.GetConfigurationParam("ProcessRotationData",     m_process_rotation_data);
    m_MissionReader.GetConfigurationParam("ProcessAccelerationData", m_process_acceleration_data);
    m_MissionReader.GetConfigurationParam("ProcessMagnetometerData", m_process_magnetometer_data);
    m_MissionReader.GetConfigurationParam("ProcessSystemStatusData", m_process_systemstatus_data);

    // Read parameters for progressive offsets
    m_MissionReader.GetConfigurationParam("OFFSET_LT10",   m_Offset_LT10);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE10_LT20", m_Offset_GTE10_LT20);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE20_LT30", m_Offset_GTE20_LT30);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE30_LT40", m_Offset_GTE30_LT40);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE40_LT50", m_Offset_GTE40_LT50);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE50_LT60", m_Offset_GTE50_LT60);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE60_LT70", m_Offset_GTE60_LT70);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE70_LT80", m_Offset_GTE70_LT80);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE80_LT90", m_Offset_GTE80_LT90);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE90",      m_Offset_GTE90);

    MOOSTrace ("%s: m_Offset_LT10       = %.1f\n", appName, m_Offset_LT10);
    MOOSTrace ("%s: m_Offset_GTE10_LT20 = %.1f\n", appName, m_Offset_GTE10_LT20);
    MOOSTrace ("%s: m_Offset_GTE20_LT30 = %.1f\n", appName, m_Offset_GTE20_LT30);
    MOOSTrace ("%s: m_Offset_GTE30_LT40 = %.1f\n", appName, m_Offset_GTE30_LT40);
    MOOSTrace ("%s: m_Offset_GTE40_LT50 = %.1f\n", appName, m_Offset_GTE40_LT50);
    MOOSTrace ("%s: m_Offset_GTE50_LT60 = %.1f\n", appName, m_Offset_GTE50_LT60);
    MOOSTrace ("%s: m_Offset_GTE60_LT70 = %.1f\n", appName, m_Offset_GTE60_LT70);
    MOOSTrace ("%s: m_Offset_GTE70_LT80 = %.1f\n", appName, m_Offset_GTE70_LT80);
    MOOSTrace ("%s: m_Offset_GTE80_LT90 = %.1f\n", appName, m_Offset_GTE80_LT90);
    MOOSTrace ("%s: m_Offset_GTE90      = %.1f\n", appName, m_Offset_GTE90);

    // Verify that comms with Kingfisher is possible
    int max_retry = 3;
    try {
        clearpath::Transport::instance().configure(m_port.c_str(), max_retry); }
    catch(...) {
        string errMsg = "ERROR: clearpath::Transport failed to open port [" + m_port + "]. ";
        errMsg       += "Something is wrong talking to the serial port.";
        MOOSTrace("%s: %s\n", appName, errMsg.c_str());
        reportConfigWarning(errMsg);
        m_program_status = "MALCONFIG"; }

    if (m_program_status == "MALCONFIG") {
        // TODO: What do we do here?
    }
    
    // NOT in MALCONFIG state, so it's ok to finish setup
    // TODO: See if we should have validation on timeout
    else {
        ShowPlatformInfo();

        if (m_process_orientation_data) {
            if (m_verbose)
                MOOSTrace("%s: Will publish orientation data from the Kingfisher.", appName);
        clearpath::DataPlatformOrientation::subscribe(20); }

        if (m_process_magnetometer_data) {
            if (m_verbose)
                MOOSTrace("%s: Will publish magnetometer data from the Kingfisher.", appName);
        clearpath::DataPlatformMagnetometer::subscribe(20); }

        #if 0
        if (m_process_rotation_data) {
            if (m_verbose)
                MOOSTrace("%s: Will publish rotation data from the Kingfisher.", appName);
        clearpath::DataPlatformRotation::subscribe(20); }
        #endif

        #if 0
        if (m_process_acceleration_data) {
            if (m_verbose)
                MOOSTrace("%s: Will publish acceleration data from the Kingfisher.", appName);
        clearpath::DataPlatformAcceleration::subscribe(20); }
        #endif

        if(m_process_systemstatus_data) {
            if (m_verbose)
                MOOSTrace("%s: Will publish system status data from the Kingfisher.", appName);
        clearpath::DataSystemStatus::subscribe(5); } }
    
    DoRegistrations();
    return true;
}


template<class Data>
Data* ActuateKFAC::GetLatestData()
{
    Data* data = Data::popNext();
    if (data) {
        Data* next = Data::popNext();
        while (next) {
            delete(data);
            data = next;
            next = Data::popNext(); } }
    return data;
}


void ActuateKFAC::ProcessOrientationData()
{
    // Only run this function if talking with the Kingfisher
    if (m_no_comms_with_kf)
        return;


    clearpath::DataPlatformOrientation* orientation = 0;
    orientation = GetLatestData<clearpath::DataPlatformOrientation>();
    if (!orientation) {
        m_orientation_received = false;
        return; }

    m_orientation_received = true;
    if (m_compute_heading) {
        double hdg = orientation->getYaw() / M_PI * 180.0 + m_magdec;
        m_Comms.Notify("KF_HEADING", hdg);
        m_AppCastValues["KF_HEADING"] = hdg;
        m_AppCastTimestamp["KF_HEADING"] = MOOSTime(); }

    m_roll  = orientation->getRoll();
    m_pitch = orientation->getPitch();
    m_yaw   = orientation->getYaw();

    double notifyPitch = m_pitch / M_PI * 180.0;
    double notifyRoll  = m_roll  / M_PI * 180.0;
    m_Comms.Notify("KF_PITCH", notifyPitch);
    m_Comms.Notify("KF_ROLL",  notifyRoll);
    m_Comms.Notify("KF_YAW",   m_yaw);
    m_AppCastValues["KF_PITCH"]    = notifyPitch;
    m_AppCastValues["KF_ROLL"]     = notifyRoll;
    m_AppCastValues["KF_YAW"]      = m_yaw;
    m_AppCastTimestamp["KF_PITCH"] = MOOSTime();
    m_AppCastTimestamp["KF_ROLL"]  = MOOSTime();
    m_AppCastTimestamp["KF_YAW"]   = MOOSTime();

    string msg = "pitch="  + doubleToString(notifyPitch, 1);
    msg +=       ", roll=" + doubleToString(notifyRoll,  1);
    msg +=       ", yaw="  + doubleToString(m_yaw);
    m_Comms.Notify("KF_ORIENTATION", msg.c_str());
    if (m_verbose)
        MOOSTrace("%s: %s\n",m_sAppName.c_str(), msg.c_str());
    delete(orientation);
}

void ActuateKFAC::ProcessMagnetometerData()
{
    // Only run this function if talking with the Kingfisher
    if (m_no_comms_with_kf)
        return;

    clearpath::DataPlatformMagnetometer* mag = 0;
    mag = GetLatestData<clearpath::DataPlatformMagnetometer>();
    if (!mag) {
        m_mag_received = false;
        return; }

    m_mag_received = true;
    double x = mag->getX();
    double y = mag->getY();
    double z = mag->getZ();
    m_mag_xyz[0] = x;
    m_mag_xyz[1] = y;
    m_mag_xyz[2] = z;
    x += m_mag_offsets[0];
    y += m_mag_offsets[1];
    z += m_mag_offsets[2];
    double yaw       = -atan2(y, x) + m_heading_offset / 180.0*M_PI;  // TODO:  Rotate by roll/pitch
    double notifyYaw = m_magdec + (yaw / M_PI * 180.0);               // TODO:  Rotate by roll/pitch

    m_Comms.Notify("KF_MAG_X",            x);
    m_Comms.Notify("KF_MAG_Y",            y);
    m_Comms.Notify("KF_MAG_Z",            z);
    m_Comms.Notify("KF_YAW_COMPUTED",     yaw);
    m_Comms.Notify("KF_HEADING_COMPUTED", notifyYaw);
    m_AppCastValues["KF_MAG_X"]               = x;
    m_AppCastValues["KF_MAG_Y"]               = y;
    m_AppCastValues["KF_MAG_Z"]               = z;
    m_AppCastValues["KF_YAW_COMPUTED"]        = yaw;
    m_AppCastValues["KF_HEADING_COMPUTED"]    = notifyYaw;
    m_AppCastTimestamp["KF_MAG_X"]            = MOOSTime();
    m_AppCastTimestamp["KF_MAG_Y"]            = MOOSTime();
    m_AppCastTimestamp["KF_MAG_Z"]            = MOOSTime();
    m_AppCastTimestamp["KF_YAW_COMPUTED"]     = MOOSTime();
    m_AppCastTimestamp["KF_HEADING_COMPUTED"] = MOOSTime();

    if (m_compute_heading) {
        m_Comms.Notify("KF_HEADING", notifyYaw);
        m_AppCastValues["KF_HEADING"] =  notifyYaw;
        m_AppCastTimestamp["KF_HEADING"] = MOOSTime(); }

    string msg = "MAG_X="   + doubleToString(m_mag_xyz[0], 1);
    msg +=       ", MAG_Y=" + doubleToString(m_mag_xyz[1], 1);
    msg +=       ", MAG_Z=" + doubleToString(m_mag_xyz[2], 1);
    m_Comms.Notify("KF_MAG", msg);
    if (m_verbose)
        MOOSTrace("%s: %s\n",m_sAppName.c_str(), msg.c_str());
    delete(mag);
}


void ActuateKFAC::ProcessRotationData()
{
    // Only run this function if talking with the Kingfisher
    if (m_no_comms_with_kf)
        return;


    clearpath::DataPlatformRotation* rot;
    rot = GetLatestData<clearpath::DataPlatformRotation>();
    if (!rot) {
        return; }

    double roll_rate = rot->getRollRate();
    double pitch_rate = rot->getPitchRate();
    double yaw_rate = rot->getYawRate();

    m_Comms.Notify("KF_ROT_ROLL",  roll_rate);
    m_Comms.Notify("KF_ROT_PITCH", pitch_rate);
    m_Comms.Notify("KF_ROT_YAW",   yaw_rate);
    m_AppCastValues["KF_ROT_ROLL"]     = roll_rate;
    m_AppCastValues["KF_ROT_PITCH"]    = pitch_rate;
    m_AppCastValues["KF_ROT_YAW"]      = yaw_rate;
    m_AppCastTimestamp["KF_ROT_ROLL"]  = MOOSTime();
    m_AppCastTimestamp["KF_ROT_PITCH"] = MOOSTime();
    m_AppCastTimestamp["KF_ROT_YAW"]   = MOOSTime();

    string msg = "roll_rate="    + doubleToString(roll_rate,  1);
    msg +=       ", pitch_rate=" + doubleToString(pitch_rate, 1);
    msg +=       ", yaw_rate="   + doubleToString(yaw_rate,  1);
    m_Comms.Notify("KF_ROT", msg);
    if (m_verbose)
        MOOSTrace("%s: %s\n",m_sAppName.c_str(), msg.c_str());
    delete(rot);
}


void ActuateKFAC::ProcessAccelerationData()
{
    // Only run this function if talking with the Kingfisher
    if (m_no_comms_with_kf)
        return;


    clearpath::DataPlatformAcceleration* acc;
    acc = GetLatestData<clearpath::DataPlatformAcceleration>();
    if (!acc)
        return;

    double x = acc->getX();
    double y = acc->getY();
    double z = acc->getZ();

    m_Comms.Notify("KF_ACC_X", x);
    m_Comms.Notify("KF_ACC_Y", y);
    m_Comms.Notify("KF_ACC_Z", z);
    m_AppCastValues["KF_ACC_X"]    = x;
    m_AppCastValues["KF_ACC_Y"]    = y;
    m_AppCastValues["KF_ACC_Z"]    = z;
    m_AppCastTimestamp["KF_ACC_X"] = MOOSTime();
    m_AppCastTimestamp["KF_ACC_Y"] = MOOSTime();
    m_AppCastTimestamp["KF_ACC_Z"] = MOOSTime();

    string msg = "acc_x="   + doubleToString(x, 1);
    msg +=       ", acc_y=" + doubleToString(y, 1);
    msg +=       ", acc_z=" + doubleToString(z, 1);
    m_Comms.Notify("KF_ACC", msg.c_str());
    if (m_verbose)
        MOOSTrace("%s: %s\n",m_sAppName.c_str(), msg.c_str());
    delete acc;
}


void ActuateKFAC::ProcessSystemStatusData()
{
    // Only run this function if talking with the Kingfisher
    if (m_no_comms_with_kf)
        return;


    clearpath::DataSystemStatus* status;
    status = GetLatestData<clearpath::DataSystemStatus>();
    if (!status)
        return;

    unsigned int i;
    unsigned int v_count = status->getVoltagesCount();
    unsigned int c_count = status->getCurrentsCount();

    // BATTERY VOLTAGE
    double total_voltage = 0;
    for (i = 0; i < v_count; i++) {
        double voltage = status->getVoltage(i);
        string voltage_str = uintToString(i) + "," + doubleToString(voltage);
        m_Comms.Notify("KF_VOLTAGE_" + uintToString(i), voltage);
        m_AppCastValues["KF_VOLTAGE_" + uintToString(i)] = voltage;
        m_AppCastTimestamp["KF_VOLTAGE_" + uintToString(i)] = MOOSTime();
        m_Comms.Notify("KF_VOLTAGE_STR", voltage_str);
        total_voltage += voltage; }
    double voltage_average = total_voltage / (double)(v_count);
    m_Comms.Notify("KF_VOLTAGE_AVG", voltage_average);
    m_AppCastValues["KF_VOLTAGE_AVG"] = voltage_average;
    m_AppCastTimestamp["KF_VOLTAGE_AVG"] = MOOSTime();

    // Check if voltage is above m_critcal_voltage level and trigger run warning if it is
    if (!m_critical_voltage_triggered && voltage_average < m_critical_voltage) {
        m_critical_voltage_triggered = true;
        string msg = "Low Voltage Warning: Average Voltage: " + doubleToString(voltage_average)  +" CRITICAL_VOLTAGE: " + doubleToString(m_critical_voltage) + "\n";
        if (m_verbose)
            MOOSTrace("%s: %s\n",m_sAppName.c_str(), msg.c_str());
        reportRunWarning(msg); }

    // BATTERY CURRENT
    double total_current = 0;
    for (i = 0; i < c_count; i++) {
        double current = status->getCurrent(i);
        string current_str = uintToString(i) + "," + doubleToString(current);
        m_Comms.Notify("KF_CURRENT", current);
        m_AppCastValues["KF_CURRENT"] = current;
        m_AppCastTimestamp["KF_CURRENT"] = MOOSTime();
        m_Comms.Notify("KF_CURRENT_" + uintToString(i), current);
        m_AppCastValues["KF_CURRENT_" + uintToString(i)] = current;
        m_AppCastTimestamp["KF_CURRENT_" + uintToString(i)] = MOOSTime();
        m_Comms.Notify("KF_CURRENT_STR", current_str);
        total_current += current; }
    double current_average = total_current / (double)(c_count);
    m_Comms.Notify("KF_CURRENT_AVG", current_average);
    m_AppCastValues["KF_CURRENT_AVG"] = current_average;
    m_AppCastTimestamp["KF_CURRENT_AVG"] = MOOSTime();

    // Check for Current Overdraw -- using a timer
    if (current_average > m_critical_current) {
        if (!m_critical_current_timer_started) {         // Timer yet to start -- so start it!
            m_critical_current_timer_started = true;
            m_critical_current_timer = MOOSTime(); }    // Assign the timer the current time!
        else { // Timer has been started -- check to see if need to fire a run warning
            double time_diff = MOOSTime() - m_critical_current_timer;
            if (time_diff > m_critical_current_timeout) { //trigger a run warning?
                string msg = "Current Draw Warning: Average Current Draw: " + doubleToString(current_average)+" for more than "+ doubleToString(m_critical_current_timeout)+  " seconds.";
                if (m_verbose)
                    MOOSTrace("%s: %s\n",m_sAppName.c_str(), msg.c_str());
                reportRunWarning(msg); } } }
    else { // Timer off
        m_critical_current_timer_started = false; }

    delete(status);
}


void ActuateKFAC::CommandThrust(double left, double right)
{
    // Clamp the values to [-100, 100]
    left  = clamp(left, -100, 100);
    right = clamp(right, -100, 100);

    // Only command the left/right values if talking with the Kingfisher
    if (!m_no_comms_with_kf)
        clearpath::SetDifferentialOutput (left, right).send();

    // Publish the left/right values
    m_Comms.Notify ("KF_COMMANDED_L", left);
    m_AppCastValues["KF_COMMANDED_L"] = left;
    m_AppCastTimestamp["KF_COMMANDED_L"] = MOOSTime();
    m_Comms.Notify ("KF_COMMANDED_R", right);
    m_AppCastValues["KF_COMMANDED_R"] = right;
    m_AppCastTimestamp["KF_COMMANDED_R"] = MOOSTime();
}

bool ActuateKFAC::Iterate()
{
    // TODO: Clean up debug output
    // TODO: Add support for other control modes
    //         - SetVelocity
    //         - SetTurn
    //         - Setting differential directly
    double t = MOOSTime();
    AppCastingMOOSApp::Iterate();

    // If program status MALCONFIG -- do nothing
    if (m_program_status == "MALCONFIG") {
        AppCastingMOOSApp::PostReport();
        return true; }

    // TWO KINDS OF CONTROL - changed by boolean MOOS message "DIRECT_THRUST_CONTROL"
    //      1. NORMAL CONTROL using DESIRED_THRUST and DESIRED_RUDDER
    //      2. DIRECT THRUST CONTROL using DIRECT_THRUST_L and DIRECT_THRUST_R

    // 1. DIRECT THRUST CONTROL
    //      - Verify values are in range [-100,100]
    if (m_DirectControlMode) {
        // TODO: Insert watchdog timer to shutdown with old age
        CommandThrust(m_ThrustL, m_ThrustR); }

    // 2. NORMAL CONTROL
    else {
        double thrustRequestAge = t - m_thrustTime;
        double rudderRequestAge = t - m_rudderTime;

        // Check that requested thrust and rudder are within timeout
        //      - Convert to left/right command values and push to vehicle
        //      - If timeout exceeded, left and right are 0.0 (allstop)
        double left  = 0.0;
        double right = 0.0;
        if ((thrustRequestAge >= m_timeout) && (rudderRequestAge >= m_timeout)) {
          m_rudderCommanded = 0.0;
          m_thrustCommanded = 0.0;}
        CalculateThrust(left, right);
        CommandThrust(left, right); }

    // Check other info from vehicle
    m_mag_received = false;
    m_orientation_received = false;
    #if 0
    if (m_process_rotation_data)        ProcessRotationData();
    if (m_process_acceleration_data)    ProcessAccelerationData();
    #endif
    if (m_process_orientation_data)     ProcessOrientationData();
    if (m_process_systemstatus_data)    ProcessSystemStatusData();
    if (m_process_magnetometer_data) {
        ProcessMagnetometerData();
        if (!m_magdec_set) {
            string msg = "Kingfisher compass in use without setting magnetic declination. Expect error in reported heading.";
            if (m_verbose)
                MOOSTrace("%s: %s\n",m_sAppName.c_str(), msg.c_str());
            reportRunWarning(msg);
            m_Comms.Notify("KF_WARNING", msg); } }

    AppCastingMOOSApp::PostReport();    
    return true;
}


bool ActuateKFAC::OnStartUp()
{
    AppCastingMOOSApp::OnStartUp();
    DoRegistrations();
    return true;
}


void ActuateKFAC::DoRegistrations()
{
    AppCastingMOOSApp::RegisterVariables();
    m_Comms.Register("DESIRED_RUDDER", 0);
    m_Comms.Register("DESIRED_THRUST", 0);
    m_Comms.Register("DIRECT_THRUST_L", 0);
    m_Comms.Register("DIRECT_THRUST_R", 0);
    m_Comms.Register("DIRECT_THRUST_CONTROL", 0);
}


void ActuateKFAC::ShowPlatformInfo()
{
    // Only run this function if talking with the Kingfisher
    if (m_no_comms_with_kf)
        return;


    clearpath::DataPlatformInfo *platform_info = 0;;
    platform_info = clearpath::DataPlatformInfo::getUpdate();
    cout << *platform_info << endl;
    delete platform_info;
    
    clearpath::DataFirmwareInfo *fw_info = 0;;
    fw_info = clearpath::DataFirmwareInfo::getUpdate();
    cout << *fw_info << endl;
    delete fw_info;
    
    clearpath::DataPowerSystem *power_info = 0;;
    power_info = clearpath::DataPowerSystem::getUpdate();
    cout << *power_info << endl;
    delete power_info;
    
    clearpath::DataProcessorStatus *cpu_info = 0;;
    cpu_info = clearpath::DataProcessorStatus::getUpdate();
    cout << *cpu_info << endl;
    delete cpu_info;
}


void ActuateKFAC::CalculateThrust (double &left, double &right)
{
    // 1. Constrain Values
    //      DESIRED_RUDDER value to MAX_RUDDER
    //          - Anything more extreme than +/-50.0 is turn-in-place
    //      DESIRED_THRUST value to MAX_THRUST
    //          - Anything greater than +/-100.0% makes no sense
    double desiredRudder = clamp (m_rudderCommanded, (-1.0 * MAX_RUDDER), MAX_RUDDER);
    if (m_verbose && desiredRudder != m_rudderCommanded) {
        MOOSTrace ("iActuationKF WARNING: DESIRED_RUDDER of %.1f constrained to %.1f\n", m_rudderCommanded, desiredRudder); }
    double desiredThrust = clamp (m_thrustCommanded, (-1.0 * MAX_THRUST), MAX_THRUST);
    if (m_verbose && desiredThrust != m_thrustCommanded) {
        MOOSTrace ("iActuationKF WARNING: DESIRED_THRUST of %.1f constrained to %.1f\n", m_thrustCommanded, desiredThrust); }

    // 2. Calculate turn
    //      - ADD rudder to left thrust
    //      - SUBTRACT rudder from right thrust
    double percentLeft  = desiredThrust + desiredRudder;
    double percentRight = desiredThrust - desiredRudder;

    // 3. Map desired thrust values to motor bounds
    //      - Range of DESIRED_THRUST: [-MAX_THRUST, MAX_THRUST]
    //      -          ...map to...
    //      - Range of valid thrust values: [-m_MaxThrustValue, m_MaxThrustValue]
    double fwdOrRevL   = (percentLeft  > 0.0) ? 1.0 : -1.0;
    double fwdOrRevR   = (percentRight > 0.0) ? 1.0 : -1.0;
    double pctThrustL  = fabs (percentLeft)  / MAX_THRUST;
    double pctThrustR  = fabs (percentRight) / MAX_THRUST;
    double mappedLeft  = pctThrustL * m_MaxThrustValue * fwdOrRevL;
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
    
    left  = clamp (mappedLeft, (-1.0 * m_MaxThrustValue), m_MaxThrustValue);
    right = clamp (mappedRight, (-1.0 * m_MaxThrustValue), m_MaxThrustValue);

    if (m_verbose) {
        MOOSTrace ("DES_ T%.1f R%.1f", m_thrustCommanded, m_rudderCommanded);
        MOOSTrace ("\t\tConst: T%.1f R%.1f", desiredThrust, desiredRudder);
        MOOSTrace ("\t\tPct L%.1f R%.1f", percentLeft, percentRight);
        MOOSTrace ("\t\tMap: L%.1f R%.1f", mappedLeft, mappedRight);
        MOOSTrace ("\t\tFinal: L%.1f R%.1f", left, right);
        MOOSTrace ("\tOffset: %c\n", cOffset); }
}


bool ActuateKFAC::buildReport()
{
    //Display static configuration parameters
    m_msgs << endl;
    m_msgs << "==================================================" << endl;
    m_msgs << "  Configuration Parameters" << endl;
    m_msgs << "==================================================" << endl;
    m_msgs << endl;
    m_msgs << "  CRITICAL_VOLTAGE:         " << m_critical_voltage << endl;
    m_msgs << "  CRITICAL_CURRENT:         " << m_critical_current << endl;
    m_msgs << "  CIRITCAL_CURRENT_TIMEOUT: " << m_critical_current_timeout << endl;
    m_msgs << endl;
    m_msgs << endl;

    //start showing runtime information
    m_msgs << "==================================================" << endl;
    m_msgs << "  Runtime Information " << endl;
    m_msgs << "==================================================" << endl;

    //start a table for all the variables to be displayed
    ACTable actab(3);
    actab << "Variable | Time | Value";
    actab.addHeaderLines();
    actab.setColumnMaxWidth(4, 55);
    actab.setColumnNoPad(4);
    map<string,double>::iterator q;
    for (q = m_AppCastValues.begin(); q != m_AppCastValues.end(); q++) {
        string varname = q->first;
        double value = q->second;
        double timeSinceSentToMOOSDB = MOOSTime() - m_AppCastTimestamp[varname];
        actab << varname << timeSinceSentToMOOSDB << value; }
    m_msgs << endl << endl;
    m_msgs << actab.getFormattedString();
    return true;
}

