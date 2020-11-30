/************************************************************/
/*    FILE: KFC_MOOSApp.h                                   */
/*    DATE: March 22, 2011                                  */
/************************************************************/

#ifndef KFC_MOOS_APP_HEADER
#define KFC_MOOD_APP_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "clearpath.h"

class KFC_MOOSApp : public CMOOSApp
{
public:
    KFC_MOOSApp();
    virtual ~KFC_MOOSApp();
    
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();
    
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
};

#endif 
