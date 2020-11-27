/************************************************************/
/*    NAME: Hordur Johannsson                               */
/*          Mike Benjamin - later modifications             */
/*    FILE: KFC_MOOSApp.cpp                                 */
/*    DATE: March 22, 2011                                  */
/************************************************************/

#include "KFC_MOOSApp.h"
#include "MBUtils.h"
#include <iostream>
#include <cmath>

using namespace std;

//--------------------------------------------------------- 
// Procedure: clamp
//   Purpose: Clamps the value of v between minv and maxv

double clamp(double v, double minv, double maxv)
{
    return std::min(maxv,std::max(minv, v));
}

//---------------------------------------------------------
// Constructor

KFC_MOOSApp::KFC_MOOSApp() 
{
    // Initialize state Variables
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
    
    for (int i=0; i<3; ++i) m_mag_offsets[i] = 0.0;
    for (int i=0; i<3; ++i) m_mag_xyz[i] = 0.0;
    
    m_timeout = 6;  // seconds
    
    m_process_orientation_data  = false;
    m_process_rotation_data     = false;
    m_process_acceleration_data = false;
    m_process_magnetometer_data = false;
    m_process_systemstatus_data = true;
}

//---------------------------------------------------------
// Destructor

KFC_MOOSApp::~KFC_MOOSApp()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool KFC_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;
    for (p=NewMail.begin(); p!=NewMail.end(); ++p) {
        CMOOSMsg & rMsg = *p;
        if (MOOSStrCmp(rMsg.GetKey(), THRUST_COMMAND )) {
            m_thrustCommanded = rMsg.GetDouble();
            m_thrustTime = rMsg.GetTime();
        } else if (MOOSStrCmp(rMsg.GetKey(), RUDDER_COMMAND )) {
            m_rudderCommanded = rMsg.GetDouble();
            m_rudderTime = rMsg.GetTime();
        } else if (MOOSStrCmp(rMsg.GetKey(), "GPS_MAGNETIC_DECLINATION")) {
            double magdec = rMsg.GetDouble();
            if (m_magdec != magdec) {
                MOOSTrace("Setting magnetic declination to %lf\n", magdec);
                m_magdec = magdec;
                m_magdec_set = true;
            }
        }
    }
    
    return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool KFC_MOOSApp::OnConnectToServer()
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
Data* KFC_MOOSApp::GetLatestData()
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

void KFC_MOOSApp::ProcessOrientationData()
{
    clearpath::DataPlatformOrientation* orientation = 0;
    orientation = GetLatestData<clearpath::DataPlatformOrientation>();
    
    if(orientation) {
        m_orientation_received = true;  
        
        if(!m_compute_heading) {
            double hdg = orientation->getYaw()/M_PI*180.0 + m_magdec;
            m_Comms.Notify("COMPASS_HEADING", hdg);
        }
        
        m_roll = orientation->getRoll();
        m_pitch = orientation->getPitch();
        m_yaw = orientation->getYaw();
        
        m_Comms.Notify("COMPASS_YAW", m_yaw);
        m_Comms.Notify("COMPASS_PITCH", m_pitch/M_PI*180.0);
        m_Comms.Notify("COMPASS_ROLL", m_roll/M_PI*180.0);
        
        delete(orientation);
    }
}

//---------------------------------------------------------
// Procedure: ProcessMagnetometerData()

void KFC_MOOSApp::ProcessMagnetometerData()
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
        
        m_Comms.Notify("COMPASS_MAG", msg);
        m_Comms.Notify("COMPASS_MAG_X", x);
        m_Comms.Notify("COMPASS_MAG_Y", y);
        m_Comms.Notify("COMPASS_MAG_Z", z);
        
        x += m_mag_offsets[0];
        y += m_mag_offsets[1];
        z += m_mag_offsets[2];
        
        // @todo  Rotate by roll/pitch
        
        double yaw = -atan2(y,x) + m_heading_offset/180.0*M_PI;
        m_Comms.Notify("COMPASS_YAW_COMPUTED", yaw);
        m_Comms.Notify("COMPASS_HEADING_COMPUTED", yaw/M_PI*180.0 + m_magdec);
        
        if(m_compute_heading)
            m_Comms.Notify("COMPASS_HEADING", yaw/M_PI*180.0 + m_magdec);
        
        delete(mag);
    }
}

//---------------------------------------------------------
// Procedure: ProcessRotationData()

void KFC_MOOSApp::ProcessRotationData()
{
    clearpath::DataPlatformRotation* rot;
    rot = GetLatestData<clearpath::DataPlatformRotation>();
    if (rot) {
        char msg[255];
        double roll_rate = rot->getRollRate();
        double pitch_rate = rot->getPitchRate();
        double yaw_rate = rot->getYawRate();
        
        snprintf(msg, 255, "x=%lf,y=%lf,z=%lf",roll_rate,pitch_rate,yaw_rate);
        
        m_Comms.Notify("COMPASS_ROT", msg);
        m_Comms.Notify("COMPASS_ROT_ROLL", roll_rate);
        m_Comms.Notify("COMPASS_ROT_PITCH", pitch_rate);
        m_Comms.Notify("COMPASS_ROT_YAW", yaw_rate);
        
        delete(rot);
    }
}

//---------------------------------------------------------
// Procedure: ProcessAccelerationData()

void KFC_MOOSApp::ProcessAccelerationData()
{
    clearpath::DataPlatformAcceleration* acc;
    acc = GetLatestData<clearpath::DataPlatformAcceleration>();
    if (acc) {
        char msg[255];
        double x = acc->getX();
        double y = acc->getY();
        double z = acc->getZ();
        
        snprintf(msg, 255, "x=%lf,y=%lf,z=%lf",x,y,z);
        
        m_Comms.Notify("COMPASS_ACC", msg);
        m_Comms.Notify("COMPASS_ACC_X", x);
        m_Comms.Notify("COMPASS_ACC_Y", y);
        m_Comms.Notify("COMPASS_ACC_Z", z);
        
        delete acc;
    }
}

//---------------------------------------------------------
// Procedure: ProcessSystemStatusData()

void KFC_MOOSApp::ProcessSystemStatusData()
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
            m_Comms.Notify("CP_VOLTAGE_"+uintToString(i), voltage);
            m_Comms.Notify("CP_VOLTAGE_STR", voltage_str);
            total_voltage += voltage;
        }    
        m_Comms.Notify("CP_VOLTAGE_AVG", total_voltage/(double)(v_count));
        
        double total_current = 0;
        for(i=0; i<c_count; i++) {
            double current = status->getCurrent(i);
            string current_str = uintToString(i) + "," + doubleToString(current);
            m_Comms.Notify("CP_CURRENT", current);
            m_Comms.Notify("CP_CURRENT_"+uintToString(i), current);
            m_Comms.Notify("CP_CURRENT_STR", current_str);
            total_current += current;
        }    
        m_Comms.Notify("CP_CURRENT_AVG", total_current/(double)(c_count));
    }
    
    delete(status);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool KFC_MOOSApp::Iterate()
{
    double t = MOOSTime();
    
    // @todo cleanup debug output
    // @todo add support for other control modes
    //         -- SetVelocity
    //         -- SetTurn
    //         -- Setting differential directly
    
    if(((t-m_thrustTime) < m_timeout) && ((t-m_rudderTime) < m_timeout)) {
        double left = clamp(m_thrustCommanded+m_rudderCommanded, -100.0, 100.0);
        double right = clamp(m_thrustCommanded-m_rudderCommanded, -100.0, 100.0);
        clearpath::SetDifferentialOutput(left, right).send();    
        //cout << "Send: " << left << " " << right << endl;
    }
    else // timeout
        clearpath::SetDifferentialOutput(0, 0).send();
    
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
        if(m_verbose)
            cout << "Processing system status data" << endl;
        ProcessSystemStatusData();
    }
    
    cout << "Done Processing data" << endl;
    
    char msg[512];
    snprintf(msg, sizeof(msg), "%lf,%lf,%lf,%d,%lf,%lf,%lf,%d",m_roll,m_pitch,m_yaw,m_orientation_received,
             m_mag_xyz[0],m_mag_xyz[1],m_mag_xyz[2],m_mag_received);
    m_Comms.Notify("COMPASS_DATA", msg);
    
    if(!m_magdec_set) {
        string warning = "Magnetic declination has not been set.";
        m_Comms.Notify("COMPASS_WARNING", warning);
    }
    
    return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool KFC_MOOSApp::OnStartUp()
{
    DoRegistrations();
    return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void KFC_MOOSApp::DoRegistrations()
{
    m_Comms.Register("DESIRED_RUDDER", 0);
    m_Comms.Register("DESIRED_THRUST", 0);
}

//---------------------------------------------------------
// Procedure: ShowPlatformInfo()

void KFC_MOOSApp::ShowPlatformInfo()
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
