/***************************************************************/
/*  NAME: Alon Yaari                                           */
/*  ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*  FILE: M200.h                                               */
/*  DATE: Spring 2014                                          */
/***************************************************************/

#ifndef I_M200_H
#define I_M200_H

#include <sys/socket.h>
#include <string>
#include <netdb.h>
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
  
  bool  OnNewMail(MOOSMSG_LIST &NewMail);
  bool  Iterate();
  bool  OnConnectToServer();
  bool  OnStartUp();

 protected:
  void  RegisterVariables();

 protected: // Handle Config Params
  bool  SetParam_IP_ADDRESS(std::string sVal);
  bool  SetParam_PORT_NUMBER(std::string sVal);
  bool  SetParam_HEADING_SOURCE(std::string sVal);
  bool  SetParam_HEADING_MSG_NAME(std::string sVal);
  bool  SetParam_MAG_OFFSET(std::string sVal);

  bool  handleSetThrustMode(std::string sVal);

 protected: // Handle Sockets, I/O
  bool  OpenConnectionTCP(std::string addr, std::string port);
  bool  OpenSocket();
  bool  Connect();
  bool  Send();
  bool  SendHeartBeat();
  bool  Receive();

 protected: // NMEA Processing
  bool  DealWithNMEA(std::string nmea);
  bool  ParseCPNVG(std::string nmea);
  bool  ParseCPRCM(std::string nmea);
  bool  ParseGPRMC(std::string nmea);
  bool  ParseCPRBS(std::string nmea);
  bool  NMEAChecksumIsValid(std::string nmea);

 protected: // Other Utilities
  void  PublishHeading(double dHeading);
  bool  staleModeCheck();

 protected: // Build AppCast Report
  bool  buildReport();
  
 protected:
  // struct getaddrinfo() fills up with data
  struct addrinfo   host_info;         
  // Pointer to the linked list of host_info
  struct addrinfo*  host_info_list;    

  int           socketfd;          // The socket descriptor
  char          inBuff[MAX_BUFF];  // char buffer for incoming data
  std::string   m_heading_source;
  std::string   m_heading_msg_name;
  bool          m_post_raw_msgs;

  double        m_batt_voltage;
  double        m_mag_offset;
  bool          m_mag_offset_using;
  
  unsigned int  m_msgs_to_front;
  unsigned int  m_msgs_from_front;

  std::string   m_thrust_mode;

 protected: // The DESIRED values from payload community
  double        m_des_thrust_lft;
  double        m_des_thrust_rgt;
  double        m_des_thrust_lft_tstamp;
  double        m_des_thrust_rgt_tstamp;

  double        m_des_rudder;
  double        m_des_thrust;
  double        m_des_rudder_tstamp;
  double        m_des_thrust_tstamp;

  double        m_des_hdg;
  double        m_des_spd;
  double        m_des_hdg_tstamp;
  double        m_des_spd_tstamp;

 protected: // Staleness info related m_des_ variables
  double        m_stale_threshold;
  unsigned int  m_stale_detections;
  bool          m_stale_mode;

 protected: // A summary of L/R commands regardless of source
  double        m_command_thrust_lft;
  double        m_command_thrust_rgt;

 protected: // Variables concerning network connection
  std::string   m_IP;
  std::string   m_Port;
  bool          m_connected;
};

#endif
