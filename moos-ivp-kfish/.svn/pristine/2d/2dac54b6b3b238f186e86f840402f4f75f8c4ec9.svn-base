/*****************************************************************/
/*    NAME: Michael "Misha" Novitzky                             */
/*    MODIFIED FROM Below                                        */
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
#include "iHealth_KF100_ExampleConfig.h"
#include "ColorParse.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showExampleConfig

void showExampleConfig()
{
    blk("                                                                ");
    blk("=============================================================== ");
    blk("iHealth_KF100 Example MOOS Configuration                        ");
    blk("=============================================================== ");
    blu("Blue lines: ","Default configuration                            ");
    blk("                                                                ");
    blk("ProcessConfig = iHealth_KF100                                   ");
    blk("{                                                               ");
    blu("  AppTick   = 4    // Optimal setting is 10                     ");
    blu("  CommsTick = 4    // Optimal setting is 10                     ");
    blu("                                                                ");
    blu("  //Timeout before declaring GPS is Off                         ");
    blu("  GPSTimeout = 2.0                                              ");
    blu("                                                                ");
    blu("  //Minimum Number of GPS Satellites                            ");
    blu("  GPSMinSatNum = 6                                              ");
    blu("                                                                ");
    blu("  //Timout before declaring iActuationKF is off                 ");
    blu("  iActuationKFTimeout = 5.0                                     ");
    blu("                                                                ");
    blu("  //Warning if Voltage below Threshold                          ");
    blu("  CriticalVoltageThreshold = 11                                 ");
    blu("                                                                ");
    blu("  //Warning if Current is above Threshold for more than Tiemout ");
    blu("  CriticalCurrentThreshold = 2                                  ");
    blu("  CriticalCurrentTimout = 1                                     ");
    blk("                                                                ");
    blu("  //Timout before declaring Compass is off                      ");
    blu("  CompassTimout = 5.0                                           ");
    blk("}                                                               ");
    blk("                                                                ");
}
