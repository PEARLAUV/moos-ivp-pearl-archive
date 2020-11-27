/************************************************************/
/*    FILE: iNewKF.h                                        */
/*    DATE: Nov 22, 2012                                    */
/************************************************************/

#ifndef INEWKF_HEADER
#define INEWKF_HEADER

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "ClearpathAPI.h"

#define MAX_RUDDER  50.0
#define MAX_THRUST 100.0

class NewKF : public CMOOSApp
{
public:
    NewKF();
    virtual ~NewKF();

    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();
    
private:
    bool GetAndPublishLatestClearpathData();
    
    void DoRegistrations();
    void CalculateThrust (double &left, double &right);

    /// The Kingfisher communication port on localhost
    int m_port;

    /// Timeout for the Kingfisher commands. If no command
    /// is received within this time the driver will stop
    /// sending commands.
    double m_timeout;
    
    double m_thrustCommanded;
    double m_thrustTime;
    double m_rudderCommanded;
    double m_rudderTime;
    double m_compassOffset;
    
    bool m_verbose;

    std::string msgCmdThrust;
    std::string msgCmdRudder;
    std::string msgCompass;
    std::string msgVoltage;

    double m_MaxThrustValue;         // MAX_THRUST_VALUE
    double m_MinThrustValue;         // MIN_THRUST_VALUE
    double m_PublishVTime;           // VOLTAGE_PUB_TIME
    
    // Direct Thrust Control
    bool  m_DirectControlMode;       // message DIRECT_THRUST_CONTROL
    double m_ThrustL;                // message DIRECT_THRUST_L
    double m_ThrustR;                // message DIRECT_THRUST_R
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

    std::string m_appName;
    double curTime;
    double curCompass;
    double curVoltage;
    double lastVNotifyTime;

    bool testMode;

};

#endif 

