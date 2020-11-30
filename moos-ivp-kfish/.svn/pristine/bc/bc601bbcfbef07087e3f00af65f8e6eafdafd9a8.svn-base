/*
 * M200Sim.h
 *
 *  Created on: Nov 25, 2013
 *      Author: Alon Yaari
 */

#ifndef M200SIM_H_
#define M200SIM_H_

#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include "m200SimServer.h"

/*
ACTUAL VEHICLE PRODUCES:
$CPIMU,164958.858,-0.061035,-0.000097,-0.061035,-0.026916,0.104533,-0.932172*57
$GPRMC,164959.00,A,4221.71114,N,07105.44878,W,0.145,156.78,311214,,,A*70
$GPVTG,156.78,T,,M,0.145,N,0.269,K,A*3D
$GPGGA,164959.00,4221.71114,N,07105.44878,W,1,06,2.80,46.6,M,-33.2,M,,*5C
$GPGSA,A,3,29,02,26,05,13,15,,,,,,,4.45,2.80,3.46*02
$GPGSV,2,1,07,02,14,134,23,05,41,071,30,13,60,062,32,15,62,205,43*79
$GPGSV,2,2,07,26,69,075,37,29,38,227,42,30,14,045,23*44
$CPNVG,164959.034,4221.711142,N,7105.44878,W,1,0,0,234.32964,-6.796959,-1.706798,164959.006*78
$CPNVR,164959.063,0.02941,-0.068552,0,0.060938,0,-0.061132*59
$CPRCM,164959.065,0,234.326694,-1.655603,-6.628557,164959.060*62


THIS EMULATOR PRODUCES THIS SUBSET:
$CPNVG,150216.246,4221.516161,N,7105.25215,W,1,0,0,151.799032,0.507128,2.519,150216.191*75
$CPRBS,150218.154,15.091787,15.091787,15.091787,0*77
$GPGGA,150216.20,4221.51616,N,07105.25215,W,2,10,0.84,19.5,M,-33.2,M,,0000*53
$GPRMC,150216.20,A,4221.51616,N,07105.25215,W,0.021,0.00,260614,,,D*7C
*/


#define WATCHDOG_TIME 1.0

//#include "NMEAutcTime.h"
#include "CPIMUnmea.h"
#include "CPNVGnmea.h"
#include "CPNVRnmea.h"
#include "CPRCMnmea.h"

#include "GPGGAnmea.h"
#include "GPGSAnmea.h"
#include "GPGSVnmea.h"
#include "GPRMCnmea.h"
#include "GPVTGnmea.h"


#include "CPNVGnmea.h"
#include "CPRBSnmea.h"
#include "GPGGAnmea.h"
#include "GPRMCnmea.h"
#include "PYDEPnmea.h"
#include "PYDEVnmea.h"
#include "PYDIRnmea.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

enum thrustModes { THRUST_MODE_UNKNOWN, THRUST_MODE_NORMAL, THRUST_MODE_DIFFERENTIAL, THRUST_MODE_HEADING_SPEED };

#define MAX_BUFF       10000
#define LOWEST_BATT_V  13.4

class M200Sim : public AppCastingMOOSApp
{
public:
                    M200Sim();
    virtual         ~M200Sim() {};


protected:
    void            RegisterForMOOSMessages();
    bool            Iterate();
    bool            OnNewMail(MOOSMSG_LIST &NewMail);
    bool            OnConnectToServer();
    bool            OnStartUp();
    bool            ReadIncoming();
    bool            PublishToClient(std::string str);
    bool            DealWithNMEA(std::string nmea);

    bool            SetParam_PORT(double dVal);

    bool            PublishCPNVG();
    bool            PublishCPRBS();
    bool            PublishGPGGA();
    bool            PublishGPRMC();

    bool            ReceivePYDEV(std::string nmea);
    bool            ReceivePYDIR(std::string nmea);
    bool            ReceivePYDEP(std::string nmea);
    void            CommandThrust_LR(double left, double right);

    void            HandleServerAppCasts();
    bool            ParseCurrentTime();
    bool            buildReport();

    double          m_curSpeed;
    double          m_curHeading;
    double          m_curThrustL;
    double          m_curThrustR;
    double          m_curLat;
    double          m_curLon;

    int             m_curThrustMode;
    double          m_CPNVGlastTime;
    double          m_CPNVGdelay;
    double          m_CPRBSlastTime;
    double          m_CPRBSdelay;
    double          m_GPGGAlastTime;
    double          m_GPGGAdelay;
    double          m_GPRMClastTime;
    double          m_GPRMCdelay;

    utcTime         m_timeNow;
    double          m_battStartTime;      // Used to calculate a declining, asymptotic battery voltage

    double          m_LastPublishModeTime;
    double          m_LastConnectionTime;
    double          m_LastValidPYDIRtime;

    int             m_iPort;
    m200SimServer*  m_server;
    bool            m_bClientConnected;

    // Appcasting variables
    unsigned int    m_curClientRxNum;
    double          m_lastRxTime;
    unsigned int    m_curClientTxNum;
    double          m_lastTxTime;
};





#endif


















//

