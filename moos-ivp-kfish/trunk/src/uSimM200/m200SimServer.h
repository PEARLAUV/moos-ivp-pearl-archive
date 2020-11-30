/*
 * m200SimServer.h
 *
 *  Created on: Jun 30, 2014
 *      Author: yar
 */

#ifndef M200SIMSERVER_H_
#define M200SIMSERVER_H_

#include <deque>
#include <vector>
#include "MOOS/libMOOS/MOOSLib.h"

#define BUF_SIZE        1024
#define MAX_BUF_SIZE    16384

/*
 Creates a thread in which to launch the M200 simulated server.
 The server blocks when listening for an incoming connection.
 The main MOOS app won't block because the listener is running
 in this class as a separate thread.
 */

enum appCastType { AC_NONE, AC_ERROR, AC_WARNING, AC_EVENT, AC_UNKNOWN };

class appCastMsgs {
public:
    appCastMsgs() { type = AC_UNKNOWN, msg = ""; };
    ~appCastMsgs() {};
    appCastMsgs(int acType, std::string acMsg)
        { define(acType, acMsg); };
    void define (int acType, std::string acMsg)
        { type = acType; msg = acMsg; };
    int type;
    std::string msg;
};

class m200SimServer {
public:
                        m200SimServer(int portID=29500);
        virtual         ~m200SimServer() {};
        bool            Run();
        static bool     dispatch(void* param);

        void 			quit();
        bool            IsListening();
        bool            ClientAttached();
        int             DataAvailable();
        std::string     GetNextSentence();
        bool            SendToClient(std::string str);
        bool            ServerLoop();
        int             GetNextAppCastMsg(std::string& msg);

private:
        bool            ServerListen();
        bool            ServerIn();

        bool            PrepareConnection();
        static bool     thread_func(void *pThreadData);

        CMOOSThread     m_serverThread;

        unsigned short  m_blankCount;
        int             m_port;
        bool            m_bListening;
        bool            m_bAccepted;
        bool            m_bGoodSocket;
        int             m_socketfd;
        int             m_newSocketfd;
        socklen_t       m_cliLen;
        struct sockaddr_in m_serv_addr, m_cli_addr;
        char            m_inBuffer[MAX_BUF_SIZE];         // persistent char buffer
        std::string     m_fullBuff;
        std::deque<std::string> m_inNMEA;
        std::deque<appCastMsgs> m_appCasts;
};

#endif
















//
