/*
 * uModemMiniPacketInfo.cpp
 *
 *  Created on: Jun 11, 2015
 *      Author: Alon Yaari
 */

#include <cstdlib>
#include <iostream>
#include "ColorParse.h"
#include "ReleaseInfo.h"
#include "uModemMiniPacketInfo.h"

using namespace std;

void showSynopsis()
{
  blk("SYNOPSIS:                                                                   ");
  blk("------------------------------------                                        ");
  blk("  This application is a lightweight implementation of WHOI Micromodem ping");
  blk("  and user mini-packet sentences. UMODEM_CMD requests are received and a");
  blk("  range ping or a mini-packet is sent accordingly. A range message is");
  blk("  generated on the success of a range ping. Otherwise, no indication is given");
  blk("  for the success of failure of a specific message. Counts are accumulated");
  blk("  and notified via appcast for overall send and receives.");
  blk("  Note that receiving a range result verifies that another modem is alive and");
  blk("  able to communicate.");
  blk("");
  blk("  PING:        Ping allows one uModem to verify that another");
  blk("               is available and responding. Ping also provides one-way travel");
  blk("               time in seconds, which can be translated into range distance.");
  blk("  MINI-PACKET: A variant of ping allows for up to 13 bits of data to be sent");
  blk("               to the destination modem. Travel time is not reported. Data");
  blk("               values are restricted to 0 to 8191, inclusive");
  blk("");
  blk("  Notes:       1. Packet transmissions are not time managed. Clashes can occur");
  blk("                  between transmissions from different sources.");
  blk("               2. Data to be sent over mini-packet is not encoded. The number");
  blk("                  provided as input is what is sent in the mini-packet.");
  blk("");
}

void showHelpAndExit()
{
  blk("                                                                            ");
  blu("============================================================================");
  blu("Usage: iuModemMP file.moos [OPTIONS]                                        ");
  blu("============================================================================");
  blk("                                                                            ");
  showSynopsis();
  blk("                                                                            ");
  blk("Options:                                                                    ");
  mag("  --example, -e                                                             ");
  blk("      Display example MOOS configuration block.                             ");
  mag("  --help, -h                                                                ");
  blk("      Display this help message.                                            ");
  mag("  --interface, -i                                                           ");
  blk("      Display MOOS publications and subscriptions.                          ");
  blk("                                                                            ");
  blk("Note: If argv[2] does not otherwise match a known option, then it will be   ");
  blk("      interpreted as a run alias. This is to support pAntler conventions.   ");
  blk("                                                                            ");
  exit(0);
}

void showExampleConfigAndExit()
{
//     0         1         2         3         4         5         6         7
//     01234567890123456789012345678901234567890123456789012345678901234567890123456789
  blk("                                                                            ");
  blu("============================================================================");
  blu("iuModemMP Example MOOS Configuration                                        ");
  blu("============================================================================");
  blk("                                                                            ");
  blk("ProcessConfig = iuModemMP                                                  ");
  blk("{                                                                           ");
  blk("  AppTick    = 5");
  blk("  CommsTick  = 5");
  blk("");
  blk("  Port             = /dev/ttyACM1  // Fully-qualified path to the serial port");
  blk("  BaudRate         = 19200         // Serial port baud rate");
  blk("  Src_ID           = 1             // uModem ID number (should be unique among");
  blk("                                   //   active modems");
  blk("  Soundspeed       = 1465          // Soundspeed in water, in meters per sec");
  blk("                                   //   1465 = fresh water, 15C");
  blk("  Show_View_Circle = TRUE          // True: publish VIEW_CIRCLE (def.= TRUE)");
  blk("                                   // False: do not publish");
  blk("}                                                                           ");
  blk("                                                                            ");
  exit(0);
}

void showInterfaceAndExit()
{
  blk("                                                                            ");
  blu("============================================================================");
  blu("iuModemMP INTERFACE                                                        ");
  blu("============================================================================");
  blk("                                                                            ");
  showSynopsis();
  blk("                                                                            ");
  blk("  UMODEM_CMD: incoming message that triggers a ping or mini-packet to be sent");
  blk("       dst   (required) Integer value 1 to 127, inclusive.");
  blk("               Modem ID of the destination modem.");
  blk("       data  (optional) Integer value 0 to 8191, inclusive.");
  blk("               If included, a mini-packet will be sent to dst with the value.");
  blk("               If not included, a ping will be sent to request range distance.");
  blk("");
  blk("  UMODEM_IN: received something from another modem");
  blk("       type  'data' or 'range' or 'ping'");
  blk("       src   Integer value 1 to 127, inclusive. (data and ping msg types)");
  blk("               Modem ID of the source of the message");
  blk("       dst   Integer value 1 to 127, inclusive. (range msg types)");
  blk("               Modem ID of the source of the message");
  blk("       data  Integer value 0 to 8191, inclusive. (data msg only)");
  blk("       range Positive double value. -1.0 = Bad checksum on send or recv (range msg only)");
  blk("               Estimated distance in meters, calculated as soundspeed x one-way-time.");
  blk("       time  Positive double value. -1.0 = Bad checksum on send or recv (range msg only)");
  blk("               Calculated time of travel, one-way between own modem and the dst modem");
  blk("");
  blk("SUBSCRIPTIONS:");
  blk("------------------------------------");
  blk("");
  blk("  UMODEM_CMD  dst=2             // Request range to modem #2");
  blk("  UMODEM_CMD  dst=12,data=1234  // Send mini-packet data 7654 to modem #3");
  blk("");
  blk("PUBLICATIONS:");
  blk("------------------------------------ ");
  blk("  VIEW_CIRCLE x=123.4,y=432.1,radius=98.76,duration=0,label=modem_2");
  blk("                                           // Circle centered on ownship with radius");
  blk("                                           //  range distance to destination modem.");
  blk("                                           //  Labeled with destination name. Overdrawn");
  blk("                                           //  with each successive range ping.");
  blk("  UMODEM_IN   type=range,dst=2,range=48.0  // Reply from modem #2 with range of 48.0m");
  blk("  UMODEM_IN   type=data,src=5,data=4567    // Incoming data from modem #5");
  blk("  UMODEM_IN   type=ping,src=5              // Pinged by modem #5");
  blk("");
  exit(0);
}


























//
