/*
 * GPSInstrument.h
 *
 *  Major Revision on: July 9, 2014
 *      Author: Alon Yaari
 */

#include <iostream>
#include <math.h>
#include <map>

#include "NMEAMessage.h"
#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"
#include "SerialComms.h"

//#include "NMEAutcTime.h"  // Uncomment if modifying this class to do something with NMEA time
#include "CPNVGnmea.h"
#include "CPRBSnmea.h"
#include "GPGGAnmea.h"
#include "GPGSTnmea.h"
#include "GPHDTnmea.h"
#include "GPRMCnmea.h"
#include "GPRMEnmea.h"
#include "PASHRnmea.h"
#include "PYDEPnmea.h"
#include "PYDEVnmea.h"
#include "PYDIRnmea.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

#define KNOTS2METERSperSEC 0.51444444

struct gpsPub {
    bool        isDouble;
    double        dVal;
    std::string    sVal;
};

class GPSHandler : public AppCastingMOOSApp
{
public:
    GPSHandler();
    virtual ~GPSHandler() {};
    bool    OnNewMail(MOOSMSG_LIST &NewMail);
    bool    Iterate();
    bool    OnConnectToServer();
    bool    OnStartUp();
    bool    buildReport();

protected:
    bool    RegisterForMOOSMessages();
    bool    ParseNMEAString(std::string nmea);
    bool    HandleTHRUST_MODE_REVERSE(std::string sVal);
    bool    SerialSetup();
    void    GetData();
    bool    GeodeticConversion();
    void    PrefixCleanup();
    void    ShowCEP();
    double  DMS2DecDeg(double dfVal);

    bool    HandleGPGGA(std::string nmea);
    bool    HandleGPGST(std::string nmea);
    bool    HandleGPHDT(std::string nmea);
    bool    HandleGPRMC(std::string nmea);
    bool    HandleGPRME(std::string nmea);
    bool    HandleGPTXT(std::string nmea);
    bool    HandlePASHR(std::string nmea);

    bool    SetParam_TYPE(std::string sVal);
    bool    SetParam_SHOW_CEP(std::string sVal);
    bool    SetParam_PREFIX(std::string sVal);
    bool    SetParam_PORT(std::string sVal);
    bool    SetParam_BAUDRATE(std::string sVal);
    bool    SetParam_TRIGGER_MSG(std::string sVal);
    bool    SetParam_PUBLISH_HEADING(std::string sVal);
    bool    SetParam_REPORT_NMEA(std::string sVal);
    bool    SetParam_HEADING_OFFSET(std::string sVal);
    bool    SetParam_SWITCH_PITCH_ROLL(std::string sVal);

    // Dealing with the publish queue
    bool    AddToPublishQueue(double dVal, std::string sMsgName);
    bool    AddToPublishQueue(std::string sVal, std::string sMsgName);
    bool    PublishTheQueue(std::string nmeaKey);

    CMOOSGeodesy    m_Geodesy;
    bool            bIsSerial;
    bool            bValid;
    bool            bShowCEP;
    bool            bValidXY;
    bool            bReportNMEA;
    bool            bNMEAfromMsg;
    bool            bHaveMagVarToPublish;
    bool            bPublishHeading;
    bool            bSwitchPitchRoll;
    bool			bReverseMode;

    double          curX;
    double          curY;
    double          curLat;
    double          curLon;
    double          curSpeed;
    double          curHeading;
    double          curPosError;
    double			curUTC;
    double          headingOffset;
    double          curRoll;
    double          curPitch;

    std::string     serialPort;
    int             baudRate;
    std::string     sBaudRate;

    std::string     m_prefix;

    std::string     triggerMsg;
    SerialComms*    serial;

    int             totalGPGGA;
    int             totalGPGST;
    int             totalGPHDT;
    int             totalGPRMC;
    int             totalGPRME;
    int             totalPASHR;
    int             totalBAD;
    int             nmeaMsgCounter;
    int				ignoreFirstMsgs;


    std::map<std::string, gpsPub>   toPublish;
};



/*
Some sample uBlox sentences:

0:  $GPGGA
1:  161755.00
2:  4221.51260
3:  N
4:  07105.25410
5:  W
6:  2
7:  11
8:  0.97
9:  0.3
10:  M
11:  -33.2
12:  M
13:
14:  0000*6A

0:  $GPRMC
1:  161755.00
2:  A
3:  4221.51260
4:  N
5:  07105.25410
6:  W
7:  0.008
8:
9:  050713
10:
11:
12:  D*6A

0:  $GPGGA
1:  161755.20
2:  4221.51260
3:  N
4:  07105.25410
5:  W
6:  2
7:  11
8:  0.97
9:  0.3
10:  M
11:  -33.2
12:  M
13:
14:  0000*68

*/













//







