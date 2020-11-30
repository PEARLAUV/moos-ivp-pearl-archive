/************************************************************/
/*    NAME: Alon Yaari                                      */
/*    ORGN: MIT Dept. of Mechanical Engineering             */
/*    FILE: main.cpp                                        */
/*    DATE: March 22, 2011                                  */
/************************************************************/
// Modified version of iActuationKF originated by
// Hordur Johannsson and Mike Benjamin
 
#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "iNewKFApp.h"
#include "ReleaseInfo.h"
#include "iNewKF_ExampleConfig.h"

using namespace std;


int usage()
{
    blk("                                                                ");
    blk("=============================================================== ");
    blk("iGPS_KF                                                         ");
    blk("=============================================================== ");
    blk("                                                                ");
    blu(" USAGE                                                          ");
    blk("   iGPS_KF file.moos [OPTIONS]                                  ");
    blk("                                                                ");
    blu(" OPTIONS                                                        ");
    blk("   --alias=<ProcessName>                                        ");
    blk("           Launch with the given process name instead of iGPS_KF");
    blk("   --help, -h, -e, -i                                           ");
    blk("           This help message                                    ");
    blk("                                                                ");
    blu("  SUBSCRIBES                                                    ");
    blk("    None                                                        ");
    blk("                                                                ");
    blu("  PUBLISHES                                                     ");
    blk("    <prefix>_LAT     Latitude in decimal degrees                ");
    blk("    <prefix>_LON     Longitude in decimal degrees               ");
    blk("    <prefix>_X       Local cartesian plane X value in meters    ");
    blk("    <prefix>_Y       Local cartesian plane Y value in meters    ");
    blk("    <prefix>_N       UTM Northings                              ");
    blk("    <prefix>_E       UTM Eastings                               ");
    blk("    <prefix>_SPEED   Speed in meters per second                 ");
    blk("    <prefix>_Heading OPTIONAL Heading in degrees from true north");
    blk("    GPS_MAGVAR       Magnetic variation in degrees              ");
    blk("    GPS_SAT          Number of satellites contributing to fix   ");
    blk("    GPS_HDOP         Horiz. dilution of precision               ");
    blk("    GPS_MODE         Fix mode                                   ");
    blk("                        A=Autonomous, D=Diff., E=Est., N=Bad    ");
    blk("    GPS_QUALITY      Fix quality                                ");
    blk("                        0=No fix, 1=Non-diff, 2=Diff, 6=Est.    ");
    blk("                                                                ");
    blu("  MISSION FILE PARAMS                                           ");
    blk("    TYPE     str     GPS type (GARMIN, ASHTECH, UBLOX)          ");
    blk("    PREFIX   str     Prefix to published position messages      ");
    blk("                     Default:   GPS_                            ");
    blk("    SHOW_CEP bool    TRUE: Output CEP as a VIEW_CIRCLE          ");
    blk("                     FALSE: (def) No VIEW_CIRCLE output for CEP ");
    blk("    PUBLISH_HEADING bool TRUE: Publish <prefix>_HEADING         ");
    blk("                         FALSE: (def) Do not publish            ");
    blk("    RAW_GPS  bool    TRUE: Publish sentences directly from GPS  ");
    blk("                     FALSE: (def) Do not publish                ");
    blk("    PORT             Serial port name (no default)              ");
    blk("    BAUDRATE         Valid baud rates (def 9600)                ");
    blk("    HANDSHAKING      TRUE or FALSE (def) to use handshaking     ");
    blk("    VERBOSE          TRUE or FALSE (def) for verbose serial     ");
    blk("    STREAMING        TRUE or FALSE (def) for streaming mode     ");
    blk("                                                                ");
    return 0;
}


int main(int argc, char *argv[])
{
    int    i;
    string mission_file = "";
    string run_command  = argv[0];

    for(i=1; i<argc; i++) {
        string argi = argv[i];
        if ((argi=="-v") || (argi=="--version") || (argi=="-version")) {
            showReleaseInfo("iGPS_KF", "gpl");
            return 0; }
        else if ((argi=="-e") || (argi=="--example") || (argi=="-example"))
            return usage();
        else if ((argi == "--help")||(argi=="-h"))
            return usage();
        else if (argi == "-i")
            return usage();
        else if (strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
            mission_file = argv[i];
        else if (strBegins(argi, "--alias="))
            run_command = argi.substr(8); }

    if (mission_file == "")
            return usage();

    cout << termColor("green");
    cout << "iNewKF running as: " << run_command << endl;
    cout << termColor() << endl;

    NewKF newKF;

    newKF.Run(run_command.c_str(), mission_file.c_str());

    return(0);
    }




//
