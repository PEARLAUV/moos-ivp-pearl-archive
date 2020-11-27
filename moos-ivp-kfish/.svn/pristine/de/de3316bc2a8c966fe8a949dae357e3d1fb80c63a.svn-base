

#include <cstdlib>
#include <iostream>
#include "M200_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  Connects with Clearpath Robotics Kingfisher M200 via a TCP    ");
  blk("  port. Receives NMEA-style status messages from the vehcile    ");
  blk("  and manages transmision of NMEA-style commands to the vehicle.");
}

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: iM200 file.moos [OPTIONS]                                ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  blk("                                                                ");
  blk("Note: If argv[2] does not otherwise match a known option,       ");
  blk("      then it will be interpreted as a run alias. This is       ");
  blk("      to support pAntler launching conventions.                 ");
  blk("                                                                ");
  exit(0);
}

void showExampleConfigAndExit()
{
    // 0        1         2         3         4         5         6         7
    // 1234567890123456789012345678901234567890123456789012345678901234567890123456789
  blk("                                                                              ");
  blu("============================================================================= ");
  blu("iM200 Example MOOS Configuration                                              ");
  blu("============================================================================= ");
  blk("                                                                              ");
  blk("ProcessConfig = iM200                                                         ");
  blk("{                                                                             ");
  blk("  AppTick        = 10                                                         ");
  blk("  CommsTick      = 10                                                         ");
  blk("  PORT_NUMBER    = 29500                                                      ");
  blk("  IP_ADDRESS     = l // default is 'localhost'                                ");
  blk("  HEADING_SOURCE = RAW_COMPASS   // One of: RAW_COMPASS sourced from $CPRCM   ");
  blk("                                 //         NAV_UPDATE  sourced from $CPNVG   ");
  blk("                                 //         RAW_GPS     sourced from $GPRMC   ");
  blk("                                 //         NONE        don't extract heading ");
  blk("  HEADING_MSG_NAME   = NAV_HEADING                                            ");
  blk("  DIRECT_THRUST_MODE = false       // default is 'false'                      ");
  blk("  MAG_OFFSET          = 0.0        // RAW_COMPASS value will be offset by this");
  blk("}                                                                             ");
  blk("                                                                              ");
  exit(0);
}

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("iM200 INTERFACE                                                 ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  DESIRED_RUDDER   double   Value [-90.0, 90.0] to command the  ");
  blk("                            rudder angle. Centered = 0.0 and    ");
  blk("  DESRIRED_THRUST  double   Value [0.0, 100.0] giving percent   ");
  blk("                            thrust. 0 is stopped                ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  NAV_X and NAV_Y      double  X and Y values for position on   ");
  blk("                               the locally-defined grid         ");
  blk("  NAV_LAT and NAV_LONG double  Lat and long values in DD        ");
  blk("  NAV_HEADING          double  Vehicle heading in degrees from N");
  blk("  NAV_SPEED            double  Vehicle speed in m/s             ");
  blk("                                                                ");
  exit(0);
}

