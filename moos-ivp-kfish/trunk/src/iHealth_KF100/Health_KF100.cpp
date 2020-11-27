/************************************************************/
/*    NAME: Michael "Misha" Novitzky                        */
/*    ORGN: MIT                                             */
/*    FILE: Health_KF.cpp                                   */
/*    DATE:                                                 */
/************************************************************/

#include <list>
#include <iterator>
#include "MBUtils.h"
#include "Health_KF100.h"
#include "ACTable.h"

using namespace std;

//---------------------------------------------------------
// Constructor

Health_KF100::Health_KF100()
{
   //GPS Variable Initialization
   gpsOn = false;
   m_ReceivedMessageTimestamp["GPS"] = MOOSTime();
   gpsThreshold = 5.0; 
   m_AppCastString["GPS"] = "Off";
   reportRunWarning("GPS is Off!");
   gpsEnoughSats = true;
   gpsNumSatThreshold = 4;
   m_AppCastString["GPS_NUM_SAT"] = "0 Satellites";
   m_ReceivedMessageTimestamp["GPS_NUM_SAT"] = MOOSTime();

   //iActuation Variable Initialization
   iActuationKFACOn = false;
   m_ReceivedMessageTimestamp["iActuationKF"] = MOOSTime();
   iActuationKFACOnTimerThreshold = 5.0;
   m_AppCastString["iActuationKF"] = "Off";
   reportRunWarning("iActuationKF is Off!");
   
   criticalVoltageThreshold = 11.2;
   m_ReceivedMessageTimestamp["KF_VOLTAGE_AVG"] = MOOSTime();
   m_AppCastDouble["KF_VOLTAGE_AVG"] = 0.0;
   criticalVoltageTriggered = false;
   
   criticalCurrentThreshold = 5.0;
   m_ReceivedMessageTimestamp["KF_CURRENT_AVG"] = MOOSTime();
   m_AppCastDouble["KF_CURRENT_AVG"] = 0.0;
   criticalCurrentTimerStarted = false;
   criticalCurrentTimerThreshold = 10.0;
   criticalCurrentTimer = 0.0;

   compassOn = false;
   m_ReceivedMessageTimestamp["Compass"] = MOOSTime();
   compassOnTimerThreshold = 5.0;
   m_AppCastString["Compass"] = "Off";
   reportRunWarning("Compass is Off!");
   
}

//---------------------------------------------------------
// Destructor

Health_KF100::~Health_KF100()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Health_KF100::OnNewMail(MOOSMSG_LIST &NewMail)
{
   AppCastingMOOSApp::OnNewMail(NewMail);

   MOOSMSG_LIST::iterator p;
//MOOSTrace("\n OnNewMail Method: message size %i \n", NewMail.size());

   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;

	string moosvar		= msg.GetKey();
	string sval		= msg.GetString();
        double dval		= msg.GetDouble();
	string source		= msg.GetSource();
	string community	= msg.GetCommunity();
//MOOSTrace("\n Message: %s\n",moosvar.c_str());
	if(moosvar.find("GPS")!= std::string::npos) //means found substring
	{
		if(gpsOn == false) //reset gpsOn to true
		{
			gpsOn = true;
			m_AppCastString["GPS"] = "On";
			retractRunWarning("GPS is Off!"); 
		}
		m_ReceivedMessageTimestamp["GPS"] = MOOSTime(); //resets gpsOnTimer
	}//end if GPS hearbeat
//Check that the Compass is on
	if(moosvar.find("COMPASS")!=std::string::npos) //means found substring
	{
		if(compassOn == false) //rest compass to on/true
		{
			compassOn = true;
			m_AppCastString["Compass"] = "On";
			retractRunWarning("Compass is Off!");
		}
		m_ReceivedMessageTimestamp["Compass"] = MOOSTime(); //resets the Compass off timer
	}// end of compass heartbeat

	//Proper Number of GPS Satellites?
	if(moosvar == "GPS_SAT" || moosvar == "NAV_SAT")
	{
		m_ReceivedMessageTimestamp["GPS_NUM_SAT"] = MOOSTime();
		double gpsNumSats = dval;
		m_AppCastString["GPS_NUM_SAT"] = doubleToString(gpsNumSats) + " Satellites";
		if(gpsEnoughSats == false && gpsNumSats >= gpsNumSatThreshold) //we have enough satellites
		{
			gpsEnoughSats = true;
			retractRunWarning("GPS not enough satellites! Need " + doubleToString(gpsNumSatThreshold));
		}
		else if(gpsEnoughSats == true && gpsNumSats < gpsNumSatThreshold) //not enough satellites!!!
		{
			gpsEnoughSats = false;
			reportRunWarning("GPS not enough satellites! Need " + doubleToString(gpsNumSatThreshold)); 
		}
	}

//Check to see iActuationKF is active by what variables they post
	if(moosvar.find("KF_") != std::string::npos) //means found substring
	{
		if(iActuationKFACOn == false) //reset iActuationKFAC to On
		{
			iActuationKFACOn = true;
			m_AppCastString["iActuationKF"] = "On";
			retractRunWarning("iActuationKF is Off!");
		}
		m_ReceivedMessageTimestamp["iActuationKF"] = MOOSTime();
	} //end of iActuation Heartbeat check

//Check proper voltage levels
	if(moosvar == "KF_VOLTAGE_AVG")
	{
		m_ReceivedMessageTimestamp["KF_VOLTAGE_AVG"] = MOOSTime();
		m_AppCastDouble["KF_VOLTAGE_AVG"] = dval;
		if(!criticalVoltageTriggered && dval < this->criticalVoltageThreshold)
		{
			criticalVoltageTriggered = true;
			reportRunWarning("Low Voltage Warning");
		}
	}

//Get Average Current Values
	if(moosvar == "KF_CURRENT_AVG")
	{
		m_ReceivedMessageTimestamp["KF_CURRENT_AVG"] = MOOSTime();
		m_AppCastDouble["KF_CURRENT_AVG"] = dval;

		//check if Current Overdraw
		if( dval > this->criticalCurrentThreshold) //means it is above critical
		{
			//check if timer started
			if(!this->criticalCurrentTimerStarted) //Timer Not yet started
			{
				this->criticalCurrentTimerStarted = true;
				this->criticalCurrentTimer = MOOSTime();
			}
		}
		
	}
	
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Health_KF100::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool Health_KF100::Iterate()
{
//   double gpsThreshold = 5.0;
   AppCastingMOOSApp::Iterate();
//MOOSTrace("\n Iterate Method \n");
   // happens AppTick times per second

	double sinceGPSBeat = MOOSTime() - m_ReceivedMessageTimestamp["GPS"];
	if(gpsOn == true && sinceGPSBeat > gpsThreshold) //if gps on check if latest heartbeat within threshold
	{
		gpsOn = false;
		m_AppCastString["GPS"] = "Off";
		reportRunWarning("GPS is Off!");
	}

	double sinceCompassBeat = MOOSTime() - m_ReceivedMessageTimestamp["Compass"];
	if(compassOn && sinceCompassBeat > this->compassOnTimerThreshold) //mean compass heartbeat is old!
	{
		compassOn = false;
		m_AppCastString["Compass"] = "Off";
		reportRunWarning("Compass is Off!");
	}

	double sinceiActuationBeat = MOOSTime() - m_ReceivedMessageTimestamp["iActuationKF"];
	if(iActuationKFACOn == true && sinceiActuationBeat > iActuationKFACOnTimerThreshold) //if gps is on check if latest hearbeat is withing threshold
	{
		iActuationKFACOn = false;
		m_AppCastString["iActuationKF"] = "Off";
		reportRunWarning("iActuationKF is Off!");
	}

	//Check Current Critical Level and Timer
	if(this->criticalCurrentTimerStarted) //Critical Current Timer has started
	{
		double timeSinceCriticalCurrentStart = MOOSTime() - this->criticalCurrentTimer;
		if(timeSinceCriticalCurrentStart > this->criticalCurrentTimerThreshold) // trigger run warning
		{
			reportRunWarning("Current Overdraw Warning!!!");
			this->criticalCurrentTimerStarted = false;
		}
	}
   AppCastingMOOSApp:PostReport();
   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
// happens before connection is open

bool Health_KF100::OnStartUp()
{

  AppCastingMOOSApp::OnStartUp();

  list<string> sParams;                                                                                         
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {         
    list<string>::iterator p;                                           
    for(p=sParams.begin(); p!=sParams.end(); p++) {                 
      string original_line = *p;                                    
      string line = *p;                                    
      string param = stripBlankEnds(toupper(biteString(line, '=')));  
      string value = stripBlankEnds(line);

//follow convention that boolean handle will tell us if parameter is ok
//along with handle functions to take care of each Config Parameter      
      bool handled = false;
MOOSTrace("\n Param Name: %s \n",param.c_str());                                                     
      if(param == "GPSTIMEOUT") {                                      
		handled = handleGPSTimeout(value);                                               
      }                                                             
      else if(param == "GPSMINSATNUM") {                                  
		handled = handleGPSNumSat(value);                                     
      }         
	else if( param == "IACTUATIONKFTIMEOUT") {
		handled = handleiActuationKFTimeout(value);
	}
      else if(param == "CRITICALVOLTAGETHRESHOLD") {
		handled = handleVoltageThreshold(value);
	}
	else if( param == "CRITICALCURRENTTHRESHOLD") {
		handled = handleCurrentThreshold(value);
	}
	else if(param == "CRITICALCURRENTTIMEOUT") {
		handled = handleCurrentTimeout(value);
	}
	else if(param == "COMPASSTIMEOUT") {
		handled = handleCompassTimeout(value);
	}
	if(!handled) //param was not caught or handled properly
	{
		reportUnhandledConfigWarning(original_line);
	}
}
  } 

  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void Health_KF100::RegisterVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
  //GPS: Register for IGPS for health monitoring
  Register("GPS*","*",0);
  Register("KF_*","*",0);
  Register("COMPASS*","*",0);
}

//---------------------------------------------------------
// Procedure: buildReport

bool Health_KF100::buildReport()
{
//Display Configuration Parameters
//Using uFldNodeBroker as an example for AppCasting
  m_msgs << endl;
  m_msgs << "=============================================================" << endl;
  m_msgs << "  Configuration Parameters " << endl;
  m_msgs << "=============================================================" << endl;
  m_msgs << endl;
  m_msgs << "  GPS_TIMEOUT_THRESHOLD:             " << this->gpsThreshold << endl;
  m_msgs << "  GPS_NUM_SATS_REQUIRED:             " << this->gpsNumSatThreshold << endl;
  m_msgs << "  CRITICAL_VOLTAGE_THRESHOLD:        " << this->criticalVoltageThreshold << endl;
  m_msgs << "  CRITICAL_CURRENT_THRESHOLD:        " << this->criticalCurrentThreshold << endl;
  m_msgs << "  CRITICAL_CURRENT_TIMOUT_THRESHOLD: " << this->criticalCurrentTimerThreshold << endl;
  m_msgs << "  COMPASS_TIMEOUT_THRESHOLD:         " << this->compassOnTimerThreshold << endl;
  m_msgs << endl;
  m_msgs << endl;
//Display Runtime Information
  m_msgs << "=============================================================" <<  endl;
  m_msgs << "  Runtime Information " << endl;
  m_msgs << "=============================================================" << endl;
	ACTable actab(3);
	actab<<"Variable | Time | Value";
	actab.addHeaderLines();
	actab.setColumnMaxWidth(4,55);
	actab.setColumnNoPad(4);

//Pack Strings
	map<string,string>::iterator q;
	for(q = m_AppCastString.begin(); q!=m_AppCastString.end(); q ++)
	{
		string varname = q->first;
		string sval    = q->second;
		double timeSinceReceivedFromMOOSDB = MOOSTime() - m_ReceivedMessageTimestamp[varname];
	
		actab << varname << timeSinceReceivedFromMOOSDB << sval;
	}

//Pack Doubles
	map<string,double>::iterator w;
	for(w = m_AppCastDouble.begin(); w != m_AppCastDouble.end(); w++)
	{
		string varname = w->first;
		double dval    = w->second;
		double timeSinceReceivedFromMOOSDB = MOOSTime() - m_ReceivedMessageTimestamp[varname];

		actab << varname << timeSinceReceivedFromMOOSDB << dval;
	}

m_msgs << endl << endl;
m_msgs << actab.getFormattedString();

   return(true);
}

bool Health_KF100::handleGPSTimeout(const std::string& value)
{
	bool handled = true;

	double dval = atof(value.c_str());
	if(isNumber(value) && dval > 0.0)
	{
		//GPS Timeout Param is a number and greater than 0
		this->gpsThreshold = dval;
	}
	else
	{
		//GPS Timeout Param is not properly configured
		handled = false;
	}
	return(handled);
}

bool Health_KF100::handleGPSNumSat(const std::string& value)
{
	bool handled = true;

	double dval = atof(value.c_str());
	if(isNumber(value) && dval > 0.0)
	{
		//GPS Num Sats more than none
		this->gpsNumSatThreshold = dval;
	}
	else //GPS num sats threhold is invalid
	{
		handled = false;
	}

	return(handled);
}

bool Health_KF100::handleVoltageThreshold(const std::string& value)
{
	bool handled = true;

	double dval = atof(value.c_str());
	if(isNumber(value) && dval > 0.0)
	{
		//Critical Voltage Threshold is more than 0
		this->criticalVoltageThreshold = dval;
	}
	else //Critical Voltage Threshold is invalid
	{
		handled = false;
	}

	return(handled);
}

bool Health_KF100::handleCurrentThreshold(const std::string& value)
{
	bool handled = true;

	double dval = atof(value.c_str());
	if(isNumber(value) && dval > 0.0)
	{
		//Critical Current Threshold is more than 0
		this->criticalCurrentThreshold = dval;
	}
	else //Critical Current Threshold is invalid
	{
		handled = false;
	}
	
	return(handled);
}

bool Health_KF100::handleCurrentTimeout(const std::string& value)
{
	bool handled = true;

	double dval = atof(value.c_str());
	if(isNumber(value) && dval > 0.0)
	{
		//Critical Current Timeout is valid
		this->criticalCurrentTimerThreshold = dval;
	}
	else  //Critical Current Timout is invalid
	{
		handled = false;
	}

	return(handled);
}

bool Health_KF100::handleCompassTimeout(const std::string& value)
{
	bool handled = true;
	
	double dval = atof(value.c_str());
	if(isNumber(value) && dval >0.0)
	{
		//Compass Timeout Threshold is valid
		this->compassOnTimerThreshold = dval;
	}
	else //Timout Threshold is invlaid
	{
		handled = false;
	}

	return(handled);
}

bool Health_KF100::handleiActuationKFTimeout(const std::string& value)
{
	bool handled = true;

	double dval = atof(value.c_str());
	if(isNumber(value) && dval > 0.0)
	{
		//iActuation Timer Threshold is valid
		this->iActuationKFACOnTimerThreshold = dval;
	}
	else //Timeout Threshold is invalid
	{
		handled = false;
	}
	return(handled);
}
