/************************************************************/
/*    NAME: Michael "Misha" Novitzky                        */
/*    ORGN: MIT                                             */
/*    FILE: Health_KF.h                                     */
/*    DATE:                                                 */
/************************************************************/

#ifndef Health_KF_HEADER
#define Health_KF_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class Health_KF100 : public AppCastingMOOSApp
{
 public:
  Health_KF100();
  virtual ~Health_KF100();

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void RegisterVariables();
  bool buildReport();
  bool handleGPSTimeout(const std::string& value);
  bool handleGPSNumSat(const std::string& value);
  bool handleVoltageThreshold(const std::string& value);
  bool handleCurrentThreshold(const std::string& value);
  bool handleCurrentTimeout(const std::string& value);
  bool handleCompassTimeout(const std::string& value);
  bool handleiActuationKFTimeout(const std::string& value);

protected:
  // insert local vars here

  //GPS Variables
  bool gpsOn;
  double gpsThreshold;
  bool gpsEnoughSats;
  double gpsNumSatThreshold;
  
  //iActuationKFAC Variables
  bool iActuationKFACOn;
  double iActuationKFACOnTimerThreshold;
  double criticalVoltageThreshold;
  bool criticalVoltageTriggered;

  double criticalCurrentThreshold;
  double criticalCurrentTimerThreshold;
  double criticalCurrentTimer;
  bool criticalCurrentTimerStarted;

  //Compas Variables
  bool compassOn;
  double compassOnTimerThreshold;

  //AppCast Variables
  std::map<std::string,std::string>  m_AppCastString;
  std::map<std::string, double> m_ReceivedMessageTimestamp;
  std::map<std::string, double> m_AppCastDouble;
};

#endif 
