/************************************************************/
/*    NAME: Hordur Johannsson                               */
/*          Mike Benjamin - later modifications             */
/*    FILE: KFC_MOOSApp.cpp                                 */
/*    DATE: March 22, 2011                                  */
/************************************************************/

#include "KFC_MOOSApp.h"
#include "MBUtils.h"
#include <iostream>
#include <cmath>

using namespace std;

#ifndef M_PI
#define M_PI 3.14159
#endif

#ifdef WIN32
#define snprintf _snprintf
#endif


// Constrains v between minv and maxv
double clamp (double v, double minv, double maxv)
{
	return std::min (maxv, std::max (minv, v));
}


KFC_MOOSApp::KFC_MOOSApp() 
{
	// Initialize state variables
	m_thrustCommanded					= 0;
	m_thrustTime						= 0;
	m_rudderCommanded					= 0;
	m_rudderTime						= 0;
	m_magdec							= 0;
	m_magdec_set						= false;
	m_orientation_received				= false;
	m_roll								= 0;
	m_pitch								= 0;
	m_yaw								= 0;
	m_mag_received						= false;

	// Initialize configuration variables
	for (int i = 0; i < 3; ++i) {
		m_mag_offsets[i]				= 0.0;
		m_mag_xyz[i]					= 0.0; }
	m_heading_offset					= 0;
	m_compute_heading					= false;
	m_verbose							= false;
	m_timeout							= 6;		// seconds
	m_process_orientation_data			= false;
	m_process_rotation_data				= false;
	m_process_acceleration_data			= false;
	m_process_magnetometer_data			= false;
	m_process_systemstatus_data			= true;
	m_new_style_command					= false;
	m_max_rudder_diff					= 50;
	msgNameTHRUST						= "DESIRED_THRUST";
	msgNameRUDDER						= "DESIRED_RUDDER";
}


bool KFC_MOOSApp::OnNewMail (MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator		p;
	for (p = NewMail.begin(); p != NewMail.end(); ++p) {
		CMOOSMsg				&rMsg			= *p;
		if (MOOSStrCmp (rMsg.GetKey(), msgNameTHRUST)) {
			m_thrustCommanded					= rMsg.GetDouble();
			m_thrustTime						= rMsg.GetTime(); }
		else if (MOOSStrCmp (rMsg.GetKey(), msgNameRUDDER)) {
			m_rudderCommanded					= rMsg.GetDouble();
			m_rudderTime						= rMsg.GetTime(); }
		else if (MOOSStrCmp (rMsg.GetKey(), "GPS_MAGNETIC_DECLINATION")) {
			double magdec						= rMsg.GetDouble();
			if (m_magdec != magdec) {
				MOOSTrace						("Setting magnetic declination to %lf\n", magdec);
				m_magdec						= magdec;
				m_magdec_set					= true; } } }
	return true;
}


bool KFC_MOOSApp::OnConnectToServer()
{
	m_MissionReader.GetConfigurationParam		("Port",					m_port);
	m_MissionReader.GetConfigurationParam		("Timeout",					m_timeout);
	m_MissionReader.GetConfigurationParam		("MagOffsetX",				m_mag_offsets[0]);
	m_MissionReader.GetConfigurationParam		("MagOffsetY",				m_mag_offsets[1]);
	m_MissionReader.GetConfigurationParam		("MagOffsetZ",				m_mag_offsets[2]);
	m_MissionReader.GetConfigurationParam		("m_verbose",				m_verbose);
	m_MissionReader.GetConfigurationParam		("HeadingOffset",			m_heading_offset);
	m_MissionReader.GetConfigurationParam		("ComputeHeading",			m_compute_heading);
	m_MissionReader.GetConfigurationParam		("ProcessOrientationData",	m_process_orientation_data);
	m_MissionReader.GetConfigurationParam		("ProcessRotationData",		m_process_rotation_data);
	m_MissionReader.GetConfigurationParam		("ProcessAccelerationData",	m_process_acceleration_data);
	m_MissionReader.GetConfigurationParam		("ProcessMagnetometerData",	m_process_magnetometer_data);
	m_MissionReader.GetConfigurationParam		("ProcessSystemStatusData",	m_process_systemstatus_data);
	m_MissionReader.GetConfigurationParam		("NewStyleCommand",			m_new_style_command);
	m_MissionReader.GetConfigurationParam		("MaxRudderDiff",			m_max_rudder_diff);
  
	int			max_retry						= 3;
	clearpath::Transport::instance().configure	(m_port.c_str(), max_retry);
	// @todo  see if we should have validation on timeout                                                     

	ShowPlatformInfo();
	if (m_process_orientation_data) {
		if (m_verbose)							MOOSTrace ("Subscribing orientation\n");
		clearpath::DataPlatformOrientation::subscribe	(20); }
  
	if (m_process_magnetometer_data) {
		if (m_verbose)							MOOSTrace ("Subscribing magn\n");
		clearpath::DataPlatformMagnetometer::subscribe	(20); }

	#if 0
	if (m_process_rotation_data) {
		if (m_verbose)							MOOSTrace ("Subscribing rotation\n");
		clearpath::DataPlatformRotation::subscribe		(20); }
	#endif

	#if 0
	if (m_process_acceleration_data) {
		if (m_verbose)							MOOSTrace ("Subscribing acc\n");
	clearpath::DataPlatformAcceleration::subscribe		(20); }
	#endif

	if (m_process_systemstatus_data) {
		if (m_verbose)							MOOSTrace ("Subscribing sstatus\n");
	clearpath::DataSystemStatus::subscribe				(5); }

	std::string					sVal;
	if (m_MissionReader.GetValue ("THRUST_MESSAGE", sVal))
		msgNameTHRUST							= sVal;
	if (m_verbose)
		MOOSTrace								("THRUST_MESSAGE = %s", msgNameTHRUST.c_str());
	if (m_MissionReader.GetValue ("RUDDER_MESSAGE", sVal))
		msgNameRUDDER							= sVal;
	if (m_verbose)
		MOOSTrace								("RUDDER_MESSAGE = %s", msgNameRUDDER.c_str());

	DoRegistrations();
	return true;
}


template<class Data> Data* KFC_MOOSApp::GetLatestData()
{
	Data*			data						= Data::popNext();
	if (data) {
		Data*		next						= Data::popNext();
		while (next) {
			delete								(data);
			data								= next;
			next								= Data::popNext(); } }
	return data;
}


void KFC_MOOSApp::ProcessOrientationData()
{
	clearpath::DataPlatformOrientation*	orientation		= 0;
	orientation											= GetLatestData<clearpath::DataPlatformOrientation>();
	if (orientation) {
		m_orientation_received							= true;  
		if (!m_compute_heading) {
			double		hdg								= orientation->getYaw() / M_PI * 180.0 + m_magdec;
			m_Comms.Notify								("KF_HEADING", hdg); }
		m_roll											= orientation->getRoll();
		m_pitch											= orientation->getPitch();
		m_yaw											= orientation->getYaw();
   		m_Comms.Notify									("KF_YAW",		m_yaw);
		m_Comms.Notify									("KF_PITCH",	m_pitch / M_PI * 180.0);
		m_Comms.Notify									("KF_ROLL",	m_roll  / M_PI * 180.0);
		delete (orientation); }
}


void KFC_MOOSApp::ProcessMagnetometerData()
{
	if (!m_magdec_set) {
		string					warning					= "Magnetic declination has not been set.";
		m_Comms.Notify									("KF_COMPASS_WARNING", warning); }

	clearpath::DataPlatformMagnetometer*	mag			= 0;
	mag													= GetLatestData<clearpath::DataPlatformMagnetometer>();
	if (mag) {
		m_mag_received									= true;
		char msg[255];
		double				x							= mag->getX() + m_mag_offsets[0];
		double				y							= mag->getY() + m_mag_offsets[1];
		double				z							= mag->getZ() + m_mag_offsets[2];
		m_mag_xyz[0]									= x;
		m_mag_xyz[1]									= y;
		m_mag_xyz[2]									= z;
		snprintf										(msg, 255, "x=%lf,y=%lf,z=%lf", x, y, z);
		m_Comms.Notify									("KF_MAG",		msg);
		m_Comms.Notify									("KF_MAG_X",	x);
		m_Comms.Notify									("KF_MAG_Y",	y);
		m_Comms.Notify									("KF_MAG_Z",	z);

		// @todo  Rotate by roll/pitch

		double				yaw							= -atan2 (y, x) + m_heading_offset / 180.0 * M_PI;
		m_Comms.Notify									("KF_YAW_COMPUTED",		yaw);
		m_Comms.Notify									("KF_HEADING_COMPUTED",	yaw / M_PI * 180.0 + m_magdec);
		if (m_compute_heading)
			m_Comms.Notify								("KF_HEADING",				yaw / M_PI * 180.0 + m_magdec);
		delete (mag); }
}


void KFC_MOOSApp::ProcessRotationData()
{
	clearpath::DataPlatformRotation*	rot;
	rot													= GetLatestData<clearpath::DataPlatformRotation>();
	if (rot) {
		char							msg[255];
		double							roll_rate		= rot->getRollRate();
		double							pitch_rate		= rot->getPitchRate();
		double							yaw_rate		= rot->getYawRate();
		snprintf										(msg, 255, "x=%lf,y=%lf,z=%lf", roll_rate, pitch_rate, yaw_rate);
		m_Comms.Notify									("KF_ROT",			msg);
		m_Comms.Notify									("KF_ROT_ROLL",	roll_rate);
		m_Comms.Notify									("KF_ROT_PITCH",	pitch_rate);
		m_Comms.Notify									("KF_ROT_YAW",		yaw_rate);
	delete (rot); }
}


void KFC_MOOSApp::ProcessAccelerationData()
{
	clearpath::DataPlatformAcceleration*	acc;
	acc													= GetLatestData<clearpath::DataPlatformAcceleration>();
	if (acc) {
		char								msg[255];
		double								x			= acc->getX();
		double								y			= acc->getY();
		double								z			= acc->getZ();
		snprintf										(msg, 255, "x=%lf,y=%lf,z=%lf", x, y, z);
		m_Comms.Notify									("KF_ACC",			msg);
		m_Comms.Notify									("KF_ACC_X",		x);
		m_Comms.Notify									("KF_ACC_Y",		y);
		m_Comms.Notify									("KF_ACC_Z",		z);
		delete (acc); }
}


void KFC_MOOSApp::ProcessSystemStatusData()
{
	clearpath::DataSystemStatus*			status;
	status												= GetLatestData<clearpath::DataSystemStatus>();
	if (status) {
		unsigned int						v_count		= status->getVoltagesCount();
		unsigned int						c_count		= status->getCurrentsCount();
		double								totalV		= 0;
		for (unsigned int i = 0; i < v_count; i++) {
			double							voltage		= status->getVoltage (i);
			string							voltage_str	= uintToString (i) + "," + doubleToString (voltage);
			m_Comms.Notify								("KF_VOLTAGE_" + uintToString (i),		voltage);
			m_Comms.Notify								("KF_VOLTAGE_STR",						voltage_str);
			totalV										+= voltage; }
		m_Comms.Notify									("KF_VOLTAGE_AVG",						totalV / (double) (v_count));

		double								totalI		= 0;
		for (unsigned int i = 0; i < c_count; i++) {
			double							current		= status->getCurrent (i);
			string							current_str	= uintToString (i) + "," + doubleToString (current);
			m_Comms.Notify								("KF_CURRENT",							current);
			m_Comms.Notify								("KF_CURRENT_" + uintToString (i),		current);
			m_Comms.Notify								("KF_CURRENT_STR",						current_str);
			totalI										+= current; }
		m_Comms.Notify									("KF_CURRENT_AVG",						totalI / (double) (c_count));
		delete (status); }
}


void KFC_MOOSApp::NewStyleCommand (double &left, double &right)
{
	int										L			= m_thrustCommanded + m_rudderCommanded;
	int										R			= m_thrustCommanded - m_rudderCommanded;
	if (L > 100)							R			-= (L - 100);
	if (L < -100)							R			-= (L + 100);
	if (R > 100)							L			-= (R - 100);
	if (R < -100)							L			-= (R + 100);
	left												= clamp (L, -100.0, 100.0);
	right												= clamp (R, -100.0, 100.0);
	HandleMaxRudder										(left, right);
}


//left = 150   
//right = -60

void KFC_MOOSApp::HandleMaxRudder (double &left, double &right)
{
	int   diff = abs ((int) left - (int) right);
	if (diff > m_max_rudder_diff) {
	  
	  if (left > right)					
	    right  = left - 50;
	  else								
	    left = right - 50; }
}

bool KFC_MOOSApp::Iterate()
{
	double									t			= MOOSTime();

	// @todo cleanup debug output
	// @todo add support for other control modes
	//         -- SetVelocity
	//         -- SetTurn
	//         -- Setting differential directly

	if (((t - m_thrustTime) < m_timeout) && ((t - m_rudderTime) < m_timeout)) {
		double								left		= 0.0;
		double								right		= 0.0;
		if (m_new_style_command)
			NewStyleCommand (left, right);
		else {
			left										= clamp (m_thrustCommanded + m_rudderCommanded, -100.0, 100.0);
			right										= clamp (m_thrustCommanded - m_rudderCommanded, -100.0, 100.0); }
		clearpath::SetDifferentialOutput				(left, right).send();
		m_Comms.Notify									("KF_COMMANDED_L", left);
		m_Comms.Notify									("KF_COMMANDED_R", right);
		if (m_verbose)
			MOOSTrace									("Sent to Kingfisher:\tleft: %3.1f \tright: %3.1f\t%s\n", left, right,
														(m_new_style_command ? "using new style command" : "")); }
	else // timeout
		clearpath::SetDifferentialOutput				(0, 0).send();

	m_mag_received										= false;
	m_orientation_received								= false;  
	if (m_process_orientation_data) {
		if (m_verbose)
			MOOSTrace									("Processing orientation data\n");
		ProcessOrientationData(); }

	#if 0
	if (m_process_rotation_data) {
		if (m_verbose)
			MOOSTrace									("Processing rotation data\n");
		ProcessRotationData(); }
	#endif

	#if 0
	if (m_process_acceleration_data) {
		if (m_verbose)
			MOOSTrace									("Processing acceleration data\n");
			ProcessAccelerationData(); }
	#endif

	if (m_process_magnetometer_data) {
		if (m_verbose)
			MOOSTrace									("Processing magnetometer data\n");
		ProcessMagnetometerData(); }

	if (m_process_systemstatus_data) {
		if (m_verbose)
			MOOSTrace									("Processing system status data\n");
		ProcessSystemStatusData(); }

	if (m_verbose)
		MOOSTrace										("Done Processing data\n");

	char msg[512];
	snprintf											(msg, sizeof (msg), "%lf,%lf,%lf,%d,%lf,%lf,%lf,%d",
															m_roll, m_pitch, m_yaw, m_orientation_received, m_mag_xyz[0], m_mag_xyz[1], m_mag_xyz[2], m_mag_received);
	m_Comms.Notify										("KF_ORIENTATION_DATA", msg);
  
	return true;
}


bool KFC_MOOSApp::OnStartUp()
{
	DoRegistrations();
	return true;
}


void KFC_MOOSApp::DoRegistrations()
{
	m_Comms.Register									(msgNameTHRUST,	0);
	m_Comms.Register									(msgNameRUDDER,	0);
}

//---------------------------------------------------------
// Procedure: ShowPlatformInfo()

void KFC_MOOSApp::ShowPlatformInfo()
{
  clearpath::DataPlatformInfo *platform_info = 0;;
  platform_info = clearpath::DataPlatformInfo::getUpdate();
  cout << *platform_info << endl;
  delete platform_info;
  
  clearpath::DataFirmwareInfo *fw_info = 0;;
  fw_info = clearpath::DataFirmwareInfo::getUpdate();
  cout << *fw_info << endl;
  delete fw_info;

  clearpath::DataPowerSystem *power_info = 0;;
  power_info = clearpath::DataPowerSystem::getUpdate();
  cout << *power_info << endl;
  delete power_info;

  clearpath::DataProcessorStatus *cpu_info = 0;;
  cpu_info = clearpath::DataProcessorStatus::getUpdate();
  cout << *cpu_info << endl;
  delete cpu_info;
}
