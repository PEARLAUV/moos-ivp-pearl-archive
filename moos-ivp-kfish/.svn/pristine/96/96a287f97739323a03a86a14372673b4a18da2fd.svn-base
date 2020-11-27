/************************************************************/
/*    NAME: Alon Yaari                                      */
/*    ORGN: MIT Dept. of Mechanical Engineering             */
/*    FILE: main.cpp                                        */
/*    DATE: March 22, 2011                                  */
/************************************************************/
// Modified version of iActuationKF originated by
// Hordur Johannsson and Mike Benjamin
 
#include <string>
#include "MBUtils.h"
#include "ColorParse.h"
#include "iActuationKFApp.h"
#include "ReleaseInfo.h"
#include "iActuationKF_ExampleConfig.h"

using namespace std;

int main(int argc, char *argv[])
{
    int    i;
    string mission_file = "";
    string run_command  = argv[0];
    bool   help_requested = false;
    bool   vers_requested = false;
    bool   exam_requested = false;

    for(i=1; i<argc; i++) {
        string argi = argv[i];
        if((argi=="-v") || (argi=="--version") || (argi=="-version"))
            vers_requested = true;
        else if((argi=="-e") || (argi=="--example") || (argi=="-example"))
            exam_requested = true;
        else if((argi == "--help")||(argi=="-h"))
            help_requested = true;
        else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
            mission_file = argv[i];
        else if(strBegins(argi, "--alias="))
            run_command = argi.substr(8);
    }

    if(vers_requested) {
        showReleaseInfo("iActuationKF", "gpl");
        return(0);
    }

    if(exam_requested) {
        showExampleConfig();
        return(0);
    }

    if((mission_file == "") || help_requested) {
        cout << "Usage: iActuationKF file.moos [OPTIONS]            " << endl;
        cout << "                                                    " << endl;
        cout << "Options:                                            " << endl;
        cout << "  --alias=<ProcessName>                             " << endl;
        cout << "      Launch iActuationKF with the given process   " << endl;
        cout << "      name rather than iActuationKF.               " << endl;
        cout << "  --example, -e                                     " << endl;
        cout << "      Display example MOOS configuration block.     " << endl;
        cout << "  --help, -h                                        " << endl;
        cout << "      Display this help message.                    " << endl;
        cout << "  --version,-v                                      " << endl;
        cout << "      Display the release version of iActuationKF. " << endl;
        return(0);
    }

    cout << termColor("green");
    cout << "iActuationKF running as: " << run_command << endl;
    cout << termColor() << endl;

    ActuateKF ActuateKF;

    ActuateKF.Run(run_command.c_str(), mission_file.c_str());

    return(0);
}

