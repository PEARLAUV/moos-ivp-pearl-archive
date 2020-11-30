/*
 * ujInfo.cpp
 *
 *  Created on: Jan 14 2015
 *      Author: Alon Yaari
 */

#include <cstdlib>
#include <iostream>
#include "ColorParse.h"
#include "ReleaseInfo.h"
#include "ujInfo.h"

using namespace std;

void showSynopsis()
{
  blk("SYNOPSIS:                                                                   ");
  blk("------------------------------------                                        ");
  blk("  Converts MOOS messages into a formatted JSON string. The JSON is saved to ");
  blk("  a text file. Data must be double type.                                    ");
  blk("  Example:         json = NAV_SPEED,5,/Users/student/data/navSpeed.json");
  blk("  Output:         {");
  blk("                    \"cols\": [");
  blk("                       {\"label\":\"DB_TIME\",\"type\":\"number\"},");
  blk("                       {\"label\":\"NAV_SPEED\",\"type\":\"number\"}");
  blk("                      ],");
  blk("                    \"rows\": [");
  blk("                       {\"c\":[{\"v\":\"1\",\"f\":null},{\"v\":\"2.1\",\"f\":null}]},");
  blk("                       {\"c\":[{\"v\":\"2\",\"f\":null},{\"v\":\"2.2\",\"f\":null}]},");
  blk("                       {\"c\":[{\"v\":\"3\",\"f\":null},{\"v\":\"2.3\",\"f\":null}]},");
  blk("                       {\"c\":[{\"v\":\"4\",\"f\":null},{\"v\":\"2.2\",\"f\":null}]},");
  blk("                       {\"c\":[{\"v\":\"5\",\"f\":null},{\"v\":\"2.2\",\"f\":null}]}");
  blk("                      ],");
  blk("                   }");
  blk("                                                                            ");
}

void showHelpAndExit()
{
  blk("                                                                            ");
  blu("============================================================================");
  blu("Usage: uJSON file.moos [OPTIONS]                                            ");
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
  blu("uJSON Example MOOS Configuration                                            ");
  blu("============================================================================");
  blk("                                                                            ");
  blk("ProcessConfig = uJSON                                                       ");
  blk("{                                                                           ");
  blk("  AppTick    = 10              // Ticks should be set to a value equal to or");
  blk("  CommsTick  = 10              // greater than the GPS output frequency     ");
  blk("                                                                            ");
  blk("  // Repeat json definition as many times as necessary.                     ");
  blk("  json = MOOS_MSG_NAME, number, filename                                    ");
  blk("  // MOOS_MSG_NAME     Name of the MOOS message providing source data.      ");
  blk("  //                   This will be used to label the JSON output.          ");
  blk("  // number            Number of historical elements to output.             ");
  blk("  // filename          Filename to store the JSON file, overwritten each    ");
  blk("  //                   iteration.                                           ");
  blk("}                                                                           ");
  blk("                                                                            ");
  exit(0);
}

void showInterfaceAndExit()
{
  blk("                                                                            ");
  blu("============================================================================");
  blu("iGPSDevice INTERFACE                                                        ");
  blu("============================================================================");
  blk("                                                                            ");
  showSynopsis();
  blk("                                                                            ");
  blk("SUBSCRIPTIONS:                                                              ");
  blk("------------------------------------                                        ");
  blk("<MOOS_MSG_NAME>    double     MOOS message of interest                      ");
  blk("                                                                            ");
  blk("PUBLICATIONS:    (NOTE: publication conditional on incoming nmea sentences) ");
  blk("------------------------------------                                        ");
  blk("  none                                                                      ");
  blk("                                                                            ");
  exit(0);
}
























//
