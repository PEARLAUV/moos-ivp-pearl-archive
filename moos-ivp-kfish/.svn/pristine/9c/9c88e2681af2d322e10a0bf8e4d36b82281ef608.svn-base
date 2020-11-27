/*
 * minipacket.h
 *
 *  Created on: Jun 12, 2015
 *      Author: Alon Yaari
 */

#ifndef MINIPACKET_H_
#define MINIPACKET_H_

#define UCHAR unsigned char
enum MINI_PACKET_TYPES { TYPE_UNK, TYPE_OUT, TYPE_IN_PING, TYPE_IN_DATA, TYPE_IN_RANGE };

#include <stdint.h>
#include "NMEAparsing.h"

#define NO_DATA 8192u

// miniPacketOut   PingOut   | DataPacketOut
//      m_data               |      CCMUC
//      m_dst      CCMPC     |      CCMUC
//
// miniPacketIn    PingReply | DataPacketIn  | PingIn
//      m_data               |    CAMUA      |
//      m_dst       CAMPR    |               |
//      m_src                |    CAMUA      | CAMPA
//      m_range     CAMPR    |               |

class miniPacket {
public:
                  miniPacket();
  virtual         ~miniPacket() {}
  int             GetDstAsInt() { return (int) m_dst; }

protected:
  void            SetData(uint16_t data) { m_data = (data < NO_DATA) ? data : NO_DATA; }
  void            SetDst(UCHAR dst) { m_dst = (dst > 0u && dst < 128u) ? dst : 0u; }

  uint16_t        m_data;
  UCHAR           m_dst;
  int             m_type;
};

class minipacketOut : public miniPacket {
public:
                  minipacketOut();
                  minipacketOut(std::string msg);
                  minipacketOut(UCHAR dst, uint16_t data=0u);
                  ~minipacketOut() {}
    std::string   GenerateNMEAforModem(UCHAR src);

    std::string   toAppcast;
};

class miniPacketIn : public miniPacket  {
public:
                  miniPacketIn();
                  miniPacketIn(std::string nmea);
                  ~miniPacketIn() {}
    std::string   GenerateMOOSmsg();
    double        GetRange() { return m_range; }
    std::string   GetRangeNote();
    std::string   Get_VIEW_CIRCLE(double x, double y, std::string label);

    static double soundSpeed;

private:
    void          HandleCAMUA(); // Incoming data packet
    void          HandleCAMPR(); // Incoming range packet
    void          HandleCAMPA(); // Getting pinged

    void          SetSrc(UCHAR src) { m_src = (src > 0u && src < 128u) ? src : 0u; }
    void          SetTravelTime(double d);

    std::string   m_nmea;
    UCHAR         m_src;
    double        m_travelTime;
    double        m_range;
};













#endif


















//

