/*
 * GPSInfo.cpp
 *
 *  Completely Revised on: July 9, 2014
 *      Author: Alon Yaari
 */

#include <cstdlib>
#include <iostream>
#include "GPSInfo.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

void showSynopsis()
{
  blk("SYNOPSIS:                                                                   ");
  blk("------------------------------------                                        ");
  blk("  Receives NMEA messages from serial GPS models or NMEA messages received   ");
  blk("  via MOOS message. NMEA sentences are parsed and data extracted is         ");
  blk("  published.                                                                ");
  blk("  This application assumes that a GPS unit connects via the serial port,    ");
  blk("  that the GPS was configured beforehand, and that the GPS retains the      ");
  blk("  configuration data when not powered.                                      ");
  blk("  This application should be able to connect with any GPS device that       ");
  blk("  streams properly-formed NMEA sentences via a serial port. It has been     ");
  blk("  shown to work with the following units, with NMEA turned on and any       ");
  blk("  proprietary binary formats disabled:                                      ");
  blk("       Garmin GPS18, GPS18x, GPS18x-5Hz                                     ");
  blk("       Ublox LEA6                                                           ");
  blk("       Trimble AgGPS                                                        ");
  blk("       Hemisphere Vector102                                                 ");
  blk("  If incoming NMEA sentences provide the source data, this application will ");
  blk("  output MOOS messages for these data (if data is in the incoming sentences:");
  blk("       lat, lon     Decimal degrees in the GPS unit's datum                 ");
  blk("       X, Y,        Relative to latOrigin, longOrigin                       ");
  blk("       speed        In meters per second                                    ");
  blk("       heading      Direction of travel in degrees clockwise from true north");
  blk("       yaw          Bow's direction in degrees clockwise from true north    ");
  blk("       pitch        Rotation along keel line, 0 = horiz, negative to port   ");
  blk("       roll         Rotation along beam, 0 = horiz, negative to rear        ");
  blk("       CEP          Error circle based centered on X,Y with radius of HDOP  ");
  blk("                                                                            ");
}

void showHelpAndExit()
{
  blk("                                                                            ");
  blu("============================================================================");
  blu("Usage: iGPS_KFAC file.moos [OPTIONS]                                        ");
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
  blu("iGPS_KFAC Example MOOS Configuration                                        ");
  blu("============================================================================");
  blk("                                                                            ");
  blk("ProcessConfig = iGPS_KFAC                                                   ");
  blk("{                                                                           ");
  blk("  AppTick    = 10                                                           ");
  blk("  CommsTick  = 10                                                           ");
  blk("                                                                            ");
  blk("  Type         = SERIAL        // Can be SERIAL or MOOS_MSG (default)       ");
  blk("  SHOW_CEP     = false         // If true, publishes VIEW_CIRCLE of CEP     ");
  blk("  PREFIX       = NAV_          // Prepends this to all GPS message names    ");
  blk("  REPORT_NMEA  = false         // Appcast report each incoming NMEA sentence");
  blk("  PUBLISH_HEADING = true       // If false, does not publish _HEADING       ");
  blk("  HEADING_OFFSET  = 0.0        // If publishing heading, this offset value  ");
  blk("                               //   will be added to heading when published.");
  blk("  SWITCH_PITCH_ROLL = false    // If true, swaps pitch and roll values.     ");
  blk("  TRIGGER_MSG  = GPGGA         // Accumulates data from all incoming        ");
  blk("                               // NMEA_MSGs but only publishes when the     ");
  blk("                               // trigger is received.                      ");
  blk("                               // No trigger when not defined, blank, or    ");
  blk("                               // set to 'NONE'                             ");
  blk("  // Options only for Type = SERIAL:                                        ");
  blk("  Port         = /dev/ttyACM1  // Fully-qualified path to the serial port   ");
  blk("  BaudRate     = 115200        // Serial port baud rate                     ");
  blk("}                                                                           ");
  blk("                                                                            ");
  exit(0);
}

void showInterfaceAndExit()
{
  blk("                                                                            ");
  blu("============================================================================");
  blu("iGPS_KFAC INTERFACE                                                         ");
  blu("============================================================================");
  blk("                                                                            ");
  showSynopsis();
  blk("                                                                            ");
  blk("SUBSCRIPTIONS:                                                              ");
  blk("------------------------------------                                        ");
  blk("  NMEA_MSG            string    When NMEA_FROM_MSG=true, read NMEA messages ");
  blk("                                as the GPS data source (instead of serial)  ");
  blk("  THRUST_MODE_REVERSE string    TRUE, add 180 degrees to the heading, output");
  blk("                                is constrained to [0,360).                  ");
  blk("                                FALSE, heading is published as normal.      ");
  blk("                                                                            ");
  blk("PUBLICATIONS:    (NOTE: Optional items are controlled by config params)     ");
  blk("------------------------------------                                        ");
  blk("  [prefix]_SUMMARY string  (Opt) Single-message summary of GPS output       ");
  blk("  VIEW_CIRCLE      string  (Opt)    visual indicator of position error      ");
  blk("  [prefix]_HEADING double  (Opt) GPS-provided heading in deg CW from true N ");
  blk("  [prefix]_LAT     double  Latitude parsed from recent NMEA sentence        ");
  blk("  [prefix]_LON     double  Longitude parsed from recent NMEA sentence       ");
  blk("  [prefix]_X       double  X position in meters relative to the local origin");
  blk("  [prefix]_Y       double  Y position in meters relative to the local origin");
  blk("  [prefix]_SPEED   double  Speed in meters per second, provided by GPS      ");
  blk("  [prefix]_YAW     double  Direction bow points in degrees CW from true N   ");
  blk("  [prefix]_PITCH   double  Degrees of tilt left or right                    ");
  blk("  [prefix]_ROLL    double  Degrees of tilt forward or back                  ");
  blk("  [prefix]_SAT     double  Number of satellites GPS can make use of         ");
  blk("  [prefix]_HDOP    double  Horizontal dilution of precision value from GPS  ");
  blk("  [prefix]_QUALITY string  DIFF Differential fix (provides best position)   ");
  blk("                           NO_D Fix, but without differential input         ");
  blk("                           EST  Position is estimated                       ");
  blk("                           BAD  No position can be calculated               ");
  blk("  [prefix]_MAGVAR  double  If available, local compass magnetic variation   ");
  blk("  [prefix]_HPE     double  If available, horizontal position error          ");
  blk("  [prefix]_RAW     string  Received NMEA sentences                          ");
  blk("                                                                            ");
  exit(0);
}

