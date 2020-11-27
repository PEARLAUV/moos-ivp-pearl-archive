/*
 * os5000.h
 *
 *  Major Revision on: July 22, 2014
 *      Author: Alon Yaari
 */

#include "MOOS/libMOOS/MOOSLib.h"
#include "SerialComms.h"

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

#define BAD_DOUBLE -99999.99

class os5000 : public AppCastingMOOSApp
{
public:
    os5000();
    virtual ~os5000() {};
    bool    OnNewMail(MOOSMSG_LIST &NewMail);
    bool    Iterate();
    bool    OnConnectToServer();
    bool    OnStartUp();
    bool    buildReport();

protected:
    bool    RegisterForMOOSMessages();
    bool    ParseNMEAString(std::string osData);
    bool    SerialSetup();
    void    GetData();
    void    PrefixCleanup();

    bool    SetParam_PREFIX(std::string sVal);
    bool    SetParam_PORT(std::string sVal);
    bool    SetParam_BAUDRATE(std::string sVal);
    bool    SetParam_MAGVAR(std::string sVal);
    std::string  ChecksumCalc(std::string sentence);

    bool            bValid;
    std::string     serialPort;
    int             baudRate;
    std::string     sBaudRate;
    std::string     m_prefix;
    SerialComms*    serial;
    int             msgCounter;
    double          magVar;
    double          curHeading;
    double          curPitch;
    double          curRoll;
    double          curTempC;

    std::string     nameHeading;
    std::string     namePitch;
    std::string     nameRoll;
    std::string     nameTempC;
};
















//







