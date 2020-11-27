
#include <string>
#include "M200.h"
#include "M200_Info.h"
#include "MBUtils.h"

using namespace std;

int main(int argc, char *argv[])
{
  string mission_file;
  string run_command = argv[0];

  for (int i = 1; i < argc; i++) {
    string argi = argv[i];
    if ((argi == "-e") || (argi == "--example") || (argi == "-example"))
      showExampleConfigAndExit();
    else if ((argi == "-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if ((argi == "-i") || (argi == "--interface"))
      showInterfaceAndExit();
    else if (strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if (strBegins(argi, "--alias="))
      run_command = argi.substr(8);
    else if (i == 2)
      run_command = argi; }

  if (mission_file == "")
    showHelpAndExit();

   iM200 test;
//   test.OpenConnectionTCP("192.168.1.165","29500");
//   test.OpenSocket();
//   test.Connect();
//   test.Send("$PYDEV,0,1.0*\r\n");
//   for(int i = 0; i <100 ;i++)
//{
//   test.Receive();
//}

   test.Run(run_command.c_str(),mission_file.c_str());
  //iM200 clap;
  //clap.Run(run_command.c_str(), mission_file.c_str());

  return 0;
}

