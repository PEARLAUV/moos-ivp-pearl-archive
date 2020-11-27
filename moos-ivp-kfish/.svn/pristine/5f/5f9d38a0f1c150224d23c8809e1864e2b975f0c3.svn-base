/************************************************************/
/*    FILE: iActuateKFAC.h                                 */
/*    DATE: May 22, 2013                                   */
/************************************************************/

#ifndef IACTUATEKFAC_HEADER
#define IACTUATEKFAC_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "clearpath.h"

#define MAX_RUDDER  50.0
#define MAX_THRUST 100.0

class ActuateKFAC : public AppCastingMOOSApp
{
public:
    ActuateKFAC();
    virtual ~ActuateKFAC() {};

    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();
    bool buildReport();


private:
    /**
     * Retrieves the latest message from a particular Clearpath message queue.
     *
     * @return a pointer to the latest message and 0 if there was no message
     *         in the queue. The type is one of the Clearpath datatypes.
     *
     * The caller needs to delete the returned pointer.
     *
     */
    template<class Data>
    Data* GetLatestData();
    
    void DoRegistrations();
    void ShowPlatformInfo();
    void ProcessMagnetometerData();
    void ProcessOrientationData();
    void ProcessRotationData();
    void ProcessAccelerationData();
    void ProcessSystemStatusData();
    void CalculateThrust (double &left, double &right);
    void CommandThrust (double left, double right);

    /// The Kingfisher communication port
    std::string m_port;
    /// Timeout for the Kingfisher commands. If no command
    /// is received within this time the driver will stop
    /// sending commands.
    double m_timeout;
    
    double m_thrustCommanded;
    double m_thrustTime;
    double m_rudderCommanded;
    double m_rudderTime;
    double m_magdec;
    // False if the magdec has never been set.
    bool m_magdec_set;
    
    // True - compute heading from magnetometer.
    bool m_compute_heading;
    // Offsets on magnetometer
    double m_mag_offsets[3];
    // Compass to Vehicle heading offset in degrees
    double m_heading_offset;
    
    // Last received orientation values
    bool  m_orientation_received;
    double m_roll;
    double m_pitch;
    double m_yaw;
    
    // Last received mag values
    bool m_mag_received;
    double m_mag_xyz[3];
    
    bool m_process_orientation_data;
    bool m_process_rotation_data;
    bool m_process_acceleration_data;
    bool m_process_magnetometer_data;
    bool m_process_systemstatus_data;
    bool m_verbose;

    std::string THRUST_COMMAND;
    std::string RUDDER_COMMAND;

    // Map DESIRED_THRUST to throttle
//    double m_OffsetLeft;             // OFFSET_LEFT
//    double m_OffsetRight;            // OFFSET_RIGHT
//    double m_MinTurnValue;           // MIN_TURN_VALUE
    double m_MaxThrustValue;         // MAX_THRUST_VALUE
    
    // Direct Thrust Control
    bool m_DirectControlMode;       // message DIRECT_THRUST_CONTROL
    double m_ThrustL;               // message DIRECT_THRUST_L
    double m_ThrustR;               // message DIRECT_THRUST_R
    double m_DirectThrustTime;

    // Progressive Offsets
    double m_Offset_LT10;
    double m_Offset_GTE10_LT20;
    double m_Offset_GTE20_LT30;
    double m_Offset_GTE30_LT40;
    double m_Offset_GTE40_LT50;
    double m_Offset_GTE50_LT60;
    double m_Offset_GTE60_LT70;
    double m_Offset_GTE70_LT80;
    double m_Offset_GTE80_LT90;
    double m_Offset_GTE90;

protected:
//For program status so that it does not crash
//instead still writes appcasts with error

    std::string m_program_status;

    double m_critical_voltage;
    bool   m_critical_voltage_triggered;
    double m_critical_current;
    double m_critical_current_timeout;
    double m_critical_current_timer;
    bool   m_critical_current_timer_started;

    bool m_no_comms_with_kf;

    //For AppCasting a mapping for reported messages to the MOOSDB
    std::map<std::string, double>     m_AppCastValues;
    std::map<std::string, double>     m_AppCastTimestamp;
};

#endif 

