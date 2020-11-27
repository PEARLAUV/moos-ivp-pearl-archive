/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: KFC_ExampleConfig.cpp                                */
/*    DATE: Nov 10th 2011                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330, */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iostream>
#include "iNewKF_ExampleConfig.h"
#include "ColorParse.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showExampleConfig

void showExampleConfig()
{
    blk("                                                                ");
    blk("=============================================================== ");
    blk("iNewKF Example MOOS Configuration                               ");
    blk("=============================================================== ");
    blk(" INFO GOES HERE\n");
    return;

    blu("Blue lines: ","Default configuration                            ");
    blk("                                                                ");
    blk("ProcessConfig = iKFController                                   ");
    blk("{                                                               ");
    blu("  AppTick   = 4    // Optimal setting is 10                     ");
    blu("  CommsTick = 4    // Optimal setting is 10                     ");
    blk("                                                                ");
    blk("  Port = /dev/ttyUSB0                                           ");
    blu("  Timeout = 6                                                   ");
    blk("                                                                ");
    blk("  // If 1 then heading is computed from KF magnetometers        ");
    blu("  ComputeHeading = 0                                            ");
    blk("                                                                ");
    blk("  // Magnetometer offsets in Gauss                              ");
    blk("  MagOffsetX    = -0.0562                                       ");
    blk("  MagOffsetY    = -0.0342                                       ");
    blu("  MagOffsetZ    = 0.0                                           ");
    blk("                                                                ");
    blk("  // An orientation from compass to vehicle in degrees          ");
    blu("  HeadingOffset = 0.0                                           ");
    blk("                                                                ");
    blu("  ProcessOrientationData  = false                               ");
    blk("  // If true, publishes:  KF_HEADING                            ");
    blk("  //                      KF_YAW                                ");
    blk("  //                      KF_PITCH                              ");
    blk("  //                      KF_ROLL                               ");
    blk("                                                                ");
    blu("  ProcessRotationData     = false                               ");
    blk("  // If true, publishes   KF_ROT                                ");
    blk("  //                      KF_ROT_ROLL                           ");
    blk("  //                      KF_ROT_PITCH                          ");
    blk("  //                      KF_ROT_YAW                            ");
    blk("                                                                ");
    blu("  ProcessAccelerationData = false                               ");
    blk("  // If true, publishes   KF_ACC                                ");
    blk("  //                      KF_ACC_X                              ");
    blk("  //                      KF_ACC_Y                              ");
    blk("  //                      KF_ACC_Z                              ");
    blk("                                                                ");
    blk("  ProcessMagnetometerData = false                               ");
    blu("  // If true, publishes:  KF_MAG                                ");
    blk("  //                      KF_MAG_X                              ");
    blk("  //                      KF_MAG_Y                              ");
    blk("  //                      KF_MAG_Z                              ");
    blk("  //                      KF_YAW_COMPUTED                       ");
    blk("  //                      KF_HEADING_COMPUTED                   ");
    blk("  // (if ComputeHeading=1)KF_HEADING                            ");
    blk("                                                                ");
    blu("  ProcessSystemStatusData = true                                ");
    blk("}                                                               ");
    blk("                                                                ");
}
