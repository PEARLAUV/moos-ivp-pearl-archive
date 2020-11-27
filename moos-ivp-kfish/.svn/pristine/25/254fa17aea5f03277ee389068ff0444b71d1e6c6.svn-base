/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT Dept. of Mechanical Engineering             */
/*    FILE: main.cpp                                        */
/*    DATE: March 22, 2011                                  */
/************************************************************/

#include <string>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "KFC_MOOSApp.h"
#include "ReleaseInfo.h"
#include "KFC_ExampleConfig.h"

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
        showReleaseInfo("iKFController", "gpl");
        return(0);
    }

    if(exam_requested) {
        showExampleConfig();
        return(0);
    }

    if((mission_file == "") || help_requested) {
        cout << "Usage: iKFController file.moos [OPTIONS]            " << endl;
        cout << "                                                    " << endl;
        cout << "Options:                                            " << endl;
        cout << "  --alias=<ProcessName>                             " << endl;
        cout << "      Launch iKFController with the given process   " << endl;
        cout << "      name rather than iKFController.               " << endl;
        cout << "  --example, -e                                     " << endl;
        cout << "      Display example MOOS configuration block.     " << endl;
        cout << "  --help, -h                                        " << endl;
        cout << "      Display this help message.                    " << endl;
        cout << "  --version,-v                                      " << endl;
        cout << "      Display the release version of iKFController. " << endl;
        return(0);
    }

    cout << termColor("green");
    cout << "iKFController running as: " << run_command << endl;
    cout << termColor() << endl;

    KFC_MOOSApp kfc_moosapp;

    kfc_moosapp.Run(run_command.c_str(), mission_file.c_str());

    return(0);
}

