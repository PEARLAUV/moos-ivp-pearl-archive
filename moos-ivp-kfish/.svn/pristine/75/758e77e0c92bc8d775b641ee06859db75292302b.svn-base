/*
 * uModemMiniPacket.h
 *
 *  Created on: Jun 11, 2015
 *      Author: Alon Yaari
 */

#ifndef UMODEMMINIPACKET_H_
#define UMODEMMINIPACKET_H_

#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "../lib_SimpleSerial/SerialComms.h"
#include "minipacket.h"

#define NOTREADYMSG "Cannot act on UMODEM_CMD until modem is ready."
enum umodemSetup { UMODEM_WAITING, UMODEM_SENT_CONFIG, UMODEM_READY };
enum packetType { INVALID_PACKET, PING_PACKET, MINI_PACKET };


class umodemMP : public AppCastingMOOSApp
{
public:
            umodemMP();
    virtual ~umodemMP() {};
    bool    OnNewMail(MOOSMSG_LIST &NewMail);
    bool    Iterate();
    bool    OnConnectToServer();
    bool    OnStartUp();
    void    HandleUmodemCommand(std::string cmd);
    void    ParseSentencesFromModem();
    bool    buildReport();

protected:
    bool    RegisterForMOOSMessages();
    void    HandleIncomingCAREV(std::string nmea);
    void    SendMessageToModem(std::string str);
    void    ModemSetup();

    bool    SerialSetup();
    bool    SetParam_PORT(std::string sVal);
    bool    SetParam_BAUDRATE(std::string sVal);
    bool    SetParam_SRC_ID(std::string sVal);
    bool    SetParam_SOUNDSPEED(std::string sVal);
    bool    SetParam_TIMEOUT(std::string sVal);
    bool    SetParam_SHOW_VIEW_CIRCLE(std::string sVal);

    std::string NiceString(std::string str);

    bool            m_bValidSerialConn;
    bool            m_bShowViewCircle;
    SerialComms*    m_serial;
    std::string     m_serial_port;
    int             m_baudrate;
    double          m_dSoundspeed;
    unsigned char   m_SRCid;
    std::string     m_revision;
    std::string     m_ident;
    std::string     m_timeOn;
    int             m_uModemState;
    std::string     m_LastRangeNote;

    double          m_X;
    double          m_Y;

    std::vector<packetType> sentPackets;
    std::map<std::string, int>    outPacketCount;
    std::map<std::string, int>    inPacketCount;
};

#endif


















//

