/************************************************************/
/*    NAME: Michael "Misha" Novitzky                        */
/*    Modified From: Alon Yaari                             */
/*    ORGN: MIT                                             */
/*    FILE: Health_KF100Main.cpp                            */
/*    DATE:  June 2, 2013                                   */
/************************************************************/

#include <string>
#include "MBUtils.h"
#include "ColorParse.h"
#include "Health_KF100.h"
#include "ReleaseInfo.h"
#include "iHealth_KF100_ExampleConfig.h"

using namespace std;

int main(int argc, char *argv[])
{
  int i;
  string mission_file = "";
  string run_command = argv[0];
  bool help_requested = false;
  bool vers_requested = false;
  bool exam_requested = false;

  for(i=1; i<argc; i++)
  {
	string argi = argv[i];
	if((argi=="-v") || (argi=="--version") || (argi=="-version"))
		vers_requested = true;
	else if ((argi=="-e") || (argi == "--example") || (argi=="-example"))
		exam_requested = true;
	else if((argi=="--help") || (argi=="-h"))
		help_requested = true;
	else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
		mission_file = argv[i];
	else if(strBegins(argi,"--alias="))
		run_command = argi.substr(8);
  }
 
  if(vers_requested)
  {
	showReleaseInfo("iHealth_KF100","gpl");
	return(0);
  }

  if(exam_requested)
  {
	showExampleConfig();
	return(0);
  }
  
  if((mission_file == "") || help_requested)
  {
	cout<< "Usage: iHealth_KF100 file.moos [OPTIONS]                    "<< endl;
	cout<< "                                                            "<< endl;
	cout<< "Options:                                                    "<< endl;
	cout<< " --alias=<ProcesName>                                       "<< endl;
	cout<< "     Launch iHealth_KF100 with the given process            "<< endl;
	cout<< "     name rather than iHealth_KF100                         "<< endl;
        cout<< " --example, -e                                              "<< endl;
        cout<< "     Display example MOOS configuration block.              "<< endl;
        cout<< " --help, -h                                                 "<< endl;
        cout<< "     Display this help message.                             "<< endl;
        cout<< " --version, -v                                              "<< endl;
        cout<< "     Display the release version of iHealth_KF100.          "<< endl;
	return(0);            
  }

  cout << termColor("green");
  cout << "iHealth_KF100 running as: " << run_command << endl;
  cout << termColor() << endl;
  
  //make an application
  Health_KF100 Health_KF100App;

  //run it
  Health_KF100App.Run(run_command.c_str(), mission_file.c_str());
  
  return(0);
}

