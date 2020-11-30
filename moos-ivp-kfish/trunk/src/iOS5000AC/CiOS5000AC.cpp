// NAME: Michael "Misha" Novitzky
// Adapted from .......
// 
// (c) 2004

// CiOS5000AC.cpp: implementation of the CiOS5000AC class.
////////////////////////////////////////////////////////

/*
#include <iterator>
#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include "CiOS5000AC.h"
#include "MBUtils.h"
#include "tokenize.h"
#include "sutil.h"
#include "remap.h"
#include "ssp.h"
#include "ACTable.h"

using namespace std;

CiOS5000AC::CiOS5000AC()
{
	m_headingMsgName = "COMPASS_HEADING";
	m_yawMsgName     = "COMPASS_YAW";
	m_verbose        = true;
	prerotation      = 0.0;
	thr              = NULL;
	pt               = NULL;
}

bool CiOS5000AC::OnNewMail(MOOSMSG_LIST &NewMail)
{
	AppCastingMOOSApp::OnNewMail(NewMail);
	MOOSMSG_LIST::iterator it;
	for (it = NewMail.begin(); it != NewMail.end(); it++)
		CMOOSMsg &msg = *it;
	NewMail.clear();
	AppCastingMOOSApp::buildReport();
	return true;
}

bool CiOS5000AC::OnConnectToServer()
{
	AppCastingMOOSApp::RegisterVariables();
	string port;
	int speed;
	string sVal;

    if (m_MissionReader.GetConfigurationParam("VERBOSE", sVal)) {
        if (sVal.size() > 0) {
            if (sVal.at(0) == 'T' || sVal.at(0) == 't')
                m_verbose = true; } }
    if (m_verbose)
    	MOOSTrace("%s: VERBOSE mode is on.\n", m_sAppName.c_str());

	m_MissionReader.GetConfigurationParam("Port", port);
	m_MissionReader.GetConfigurationParam("Speed", speed);
	m_MissionReader.GetConfigurationParam("PreRotation", prerotation);
	
	if (m_MissionReader.GetConfigurationParam("Prefix", sVal)) {
		if (!sVal.empty()) {
			size_t sLen = sVal.length();
			char c = sVal.at(sLen - 1);
			if (c == '_') {
				m_headingMsgName = sVal + "HEADING";
				m_yawMsgName     = sVal + "YAW"; }
			else {
				m_headingMsgName = sVal + "_";
				m_headingMsgName += "HEADING";
				m_yawMsgName     = sVal + "_";
				m_yawMsgName     += "YAW"; } } }
	MOOSTrace("%s: Heading will be published as %s.\n", m_sAppName.c_str(), m_headingMsgName.c_str());
	MOOSTrace("%s:     Yaw will be published as %s.\n", m_sAppName.c_str(), m_yawMsgName.c_str());
	pt = new CSerialPort(port);
	pt->SetBaudRate(speed);
	pthread_create(&thr, NULL, &trampoline, this);
	return true;
}

void CiOS5000AC::thread(void)
{
	while (1) {
		int st, en;
		char *s;
		try {
			pt->ReadUntilChar('$');
			pt->ReadUntilChar('\r');
			st = pt->FindCharIndex('$');
			if (st == -1) {
				pt->AllQueueFlush();
				continue; }
			free(pt->Read(st));
			en = pt->FindCharIndex('\r');
			s = pt->Read(en+1);
			if(s == NULL) {
				pt->AllQueueFlush();
				continue; } }
		catch (const exception &e) {
			string s = ssp("Got an exception: %s", e.what());
			printf("%s\n", s.c_str());
			m_Comms.Notify("COMPASS_DEBUG_OS", s);
			continue; }
		char *head, *roll, *pitch, *temp;
		head  = strchr(s, 'C');
		roll  = strchr(s, 'R');
		pitch = strchr(s, 'P');
		temp  = strchr(s, 'T');
		if (head) {
			double h = atof(++head);
			h += prerotation;
			while (h > 360) h -= 360.0;
			while (h < 0)   h += 360.0;
			double y = -h * M_PI / 180.0;

			// Publish heading
			m_Comms.Notify(m_headingMsgName.c_str(), h);
			m_AppCastValues[m_headingMsgName.c_str()] = h;
			m_AppCastTimestamp[m_headingMsgName.c_str()] = MOOSTime();

			// Publish Yaw
			m_Comms.Notify(m_yawMsgName.c_str(), y);
			m_AppCastValues[m_yawMsgName.c_str()] = y;
			m_AppCastTimestamp[m_yawMsgName.c_str()] = MOOSTime();
			if (m_verbose)
				MOOSTrace("%s: Heading = %lf\n", m_sAppName.c_str(), h); }
		if (roll) {
			double r = atof(++roll);
			m_Comms.Notify("COMPASS_ROLL", r);
			m_AppCastValues["COMPASS_ROLL"] = r;
			m_AppCastTimestamp["COMPASS_ROLL"] = MOOSTime(); }
		if (pitch) {
			double p = atof(++pitch);
			m_Comms.Notify("COMPASS_PITCH", p);
			m_AppCastValues["COMPASS_PITCH"] = p;
			m_AppCastTimestamp["COMPASS_PITCH"] = MOOSTime(); }
		if (temp) {
			double t = atof(++temp);
			m_Comms.Notify("COMPASS_TEMPERATURE", t);
			m_AppCastValues["COMPASS_TEMPERATURE"] = t;
			m_AppCastTimestamp["COMPASS_TEMPERATURE"] = MOOSTime(); }
		free(s); }
}

bool CiOS5000AC::Iterate()
{
	AppCastingMOOSApp::Iterate();
	AppCastingMOOSApp::PostReport();
	return true;
}

bool CiOS5000AC::OnStartUp()
{
	AppCastingMOOSApp::OnStartUp();
	// happens after connection is completely usable
	// ... not when it *should* happen. oh well...
	return true;
}

bool CiOS5000AC::buildReport()
{
	ACTable actab(3);
	actab << "Variable | Time | Value";
	actab.addHeaderLines();
	actab.setColumnMaxWidth(4, 55);
	actab.setColumnNoPad(4);

	//Pack values notified to MOOSDB

	map<string,double>::iterator q;
	for (q = m_AppCastValues.begin(); q != m_AppCastValues.end(); q++) {
		string varname = q->first;
		double value = q->second;
		double timeSinceSentToMOOSDB = MOOSTime() - m_AppCastTimestamp[varname];
		actab << varname << timeSinceSentToMOOSDB << value; }
	m_msgs << endl << endl;
	m_msgs << actab.getFormattedString();
	return true;
}


*/


//

