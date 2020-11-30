

#include <cstdlib>
#include <iostream>
#include "Clapboard_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  Listens for character 'X' on the serial stream to indicate    ");
  blk("  that a switch somewhere has been closed. Publishes message    ");
  blk("  with timestamp when event occurs.                             ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: iClapboard file.moos [OPTIONS]                        ");
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

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("iSerialSwtich Example MOOS Configuration                        ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = iClapboard                                   ");
  blk("{                                                               ");
  blk("  AppTick    = 10                                               ");
  blk("  CommsTick  = 10                                               ");
  blk("                                                                ");
  blk("  OUTPUT_MSG_NAME = TRIGGER                                     ");
  blk("  PORT            = /dev/serialport1                            ");
  blk("  STREAMING       = true                                        ");
  blk("  VERBOSE         = true                                        ");
  blk("  BAUDRATE        = 57600                                       ");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("pXRelay INTERFACE                                               ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  Whatever variable is specified by the INCOMING_VAR            ");
  blk("  configuration parameter.                                      ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  Whatever variable is specified by the OUTGOING_VAR            ");
  blk("  configuration parameter.                                      ");
  blk("                                                                ");
  exit(0);
}

