/*********************************************************************/
/* M200.h                                                            */
/*                                                                   */
/*********************************************************************/
#ifndef I_M200_H
#define I_M200_H

#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "CPNVGnmea.h"
//#include "CPRCMnmea.h"
#include "GPRMCnmea.h"
#include "CPRBSnmea.h"

#define MAX_BUFF 10000
#define HEADING_OPTIONS "NONE RAW_COMPASS RAW_GPS NAV_UPDATE"

class iM200 : public AppCastingMOOSApp {

public:
            iM200();
            ~iM200() {};

// protected:
    bool    buildReport();
    bool    Iterate();
    void    RegisterVariables();
    bool    OnNewMail(MOOSMSG_LIST &NewMail);
    bool    HandleMail_Des_Heading(double d);
    bool    HandleMail_Des_Speed(double d);
    bool    HandleMail_Commanded_L(double d);
    bool    HandleMail_Commanded_R(double d);
    bool    OnConnectToServer();
    bool    OnStartUp();
    bool    SetParam_IP_ADDRESS(std::string sVal);
    bool    SetParam_PORT_NUMBER(std::string sVal);
    bool    SetParam_DIRECT_THRUST_MODE(std::string sVal);
    bool    SetParam_HEADING_SOURCE(std::string sVal);
    bool    SetParam_HEADING_MSG_NAME(std::string sVal);
    bool    SetParam_MAG_OFFSET(std::string sVal);


    bool OpenConnectionTCP(std::string addr, std::string port);
    bool OpenSocket();
    bool Connect();
    bool Send();
    bool Receive();
    bool DealWithNMEA(std::string nmea);
    bool ParseCPNVG(std::string nmea);
    bool ParseCPRCM(std::string nmea);
    bool ParseGPRMC(std::string nmea);
    bool ParseCPRBS(std::string nmea);
    bool NMEAChecksumIsValid(std::string nmea);

    struct addrinfo     host_info;              // The struct that getaddrinfo() fills up with data
    struct addrinfo*    host_info_list;         // Pointer to the to the linked list of host_info
    int                 socketfd;               // The socket descriptor
    char                inBuff[MAX_BUFF];       // Persistent char buffer for incoming data
    bool                m_direct_thrust_mode;
    std::string         m_heading_source;
    std::string         m_heading_msg_name;
    double              m_magOffset;
    bool                m_bHaveMagValue;

    int                 countSentToM200;
    int                 countFromM200;
    double              dDesHeading;
    double              dDesSpeed;
    double              dCommandL;
    double              dCommandR;
    CMOOSGeodesy        m_Geodesy;
    std::string         m_IP;
    std::string         m_Port;
    std::string         strRpt;                 // Used for generating warning text
};



#endif



















//
