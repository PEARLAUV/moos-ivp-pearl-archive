/*
 * m200SimServer.cpp
 *
 *  Created on: Jun 30, 2014
 *      Author: Alon Yaari
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include "MBUtils.h"
#include "m200SimServer.h"
#include "string.h"         // string error management

using namespace std;

bool m200SimServer::dispatch(void * pParam)
{
    m200SimServer* pMe = (m200SimServer*)pParam;
    return pMe->ServerLoop();
}

bool m200SimServer::ServerLoop()
{
    appCastMsgs acMsg (AC_EVENT, "Starting emulator server loop");
    m_appCasts.push_back(acMsg);
    if (!PrepareConnection()) {
        string error = "Could not open listening TCP port for emulator server on port ";
        error += intToString(m_port);
        error += ".";
        acMsg.define(AC_ERROR, error);
        m_appCasts.push_back(acMsg);
        return false; }
    acMsg.define(AC_EVENT, "Emulator is waiting for client to connect.");
    m_appCasts.push_back(acMsg);

    // ServerListen() blocks while waiting for a client connection
    //      - If the client drops out, return to the outer while loop to listen again
    //      - ServerListen() returns false when error on port
    while (ServerListen()) {
        acMsg.define(AC_EVENT, "Client has connected to the emulator.");
        m_appCasts.push_back(acMsg);

        // ReceiveLoop() Receives streaming data as it is available and deals with it
        //      - If client drops then the drop out of the inner loop
        //      - ReceiveLoop() returns false when client disconnects
        bool bGood = true;
        while (bGood) {
            bGood = ServerIn();
            MOOSPause(10); } }

    string error = "Emulator has TCP error listening on port ";
    error += intToString(m_port);
    error += ".";
    acMsg.define(AC_ERROR, error);
    m_appCasts.push_back(acMsg);
    return false;
}

m200SimServer::m200SimServer(int portID)
{
    m_port            = portID;
    m_socketfd        = 0;
    m_newSocketfd     = 0;
    m_cliLen          = 0;
    m_blankCount      = 0;
    m_fullBuff        = "";
    m_bGoodSocket     = false;
    m_bListening      = false;
    m_bAccepted       = false;
    m_inNMEA.clear();
}

bool m200SimServer::Run()
{
    appCastMsgs acMsg (AC_EVENT, "Emulator thread initiated.");
    m_appCasts.push_back(acMsg);
    m_serverThread.Initialise(dispatch, this);
    return m_serverThread.Start();
}

bool m200SimServer::IsListening()
{
    return m_bListening;
}

bool m200SimServer::ClientAttached()
{
    return m_bAccepted;
}

bool m200SimServer::PrepareConnection()
{
    appCastMsgs acMsg;

    // socket()
    m_socketfd = socket(AF_INET, SOCK_STREAM, 0);
    int tmp = errno;
    m_bGoodSocket = (m_socketfd >= 0);
    if (!m_bGoodSocket) {
        string error = "Error creating emulator TCP port. ";
        error += strerror(tmp);
        acMsg.define(AC_ERROR, error);
        m_appCasts.push_back(acMsg);
        return false; }
    string event = "Successfully created socket connection for the emulator on port ";
    event += intToString(m_port);
    event += ".";
    acMsg.define(AC_EVENT, event);
    m_appCasts.push_back(acMsg);

    bzero((char *) &m_serv_addr, sizeof(m_serv_addr));
    m_serv_addr.sin_family = AF_INET;
    m_serv_addr.sin_addr.s_addr = INADDR_ANY;
    m_serv_addr.sin_port = htons(m_port);

    // bind()
    m_bGoodSocket = (bind(m_socketfd, (sockaddr*) &m_serv_addr, sizeof(m_serv_addr)) >= 0);
    tmp = errno;
    if (!m_bGoodSocket) {
        string error = "Error binding emulator TCP socket. ";
        error += strerror(tmp);
        acMsg.define(AC_ERROR, error);
        m_appCasts.push_back(acMsg);
        return false; }
    return m_bGoodSocket;
}

bool m200SimServer::ServerListen()
{
    if (!m_bGoodSocket)
        return false;
    string event = "Emulator listening for connection on port ";
    event += intToString(m_port);
    event += ".";
    appCastMsgs acMsg (AC_EVENT, event);
    m_appCasts.push_back(acMsg);

    // listen()
    m_bListening = true;
    int heard = listen(m_socketfd, 2);
    int tmpListen = errno;
    if (heard) {
      string msg = "Error configuring listening on the TCP socket. ";
      msg += strerror(tmpListen);
      acMsg.define(AC_EVENT, msg);
      m_appCasts.push_back(acMsg); }
    else {
      string event = "Connection configured for listening.";
      appCastMsgs acMsg (AC_EVENT, event);
      m_appCasts.push_back(acMsg); }

    // accept()
    //      - BLOCKING CALL
    m_cliLen = sizeof(m_cli_addr);
    m_newSocketfd = accept(m_socketfd, (struct sockaddr*) &m_cli_addr, &m_cliLen);
    int tmp = errno;
    m_bAccepted = (m_newSocketfd >= 0);
    m_bListening = m_bAccepted;
    if (!m_bAccepted) {
        string error = "Error accepting client to emulator TCP socket. ";
        error += strerror(tmp);
        acMsg.define(AC_ERROR, error);
        m_appCasts.push_back(acMsg);
        return false; }
    else {
      string event = "Accepted connection on port ";
      event += intToString(m_port);
      event += ".";
      appCastMsgs acMsg (AC_EVENT, event);
      m_appCasts.push_back(acMsg); }
    return m_bAccepted;
}

bool m200SimServer::ServerIn()
{
    appCastMsgs acMsg;

    if (!m_bAccepted)
        return false;
    char incoming[BUF_SIZE];

    // read()
    ssize_t numBytes = read(m_newSocketfd, incoming, BUF_SIZE);
    if (numBytes < 0) {
        m_bAccepted = false;
        m_bListening = false;
        string error = "Error reading from TCP port. ";
        acMsg.define(AC_ERROR, error);
        m_appCasts.push_back(acMsg);
        return false; }
    if (strlen(m_inBuffer) + numBytes < MAX_BUF_SIZE)
        strncat(m_inBuffer, incoming, numBytes);
    string toParse = m_inBuffer;
    strcpy(m_inBuffer, "");   // Clear the persistent buffer

    // Everything before the first $ is lost; we're missing the beginning of that NMEA message
    //      - If no $ in the message then there isn't anything to parse
    //          so put it back into the persistent buffer to be concat'd to next time
    unsigned long int startNMEA = toParse.find('$');
    if (startNMEA == string::npos) {
        strcpy(m_inBuffer, toParse.c_str());
        return true; }
    if (startNMEA > 0)
        toParse = toParse.substr(startNMEA);

    // Parse string into a vector of lines
    //      - Split on any combination of "\r\n"
    for (int i = 0; i < (int) toParse.length(); i++)
        if (toParse.at(i) == '\r')
            toParse.at(i) = '\n';
    vector<string> lines = parseString(toParse, "\n");
    if (lines.size() < 1)
        return true;

    // If the last item is not a full sentence, keep it for next time
    string last = lines[lines.size() - 1];
    unsigned int lastSize = last.size();
    unsigned long int starPos = last.find('*');
    if (lastSize < 4 || starPos == string::npos)
        strcpy(m_inBuffer, last.c_str());

    // Cycle through the lines vector
    //      - Discard blank lines and non-NMEA lines
    //      - Valid NMEA lines append to the master sentence deque
    for (int i = 0; i < (int) lines.size(); i++) {
        string str = lines.at(i);
        if (str.length() > 1) {
            if (str.at(0) == '$') {
                starPos = str.find('*');
                if (starPos != string::npos)
                    m_inNMEA.push_front(str); } } }
    return true;
}

int m200SimServer::DataAvailable()
{
    return m_inNMEA.size();
}

string m200SimServer::GetNextSentence()
{
    string str = "";
    if (DataAvailable()) {
        str = m_inNMEA.back();
        m_inNMEA.pop_back(); }
    return str;
}

// SentToClient()
//      Sends a string over TCP to the attached client.
//      - Returns TRUE if successful or if blank string (which is not sent)
//      - Returns FALSE if error on writing to the TCP port
bool m200SimServer::SendToClient(string str)
{
    appCastMsgs acMsg;

    ssize_t numBytes = str.length();
    if (numBytes > 0) {

        // write()
        numBytes = write(m_newSocketfd, str.c_str(), numBytes);
        if (numBytes < 0) {
            m_bAccepted = false;
            m_bListening = false;
            string error = "Error writing to TCP socket. ";
            acMsg.define(AC_ERROR, error);
            m_appCasts.push_back(acMsg);
            return false; } }
    return true;
}

int m200SimServer::GetNextAppCastMsg(string& msg)
{
    if (m_appCasts.empty())
        return AC_NONE;
    msg = m_appCasts.at(0).msg;
    int type = m_appCasts.at(0).type;
    m_appCasts.pop_front();
    return type;
}


















//
