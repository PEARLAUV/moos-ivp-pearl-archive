/*
 * minipacket.cpp
 *
 *  Created on: Jun 12, 2015
 *      Author: Alon Yaari
 */


#include <iostream>
#include "ColorPack.h"
#include "XYCircle.h"

// Outgoing messages
#include "../lib_NMEAParse/CCMPCnmea.h"   // Ping command, host to modem                      $CCMPC,1,2
#include "../lib_NMEAParse/CCMUCnmea.h"   // User Mini-Packet command, host to modem          $CCMUC,1,2,1FAB

// Incoming messages
#include "../lib_NMEAParse/CAMPAnmea.h"   // Ping has been received, modem to host            $CAMPA,1,2*5D
#include "../lib_NMEAParse/CAMPRnmea.h"   // Reply to Ping has been received, modem to host   $CAMPR,2,1,0.0101*7C
#include "../lib_NMEAParse/CAMUAnmea.h"   // Mini-Packet received acoustically, modem to host $CAMUA,1,2,1fab*20

#include "MBUtils.h"
#include "minipacket.h"

using namespace std;

miniPacket::miniPacket()
{
  SetDst(0u);
  SetData(NO_DATA);
  m_type = TYPE_UNK;
}

minipacketOut::minipacketOut()
{
  miniPacket();
  m_type = TYPE_OUT;
}

minipacketOut::minipacketOut(UCHAR dst, uint16_t data)
{
  minipacketOut();
  SetDst(dst);
  SetData(data);
}

minipacketOut::minipacketOut(string msg)
{
  minipacketOut();
  msg = toupper(msg);
  int dst = (int) tokDoubleParse(msg, "DST", ',', '=');
  SetDst(dst);
  if (strContains(msg, "DATA")) {
    int data = (int) tokDoubleParse(msg, "DATA", ',', '=');
    SetData(data); }
}

string minipacketOut::GenerateNMEAforModem(UCHAR src)
{
  string outStr = "";

  // No data to send so this is just a ping
  if (m_data == NO_DATA) {
    toAppcast += "CCMPC with src " + intToString((int) src);
    toAppcast += "  and dst " + intToString((int) m_dst);
    CCMPCnmea ccmpc;
    ccmpc.Set_srcAddress(src);
    ccmpc.Set_destAddress(m_dst);
    if (ccmpc.CriticalDataAreValid())
      ccmpc.ProduceNMEASentence(outStr); }

  // Data to send so this is a minipacket
  else {
    toAppcast = "CCMUC with src " + intToString((int) src);
    toAppcast += "  and dst " + intToString((int) m_dst);
    toAppcast += "  and data " + intToString((int) m_data);
    CCMUCnmea ccmuc;
    ccmuc.Set_srcAddress(src);
    ccmuc.Set_destAddress(m_dst);
    ccmuc.Set_msgData((unsigned short int) m_data);
    if (ccmuc.CriticalDataAreValid())
      ccmuc.ProduceNMEASentence(outStr); }

  toAppcast = outStr;
  return outStr;
}

miniPacketIn::miniPacketIn()
{
  miniPacket();
  SetSrc(0u);
  SetTravelTime(-1.0);
  m_range = -1.0;
}

miniPacketIn::miniPacketIn(string nmea)
{
  miniPacketIn();
  m_nmea = nmea;
  string key = NMEAbase::GetKeyFromSentence(m_nmea);
  if (key == "CAMPA")       HandleCAMPA();
  else if (key == "CAMPR")  HandleCAMPR();
  else if (key == "CAMUA")  HandleCAMUA();
}

// CAMUA: Incoming data packet      - Grab the dst ID and data
void miniPacketIn::HandleCAMUA()
{
  m_type = TYPE_IN_DATA;
  CAMUAnmea camua;
  camua.ParseSentenceIntoData(m_nmea, false);
  if (camua.CriticalDataAreValid()) {
    UCHAR uc = 0u;
    camua.Get_destAddress(uc);
    SetDst(uc);
    uint16_t u16;
    camua.Get_msgData(u16);
    SetData(u16); }
}

// CAMPR: Incoming range packet     - Grab the dst ID and range
void miniPacketIn::HandleCAMPR()
{
  m_type = TYPE_IN_RANGE;
  CAMPRnmea campr;
  campr.ParseSentenceIntoData(m_nmea, false);
  if (campr.CriticalDataAreValid()) {
    UCHAR uc = 0u;

    // in CAMPR, SRC is the unit that responded to the ping
    //     in other words, the destination of the CCMPC message
    campr.Get_srcAddress(uc);
    SetDst(uc);
    double d;
    campr.Get_travelTime(d);
    SetTravelTime(d); }
}

void miniPacketIn::SetTravelTime(double d)
{
  m_travelTime = (d < 0.0) ? -1.0 : d;
  m_range = (soundSpeed > 0.0) ? soundSpeed * m_travelTime : -1.0;
}

// CAMPA: Getting pinged            - Grab the src ID
 void miniPacketIn::HandleCAMPA()
{
   m_type = TYPE_IN_PING;
  UCHAR uc = 0u;
  CAMPAnmea campa;
  campa.ParseSentenceIntoData(m_nmea, false);
  if (campa.CriticalDataAreValid()) {
    UCHAR uc = 0u;
    campa.Get_srcAddress(uc);
    SetSrc(uc); }
}

string miniPacketIn::GenerateMOOSmsg()
{
  string moosMsg = "";
  switch (m_type) {
    case TYPE_IN_DATA:
      moosMsg += "type=data,src=";
      moosMsg += intToString((int) m_src);
      moosMsg += ",data=";
      moosMsg += intToString((int) m_data);
      break;
    case TYPE_IN_PING:
      moosMsg += "type=ping,src=";
      moosMsg += intToString((int) m_src);
      break;
    case TYPE_IN_RANGE:
      moosMsg += "type=range,dst=";
      moosMsg += intToString((int) m_dst);
      moosMsg += ",time=";
      moosMsg += doubleToString(m_travelTime, 1);
      if (soundSpeed) {
        moosMsg += ",range=";
        moosMsg += doubleToString(m_range); }
      break;
    default:
      moosMsg = "";
      break; }
  return moosMsg;
}

string miniPacketIn::GetRangeNote()
{
  string note = "";
  if (m_type == TYPE_IN_RANGE) {
    note = "Distance to modem #";
    note += intToString((int) m_dst);
    note += " is ";
    note += doubleToString(m_range, 1);
    note += "m"; }
  return note;
}

string miniPacketIn::Get_VIEW_CIRCLE(double x, double y, string label)
{
  string view = "";
  if (m_type == TYPE_IN_RANGE && m_range > 0.0) {
      //XYCircle circle(x, y, m_range);
      XYCircle circle(0, 0, m_range);
      circle.set_label(label);
      circle.set_active(true);
      view = circle.get_spec(); }
  return view;
}
























//
