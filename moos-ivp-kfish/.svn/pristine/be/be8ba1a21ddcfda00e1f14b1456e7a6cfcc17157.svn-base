/************************************************************/
/*    NAME: Alon Yaari
/*
/*    BASED ON: iKFController written by
/*              Hordur Johannsson and Mike Benjamin
/*    FILE: iActuationKFApp.cpp
/*    DATE: April 24, 2012
/************************************************************/


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
#include "iActuationKFApp.h"
#include "MBUtils.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.14159
#endif

#ifdef WIN32
#define snprintf _snprintf
#endif

//--------------------------------------------------------- 
// Procedure: clamp
//   Purpose: Clamps the value of v between minv and maxv

double clamp(double v, double minv, double maxv)
{
    return min(maxv,max(minv, v));
}

//---------------------------------------------------------
// Constructor

ActuateKF::ActuateKF() 
{
    // Initialize state Variables
    m_port = "/dev/KINGFISHER";
    m_thrustCommanded = 0;
    m_thrustTime      = 0;
    m_rudderCommanded = 0;
    m_rudderTime      = 0;
    m_magdec          = 0;
    m_magdec_set      = false;
    
    m_orientation_received = false;
    m_roll            = 0;
    m_pitch           = 0;
    m_yaw             = 0;
    m_mag_received    = false;
    
    // Initialize configuration variables
    m_heading_offset  = 0;
    m_compute_heading = false;
    m_verbose = false;

    // Mapping thrust and rudder to motor offsets
//    m_OffsetLeft = 0;          // Value ADDED so rudder=0 is in the center of motor's dead zone
//   m_OffsetRight = 0;         // Value ADDED so rudder=0 is in the center of motor's dead zone
//    m_MinTurnValue = 0;        // Smallest value that allows the vehicle to turn
    m_MaxThrustValue = 100;    // Maximum thrust to be commanded to the motors
    
    // Direct thrust control
    m_DirectControlMode = false;    // When true, command the motors directly
    m_ThrustL = 0.0;                // Direct thrust value to command the left motor
    m_ThrustR = 0.0;                // Direct thrust value to commamnd the right motor
    m_DirectThrustTime = 0.0;       // Last time direct thrust was commanded

    for (int i=0; i<3; ++i) m_mag_offsets[i] = 0.0;
    for (int i=0; i<3; ++i) m_mag_xyz[i] = 0.0;
    
    m_timeout = 6;  // seconds
    
    m_process_orientation_data  = false;
    m_process_rotation_data     = false;
    m_process_acceleration_data = false;
    m_process_magnetometer_data = false;
    m_process_systemstatus_data = true;

    m_Offset_LT10 = 0;
    m_Offset_GTE10_LT20 = 0;
    m_Offset_GTE20_LT30 = 0;
    m_Offset_GTE30_LT40 = 0;
    m_Offset_GTE40_LT50 = 0;
    m_Offset_GTE50_LT60 = 0;
    m_Offset_GTE60_LT70 = 0;
    m_Offset_GTE70_LT80 = 0;
    m_Offset_GTE80_LT90 = 0;
    m_Offset_GTE90 = 0;
}

//---------------------------------------------------------
// Destructor

ActuateKF::~ActuateKF()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool ActuateKF::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;
    for (p=NewMail.begin(); p!=NewMail.end(); ++p) {
        CMOOSMsg & rMsg = *p;

        if (MOOSStrCmp(rMsg.GetKey(), THRUST_COMMAND )) {
            m_thrustCommanded = rMsg.GetDouble();
            m_thrustTime = rMsg.GetTime();
        }
        
        else if (MOOSStrCmp(rMsg.GetKey(), RUDDER_COMMAND )) {
            m_rudderCommanded = rMsg.GetDouble();
            m_rudderTime = rMsg.GetTime();
        }
        
        else if (MOOSStrCmp(rMsg.GetKey(), "GPS_MAGNETIC_DECLINATION")) {
            double magdec = rMsg.GetDouble();
            if (m_magdec != magdec) {
                MOOSTrace("Setting magnetic declination to %lf\n", magdec);
                m_magdec = magdec;
                m_magdec_set = true;
            }
        }
        
        else if (MOOSStrCmp(rMsg.GetKey(), "DIRECT_THRUST_CONTROL")) {
            string sVal = p->GetString();
            MOOSToUpper(sVal);
            if (MOOSStrCmp (sVal, "TRUE")) {
                m_DirectControlMode = true;
                MOOSTrace ("\n------------------\n");
                MOOSTrace ("iActuationKF: DIRECT THRUST CONTROL MODE ON (ignoring DESIRED_THRUST/RUDDER\n");
                MOOSTrace ("------------------\n\n");
            }
            else {
                m_DirectControlMode = false;
                MOOSTrace ("\n------------------\n");
                MOOSTrace ("iActuationKF: NORMAL DESIRED_THRUST/RUDDER CONTROL (direct thrust control mode off)\n");
                MOOSTrace ("------------------\n\n");
            }
        }
        
        else if (MOOSStrCmp(rMsg.GetKey(), "DIRECT_THRUST_L")) {
            m_ThrustL = p->GetDouble();
            m_DirectThrustTime = MOOSTime();
        }
        
        else if (MOOSStrCmp(rMsg.GetKey(), "DIRECT_THRUST_R")) {
            m_ThrustR = p->GetDouble();
            m_DirectThrustTime = MOOSTime();
        }
    }
    return(true);
}



//---------------------------------------------------------
// Procedure: OnConnectToServer

bool ActuateKF::OnConnectToServer()
{
    m_MissionReader.GetConfigurationParam("Port", m_port);
    m_MissionReader.GetConfigurationParam("Timeout", m_timeout);
    m_MissionReader.GetConfigurationParam("MagOffsetX", m_mag_offsets[0]);
    m_MissionReader.GetConfigurationParam("MagOffsetY", m_mag_offsets[1]);
    m_MissionReader.GetConfigurationParam("MagOffsetZ", m_mag_offsets[2]);
    m_MissionReader.GetConfigurationParam("m_verbose", m_verbose);
    
    m_MissionReader.GetConfigurationParam("HeadingOffset", 
                                          m_heading_offset);
    m_MissionReader.GetConfigurationParam("ComputeHeading", 
                                          m_compute_heading);
    m_MissionReader.GetConfigurationParam("ProcessOrientationData", 
                                          m_process_orientation_data);
    m_MissionReader.GetConfigurationParam("ProcessRotationData", 
                                          m_process_rotation_data);
    m_MissionReader.GetConfigurationParam("ProcessAccelerationData", 
                                          m_process_acceleration_data);
    m_MissionReader.GetConfigurationParam("ProcessMagnetometerData", 
                                          m_process_magnetometer_data);
    m_MissionReader.GetConfigurationParam("ProcessSystemStatusData", 
                                          m_process_systemstatus_data);
    

    // Read parameters for progressive offsets
    m_MissionReader.GetConfigurationParam("OFFSET_LT10", m_Offset_LT10);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE10_LT20", m_Offset_GTE10_LT20);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE20_LT30", m_Offset_GTE20_LT30);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE30_LT40", m_Offset_GTE30_LT40);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE40_LT50", m_Offset_GTE40_LT50);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE50_LT60", m_Offset_GTE50_LT60);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE60_LT70", m_Offset_GTE60_LT70);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE70_LT80", m_Offset_GTE70_LT80);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE80_LT90", m_Offset_GTE80_LT90);
    m_MissionReader.GetConfigurationParam("OFFSET_GTE90", m_Offset_GTE90);

    MOOSTrace ("m_Offset_LT10       = %.1f\n", m_Offset_LT10);
    MOOSTrace ("m_Offset_GTE10_LT20 = %.1f\n", m_Offset_GTE10_LT20);
    MOOSTrace ("m_Offset_GTE20_LT30 = %.1f\n", m_Offset_GTE20_LT30);
    MOOSTrace ("m_Offset_GTE30_LT40 = %.1f\n", m_Offset_GTE30_LT40);
    MOOSTrace ("m_Offset_GTE40_LT50 = %.1f\n", m_Offset_GTE40_LT50);
    MOOSTrace ("m_Offset_GTE50_LT60 = %.1f\n", m_Offset_GTE50_LT60);
    MOOSTrace ("m_Offset_GTE60_LT70 = %.1f\n", m_Offset_GTE60_LT70);
    MOOSTrace ("m_Offset_GTE70_LT80 = %.1f\n", m_Offset_GTE70_LT80);
    MOOSTrace ("m_Offset_GTE80_LT90 = %.1f\n", m_Offset_GTE80_LT90);
    MOOSTrace ("m_Offset_GTE90      = %.1f\n", m_Offset_GTE90);

    double dVal = 0;

//    if (m_MissionReader.GetConfigurationParam("OFFSET_LEFT", dVal))
//        m_OffsetLeft = dVal;
//    MOOSTrace ("iActuationKF: OFFSET_LEFT = %f\n", m_OffsetLeft);

//    if (m_MissionReader.GetConfigurationParam("OFFSET_RIGHT", dVal))
//        m_OffsetRight = dVal;
//    MOOSTrace ("iActuationKF: OFFSET_LEFT = %f\n", m_OffsetRight);
    
//    if (m_MissionReader.GetConfigurationParam("MIN_TURN_VALUE", dVal)) {
//        if (dVal < 0.0 || dVal > MAX_RUDDER) {
//            MOOSTrace ("iActuationKF ERROR: MIN_TURN_VALUE must be in range [0, 50].  Mission file defines it as %f.\n", dVal);
//            return false; }
//        m_MinTurnValue = dVal; }
//    MOOSTrace ("iActuationKF: MIN_TURN_VALUE = %f\n", m_MinTurnValue);
    
    if (m_MissionReader.GetConfigurationParam("MAX_THRUST_VALUE", dVal)) {
        if (dVal <= 0.0 || dVal > 100.0) {
            MOOSTrace ("iActuationKF ERROR: MAX_THRUST_VALUE must be in range (0, 100].  Mission file defines it as %f.\n", dVal);
            return false; }
        m_MaxThrustValue = dVal; }
    MOOSTrace ("iActuationKF: MAX_THRUST_VALUE = %f\n", m_MaxThrustValue);

    int max_retry = 3;
    clearpath::Transport::instance().configure(m_port.c_str(), max_retry);
    // @todo  see if we should have validation on timeout                                                     
    
    ShowPlatformInfo();
    
    if(m_process_orientation_data) {
        if(m_verbose)
            cout << "Subscribing orientation" << endl;
        clearpath::DataPlatformOrientation::subscribe(20);
    }
    
    if(m_process_magnetometer_data) {
        if(m_verbose)
            cout << "Subscribing magn" << endl;
        clearpath::DataPlatformMagnetometer::subscribe(20);
    }
    
#if 0
    if(m_process_rotation_data) {
        if(m_verbose) 
            cout << "Subscribing rotation" << endl;
        clearpath::DataPlatformRotation::subscribe(20);
    }
#endif
    
#if 0
    if(m_process_acceleration_data) {
        if(m_verbose)
            cout << "Subscribing acc" << endl;
        clearpath::DataPlatformAcceleration::subscribe(20);
    }
#endif
    
    if(m_process_systemstatus_data) {
        if(m_verbose)
            cout << "Subscribing sstatus" << endl;
        clearpath::DataSystemStatus::subscribe(5);
    }
    
    // @todo read from config file                                                                            
    THRUST_COMMAND = std::string("DESIRED_THRUST");
    RUDDER_COMMAND = std::string("DESIRED_RUDDER");
    
    DoRegistrations();
    return(true);
}

//---------------------------------------------------------
// Procedure: GetLatestData()
template<class Data>
Data* ActuateKF::GetLatestData()
{
    Data* data = Data::popNext();
    
    if(data) {
        Data* next = Data::popNext();
        while (next) {
            delete(data);
            data = next;
            next = Data::popNext();
        }
    }
    
    return(data);
}


//---------------------------------------------------------
// Procedure: ProcessOrientationData()

void ActuateKF::ProcessOrientationData()
{
    clearpath::DataPlatformOrientation* orientation = 0;
    orientation = GetLatestData<clearpath::DataPlatformOrientation>();
    
    if(orientation) {
        m_orientation_received = true;  
        
        if(!m_compute_heading) {
            double hdg = orientation->getYaw()/M_PI*180.0 + m_magdec;
            m_Comms.Notify("KF_HEADING", hdg);
        }
        
        m_roll = orientation->getRoll();
        m_pitch = orientation->getPitch();
        m_yaw = orientation->getYaw();
        
        m_Comms.Notify("KF_YAW", m_yaw);
        m_Comms.Notify("KF_PITCH", m_pitch/M_PI*180.0);
        m_Comms.Notify("KF_ROLL", m_roll/M_PI*180.0);
        
        delete(orientation);
    }
}

//---------------------------------------------------------
// Procedure: ProcessMagnetometerData()

void ActuateKF::ProcessMagnetometerData()
{
    clearpath::DataPlatformMagnetometer* mag = 0;
    mag = GetLatestData<clearpath::DataPlatformMagnetometer>();
    
    if(mag) {
        m_mag_received = true;
        
        char msg[255];
        double x = mag->getX();
        double y = mag->getY();
        double z = mag->getZ();
        
        m_mag_xyz[0] = x;
        m_mag_xyz[1] = y;
        m_mag_xyz[2] = z;
        
        snprintf(msg, 255, "x=%lf,y=%lf,z=%lf",x,y,z);
        
        m_Comms.Notify("KF_MAG", msg);
        m_Comms.Notify("KF_MAG_X", x);
        m_Comms.Notify("KF_MAG_Y", y);
        m_Comms.Notify("KF_MAG_Z", z);
        
        x += m_mag_offsets[0];
        y += m_mag_offsets[1];
        z += m_mag_offsets[2];
        
        // @todo  Rotate by roll/pitch
        
        double yaw = -atan2(y,x) + m_heading_offset/180.0*M_PI;
        m_Comms.Notify("KF_YAW_COMPUTED", yaw);
        m_Comms.Notify("KF_HEADING_COMPUTED", yaw/M_PI*180.0 + m_magdec);
        
        if(m_compute_heading)
            m_Comms.Notify("KF_HEADING", yaw/M_PI*180.0 + m_magdec);
        
        delete(mag);
    }
}

//---------------------------------------------------------
// Procedure: ProcessRotationData()

void ActuateKF::ProcessRotationData()
{
    clearpath::DataPlatformRotation* rot;
    rot = GetLatestData<clearpath::DataPlatformRotation>();
    if (rot) {
        char msg[255];
        double roll_rate = rot->getRollRate();
        double pitch_rate = rot->getPitchRate();
        double yaw_rate = rot->getYawRate();
        
        snprintf(msg, 255, "x=%lf,y=%lf,z=%lf",roll_rate,pitch_rate,yaw_rate);
        
        m_Comms.Notify("KF_ROT", msg);
        m_Comms.Notify("KF_ROT_ROLL", roll_rate);
        m_Comms.Notify("KF_ROT_PITCH", pitch_rate);
        m_Comms.Notify("KF_ROT_YAW", yaw_rate);
        
        delete(rot);
    }
}

//---------------------------------------------------------
// Procedure: ProcessAccelerationData()

void ActuateKF::ProcessAccelerationData()
{
    clearpath::DataPlatformAcceleration* acc;
    acc = GetLatestData<clearpath::DataPlatformAcceleration>();
    if (acc) {
        char msg[255];
        double x = acc->getX();
        double y = acc->getY();
        double z = acc->getZ();
        
        snprintf(msg, 255, "x=%lf,y=%lf,z=%lf",x,y,z);
        
        m_Comms.Notify("KF_ACC", msg);
        m_Comms.Notify("KF_ACC_X", x);
        m_Comms.Notify("KF_ACC_Y", y);
        m_Comms.Notify("KF_ACC_Z", z);
        
        delete acc;
    }
}

//---------------------------------------------------------
// Procedure: ProcessSystemStatusData()

void ActuateKF::ProcessSystemStatusData()
{
    clearpath::DataSystemStatus* status;
    status = GetLatestData<clearpath::DataSystemStatus>();
    if(status) {
        unsigned int i;
        unsigned int v_count = status->getVoltagesCount();
        unsigned int c_count = status->getCurrentsCount();
        
        double total_voltage = 0;
        for(i=0; i<v_count; i++) {
            double voltage = status->getVoltage(i);
            string voltage_str = uintToString(i) + "," + doubleToString(voltage);
            m_Comms.Notify("KF_VOLTAGE_"+uintToString(i), voltage);
            m_Comms.Notify("KF_VOLTAGE_STR", voltage_str);
            total_voltage += voltage;
        }    
        m_Comms.Notify("KF_VOLTAGE_AVG", total_voltage/(double)(v_count));
        
        double total_current = 0;
        for(i=0; i<c_count; i++) {
            double current = status->getCurrent(i);
            string current_str = uintToString(i) + "," + doubleToString(current);
            m_Comms.Notify("KF_CURRENT", current);
            m_Comms.Notify("KF_CURRENT_"+uintToString(i), current);
            m_Comms.Notify("KF_CURRENT_STR", current_str);
            total_current += current;
        }    
        m_Comms.Notify("KF_CURRENT_AVG", total_current/(double)(c_count));
    }
    
    delete(status);
}


bool ActuateKF::Iterate()
{
    // @todo cleanup debug output
    // @todo add support for other control modes
    //         -- SetVelocity
    //         -- SetTurn
    //         -- Setting differential directly

    double t = MOOSTime();


    // TWO KINDS OF CONTROL:
    //      1. NORMAL CONTROL using DESIRED_THRUST and DESIRED_RUDDER
    //      2. DIRECT THRUST CONTROL using DIRECT_THRUST_L and DIRECT_THRUST_R
    // Switch between using boolean DIRECT_THRUST_CONTROL


    // 1. DIRECT THRUST CONTROL
    if (m_DirectControlMode) {
        
        // Verify values are in range [-100,100]
        double left = clamp(m_ThrustL, -100, 100);
        double right = clamp(m_ThrustR, -100, 100);
        clearpath::SetDifferentialOutput (left, right).send();
        if (m_verbose)
            MOOSTrace ("%s: DIRECT CONTROL Sent to Kingfisher   left: %3.1f \tright: %3.1f\n", m_sMOOSName.c_str(), left, right);
    }

    // 2. NORMAL CONTROL
    else {

        double thrustRequestAge = t - m_thrustTime;
        double rudderRequestAge = t - m_rudderTime;

        // Requested thrust and rudder are within timeout
        //      - Convert to left/right command values and push to vehicle
        if ((thrustRequestAge < m_timeout) && (rudderRequestAge < m_timeout)) {
            double left = 0.0;
            double right = 0.0;
            CalculateThrust (left, right);
            clearpath::SetDifferentialOutput (left, right).send();
            m_Comms.Notify ("KF_COMMANDED_L", left);
            m_Comms.Notify ("KF_COMMANDED_R", right);
          //  if (m_verbose)
          //      MOOSTrace ("%s: Sent to Kingfisher   left: %3.1f \tright: %3.1f\n", m_sMOOSName.c_str(), left, right);
        }

        // Too long since last requested thrust or rudder
        //      - Stop all
        else {
            clearpath::SetDifferentialOutput (0, 0).send();
            char timeoutMsg[100];
            snprintf (timeoutMsg, 100, "MaxTimeout=%.1f,ThrustRequestAge=%.1f,RudderRequestAge=%.1f", m_timeout, thrustRequestAge, rudderRequestAge);
            m_Comms.Notify ("KF_TIMEOUT", timeoutMsg); }
    }
    m_mag_received = false;
    m_orientation_received = false;  
    
    if(m_process_orientation_data) {
        if(m_verbose)
            cout << "Processing orientation data" << endl;
        ProcessOrientationData();
    }
    
#if 0
    if(m_process_rotation_data) {
        if(m_verbose)
            cout << "Processing Rotation data" << endl;
        ProcessRotationData();  
    }
#endif
    
#if 0
    if(m_process_acceleration_data) {
        if(m_verbose)
            cout << "Processing acceleration data" << endl;
        ProcessAccelerationData();
    }
#endif
    
    if(m_process_magnetometer_data) {
        if(m_verbose) 
            cout << "Processing magnetometer data" << endl;
        ProcessMagnetometerData();
    }
    
    if(m_process_systemstatus_data) {
  //      if(m_verbose)
  //          cout << "Processing system status data" << endl;
        ProcessSystemStatusData();
    }
    
    // cout << "Done Processing data" << endl;
    
    char msg[512];
    snprintf(msg, sizeof(msg), "%lf,%lf,%lf,%d,%lf,%lf,%lf,%d",m_roll,m_pitch,m_yaw,m_orientation_received,
             m_mag_xyz[0],m_mag_xyz[1],m_mag_xyz[2],m_mag_received);
    m_Comms.Notify("KF_DATA", msg);
    
    if(!m_magdec_set) {
        string warning = "Magnetic declination has not been set.";
        m_Comms.Notify("KF_WARNING", warning);
    }
    
    return(true);
}


//---------------------------------------------------------
// Procedure: OnStartUp()

bool ActuateKF::OnStartUp()
{
    DoRegistrations();
    return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void ActuateKF::DoRegistrations()
{
    m_Comms.Register("DESIRED_RUDDER", 0);
    m_Comms.Register("DESIRED_THRUST", 0);
    m_Comms.Register("DIRECT_THRUST_L", 0);
    m_Comms.Register("DIRECT_THRUST_R", 0);
    m_Comms.Register("DIRECT_THRUST_CONTROL", 0);
}

//---------------------------------------------------------
// Procedure: ShowPlatformInfo()

void ActuateKF::ShowPlatformInfo()
{
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

void ActuateKF::CalculateThrust (double &left, double &right)
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

