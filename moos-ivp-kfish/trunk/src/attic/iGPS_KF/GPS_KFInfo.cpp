

#include <cstdlib>
#include <iostream>
#include "GPSKF_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

void showSynopsis()
{
  blk("SYNOPSIS:                                                                   ");
  blk("------------------------------------                                        ");
  blk("  Connects with several models of GPS that are commonly deployed on the     ");
  blk("  Clearpath Kingfisher M100 and M200 vehicles. Supported units include those");
  blk("  from Garmin and uBlox, 1Hz through 5Hz.                                   ");
  blk("  NMEA messages can be received via serial port or MOOS message. Output     ");
  blk("  includes Lat, Lon, X, Y, northings, eastings, speed in meters per second, ");
  blk("  some quality indicators, and optionally, heading.                         ");
  blk("                                                                            ");
}

void showHelpAndExit()
{
  blk("                                                                            ");
  blu("============================================================================");
  blu("Usage: iGPS_KF file.moos [OPTIONS]                                          ");
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
  blu("iGPS_KF Example MOOS Configuration                                          ");
  blu("============================================================================");
  blk("                                                                            ");
  blk("// When connecting with a 5hz uBlox:                                        ");
  blk("ProcessConfig = iGPS_KF                                                     ");
  blk("{                                                                           ");
  blk("  AppTick    = 10                                                           ");
  blk("  CommsTick  = 10                                                           ");
  blk("                                                                            ");
  blk("  Port         = /dev/ttyACM1  // Fully-qualified path to the serial port   ");
  blk("  Streaming    = true          // Always set to TRUE                        ");
  blk("  Verbose      = false         // Verbosity of the serial connection        ");
  blk("  BaudRate     = 115200        // Serial port baud rate                     ");
  blk("  Type         = UBLOX         // UBLOX, GARMIN, or ASHTECH                 ");
  blk("  SHOW_CEP     = false         // If true, publishes VIEW_CIRCLE of CEP     ");
  blk("  SHOW_SUMMARY = false         // If true, publishes GPS output as one msg  ");
  blk("  RAW_GPS      = true          // If true, publishes raw msgs from GPS      ");
  blk("  PREFIX       = NAV_          // Prepends this to all GPS output messages  ");
  blk("  PUBLISH_HEADING = true       // If true, publishes _HEADING               ");
  blk("}                                                                           ");
  blk("                                                                            ");
  blk("// When receiving NMEA as MOOS messages:                                    ");
  blk("ProcessConfig = iGPS_KF                                                     ");
  blk("{                                                                           ");
  blk("  AppTick    = 10                                                           ");
  blk("  CommsTick  = 10                                                           ");
  blk("                                                                            ");
  blk("  NMEA_FROM_MSG = true         // When true, ignores serial port details    ");
  blk("  Type         = UBLOX         // UBLOX, GARMIN, or ASHTECH                 ");
  blk("  SHOW_CEP     = false         // If true, publishes VIEW_CIRCLE of CEP     ");
  blk("  SHOW_SUMMARY = false         // If true, publishes GPS output as one msg  ");
  blk("  RAW_GPS      = true          // If true, publishes raw msgs from GPS      ");
  blk("  PREFIX       = NAV_          // Prepends this to all GPS output messages  ");
  blk("  PUBLISH_HEADING = true       // If true, publishes _HEADING               ");
  blk("}                                                                           ");
  blk("                                                                            ");
  blk("                                                                            ");
  exit(0);
}

void showInterfaceAndExit()
{
  blk("                                                                            ");
  blu("============================================================================");
  blu("iGPS_KF INTERFACE                                                           ");
  blu("============================================================================");
  blk("                                                                            ");
  showSynopsis();
  blk("                                                                            ");
  blk("SUBSCRIPTIONS:                                                              ");
  blk("------------------------------------                                        ");
  blk("  NMEA_MSG   string    When NMEA_FROM_MSG=true, read NMEA messages from this");
  blk("                                                                            ");
  blk("PUBLICATIONS:                                                               ");
  blk("------------------------------------                                        ");
  blk("  [prefix]_LAT     double  Latitude parsed from recent NMEA sentence        ");
  blk("  [prefix]_LON     double  Longitude parsed from recent NMEA sentence       ");
  blk("  [prefix]_X       double  X position in meters relative to the local origin");
  blk("  [prefix]_Y       double  Y position in meters relative to the local origin");
  blk("  [prefix]_E       double  eastings in UTM coordinates                      ");
  blk("  [prefix]_N       double  northings in UTM coordinates                     ");
  blk("  [prefix]_SPEED   double  GPS-calculated speed in meters per second        ");
  blk("  [prefix]_HEADING double  If set to publish, GPS-calculated heading        ");
  blk("  [prefix]_SAT     double  Number of satellites GPS can make use of         ");
  blk("  [prefix]_HDOP    double  Horizontal dilution of precision value from GPS  ");
  blk("  [prefix]_QUALITY string  DIFF Differential fix (provides best position)   ");
  blk("                           NO_D Fix, but without differential input         ");
  blk("                           EST  Position is estimated                       ");
  blk("                           BAD  No position can be calculated               ");
  blk("  [prefix]_MAGVAR  double  If available, local compass magnetic variation   ");
  blk("  [prefix]_HPE     double  If available, horizontal position error          ");
  blk("  [prefix]_SUMMARY string  Optional single-message summary of GPS output    ");
  blk("  [prefix]_RAW     string  Received NMEA sentences                          ");
  blk("  [prefix]_WARNING string  Warning message due to NMEA parse issue          ");
  blk("  VIEW_CIRCLE      string  Optional visual indicator of position error      ");
  blk("                                                                            ");
  exit(0);
}

