/*********************************************************************/
/* GPSInstrument.h                                                   */ 
/*                                                                   */
/*********************************************************************/

#include "NMEAMessage.h"
#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"

#include <iostream>
#include <math.h>

#define KNOTS2METERSperSEC 0.51444444

class CGPSInstrument : public CMOOSInstrument
{
 public:
  CGPSInstrument();
  virtual ~CGPSInstrument() {};
  
 protected:
  CMOOSGeodesy m_Geodesy;
  bool ParseNMEAString();
  bool InitialiseSensor();
  bool Iterate();
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool OnConnectToServer();
  bool OnStartUp();
  void GetData();
  bool HandleLatLon(std::string nmea, std::string lat, std::string latH, std::string lon, std::string lonH);
  bool GeodeticConversion();
  bool HandleGPSQuality(std::string qStr);
  bool HandleMagVar(std::string mvStr, std::string mvStrH);
  bool PublishData();
  bool InitNotifyNames();
  void ShowCEP();
  void AddWarning(std::string wStr);
  double DMS2DecDeg(double dfVal);

  std::string m_sType;
  std::string curNMEA;
  bool bShowCEP;
  bool bShowSummary;
  bool bRawGPS;
  bool bValidXY;
  bool bNMEAfromMsg;
  bool bHaveMagVarToPublish;
  int  initIsComplete;
  bool bPublishHeading;

  double curGPSTime;
  double lastGPSTime;
  double lastXYUpdateTime;
  std::string m_prefix;

  std::string nameSummary;
  std::string nameRaw;
  std::string nameLat;      double curLat;
  std::string nameLon;      double curLon;
  std::string nameLong;
  std::string nameX;        double curX;
  std::string nameY;        double curY;
  std::string nameN;        double curN;
  std::string nameE;        double curE;
  std::string nameSpeed;    double curSpeed;
  std::string nameHeading;  double curHeading;
  std::string nameSatNum;   unsigned int curSatNum;
  std::string nameHDOP;     double curHDOP;
  std::string nameQuality;  std::string curQuality;
  std::string nameMagVar;   double curMagVar;
  std::string nameHPE;      double curPosError;
  std::string nameWarning;  std::string curWarning;
};



/*
Some sample uBlox sentences:

0:  $GPGGA
1:  161755.00
2:  4221.51260
3:  N
4:  07105.25410
5:  W
6:  2
7:  11
8:  0.97
9:  0.3
10:  M
11:  -33.2
12:  M
13:
14:  0000*6A

0:  $GPRMC
1:  161755.00
2:  A
3:  4221.51260
4:  N
5:  07105.25410
6:  W
7:  0.008
8:
9:  050713
10:
11:
12:  D*6A

0:  $GPGGA
1:  161755.20
2:  4221.51260
3:  N
4:  07105.25410
5:  W
6:  2
7:  11
8:  0.97
9:  0.3
10:  M
11:  -33.2
12:  M
13:
14:  0000*68

*/













//
