/*********************************************************************/
/* Clapboard.h                                                    */
/*                                                                   */
/*********************************************************************/

#include "MOOS/libMOOS/MOOSLib.h"

#include <iostream>

class Clapboard : public CMOOSInstrument
{
 public:
    Clapboard();
    virtual ~Clapboard() {};
  
 protected:
    bool Iterate();
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool OnConnectToServer();
    bool OnStartUp();
    bool CheckSerial();
    bool InitialiseSensor();

    bool bVerbose;
    std::string   outMsgName;
};
