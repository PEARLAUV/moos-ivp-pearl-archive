

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
  blk("  port. Receives NMEA-style status messages from the vehicle    ");
  blk("  and manages sending NMEA-style commands to the vehicle.       ");
  blk("                                                                ");
  blk("  NMEA SENTENCES FROM THE VEHICLE                               ");
  blk("  iM200 parses incoming messages relating to vehicle health,    ");
  blk("  GPS, and IMU.                                                 ");
  blk("                                                                ");
  blk("  NMEA SENTENCES TO THE VEHICLE                                 ");
  blk("  The only sentences sent to the vehicle are to thrust commands.");
  blk("  The M200 is actuated via differential drive. Values for       ");
  blk("  left-side-thrust and right-side-thrust are send via an NMEA   ");
  blk("  sentence.                                                     ");
//  blk("                                                                ");
//  blk("  Rudder-thrust Mode                                            ");
//  blk("  When DIFFERENTIAL_MODE==false (or is omitted from the mission ");
//  blk("  file), iM200 listens for DESIRED_HEADING and DESIRED_THRUST.  ");
//  blk("  Internally, these are converted into the left and right thrust");
//  blk("  values and pushed to the iM200 front seat.                    ");
//  blk("                                                                ");
//  blk("  Differential Mode (Direct-Thrust Mode)                        ");
//  blk("  When DIFFERENTIAL_MODE==true, iM200 listens for COMMANDED_L   ");
//  blk("  COMMANDED_R. These are pushed to the front seat without change");
//  blk("  to command the left and right thrust values, respectively.    ");
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
  blk("                                                                ");
  blu("================================================================");
  blu("iM200 Example MOOS Configuration                                ");
  blu("================================================================");
  blk("                                                                ");
  blk("ProcessConfig = iM200                                           ");
  blk("{                                                               ");
  blk("  AppTick        = 10                                           ");
  blk("  CommsTick      = 10                                           ");
  blk("                                                                ");
  blk("  port_number    = 29500                                        ");
  blk("  ip_address     = localhost     // Default is 'localhost'      ");
  blk("  heading_source = NAV_UPDATE    // RAW_COMPASS from $CPRCM     ");
  blk("                                 // NAV_UPDATE  from $CPNVG     ");
  blk("                                 // RAW_GPS     from $GPRMC     ");
  blk("                                 // NONE                        ");
  blk("                                                                ");
  blk("  heading_msg_name = NAV_HEADING                                ");
  blk("  thrust_mode      = direct-thrust  // or rudder-thrust,        ");
  blk("                                    // heading-speed            ");
  blk("  mag_offset       = 0.0            // NAV_HEADING offset       ");
  blk("}                                                               ");
  blk("                                                                ");
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
  blk("                                                                ");
//  blk("  // Relevant when not in DIFFERENTIAL_MODE                     ");
//  blk("  DESIRED_RUDDER  double Value [-90.0, 90.0] for rudder angle.  ");
//  blk("                         When not in diff mode. Centered = 0.0. ");
//  blk("  DESRIRED_THRUST double Value [0.0, 100.0] giving pct thrust.  ");
//  blk("                         When not in diff mode. No thrust = 0.0.");
//  blk("                                                                ");
//  blk("  // Relevant when DIFFERENTIAL_MODE==TRUE                      ");
  blk("  COMMANDED_L     double Value [-100.0, 100.0] to be commanded  ");
  blk("                         directly to the vehicle's left         ");
  blk("                         (portside) motor. No thrust = 0.0.     ");
  blk("  COMMANDED_R     double Value [-100.0, 100.0] to be commanded  ");
  blk("                         directly to the vehicle's right        ");
  blk("                         (starboard) motor. No thrust = 0.0.    ");
  blk("  KF_COMMANDED_L  double Deprecated equivalent to COMMANDED_L   ");
  blk("  KF_COMMANDED_R  double Deprecated equivalent to COMMANDED_R   ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  NAV_HEADING        double Vehicle heading in degrees from N   ");
  blk("  NMEA_MSG           double Sentences from the GPS              ");
  blk("  M200_BATT_VOLTAGE  double Battery voltage                     ");
  blk("                                                                ");
  exit(0);
}

