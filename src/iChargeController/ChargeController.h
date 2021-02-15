/*
 * ChargeController.h
 * 
 * Created on: 2/6/2021
 * Author: Ethan Rolland
 *
 */

#ifndef CHARGECONTROLLER_H_
#define CHARGECONTROLLER_H_

#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "SerialComms.h"

class CHARGE : public AppCastingMOOSApp
{
 public:
        CHARGE();
        ~CHARGE() {};
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   bool buildReport();
 protected:
   void RegisterForMOOSMessages();
   
   bool SerialSetup();
   void GetData();
   
   //Handle config params
   bool SetParam_PORT(std::string sVal);                //m_serial_port
   bool SetParam_BAUDRATE(std::string sVal);            //m_baudrate
   bool SetParam_PREFIX(std::string sVal);              //m_prefix
   bool SetParam_READ_DATA(std::string sVal);           //m_read_data
   bool SetParam_MAX_THRUST(std::string sVal);          //m_max_thrust
   bool SetParam_MAX_RUDDER(std::string sVal);          //m_max_rudder
   
   bool SetPublishNames();
   
   //MOOS file parameters
   std::string   m_serial_port;
   int           m_baudrate;
   std::string   m_prefix;
   bool          m_read_data;
   double        m_max_thrust;
   double        m_max_rudder;
   
   SerialComms*  m_serial;
   bool          m_bValidSerialConn;
   
   double        currMaxThrust;
   double        currMaxRudder;
   
   //Publish name
   std::string   m_pubNameMaxThrust;
   std::string   m_pubNameMaxRudder;
   
   //Appcast details
   unsigned int  m_msgs_from_device;
   std::string   m_last_msg_from_device;
   
};

#endif
