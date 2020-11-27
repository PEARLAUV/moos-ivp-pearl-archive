// $Header: /home/cvsroot/project-marine-shell/src/iOS5000/CiOS5000.cpp,v 1.6 2007/08/03 19:59:34 anrp Exp $
// (c) 2004

// CiOS5000.cpp: implementation of the CiOS5000 class.
////////////////////////////////////////////////////////

#include <iterator>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include "CiOS5000.h"
#include "tokenize.h"
#include "sutil.h"
#include "remap.h"
#include "ssp.h"

using namespace std;

CiOS5000::CiOS5000()
{
}

CiOS5000::~CiOS5000()
{
}

bool CiOS5000::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator it;
	
	for(it = NewMail.begin(); it != NewMail.end(); it++) {
		CMOOSMsg &msg = *it;
	}

	NewMail.clear();
	
	return true;
}

bool CiOS5000::OnConnectToServer()
{
	string port;
	int speed;

	m_MissionReader.GetConfigurationParam("Port", port);
	m_MissionReader.GetConfigurationParam("Speed", speed);

	m_MissionReader.GetConfigurationParam("PreRotation", prerotation);
	
	pt = new CSerialPort(port);
	pt->SetBaudRate(speed);

	pthread_create(&thr, NULL, &trampoline, this);
	
	return true;
}

void CiOS5000::thread(void)
{
	while(1) {
		int st, en;
		char *s;
		try {
			pt->ReadUntilChar('$');
			pt->ReadUntilChar('\r');
			st = pt->FindCharIndex('$');
			if(st == -1) {
				pt->AllQueueFlush();
				continue;
			}
			free(pt->Read(st));
			en = pt->FindCharIndex('\r');

			s = pt->Read(en+1);
			if(s == NULL) {
				pt->AllQueueFlush();
				continue;
			}
		} catch (const exception &e) {
			string s = ssp("Got an exception: %s", e.what());
			printf("%s\n", s.c_str());
			m_Comms.Notify("COMPASS_DEBUG_OS", s);
			
			continue;
		}

		char *head, *roll, *pitch, *temp;

		head = strchr(s, 'C');
		roll = strchr(s, 'R');
		pitch = strchr(s, 'P');
		temp = strchr(s, 'T');

		if(head) {
			double h = atof(++head);
			h+=prerotation;
			while(h > 360) h -= 360.0;
			while(h < 0) h += 360.0;
			double y = -h * M_PI/180.0;

			m_Comms.Notify("COMPASS_HEADING", h);
			m_Comms.Notify("COMPASS_YAW", y);

			printf("Heading: %lf\n", h);
		}

		if(roll) {
			double r = atof(++roll);

			m_Comms.Notify("COMPASS_ROLL", r);
		}

		if(pitch) {
			double p = atof(++pitch);

			m_Comms.Notify("COMPASS_PITCH", p);
		}

		if(temp) {
			double t = atof(++temp);
			
			m_Comms.Notify("COMPASS_TEMPERATURE", t);
		}

		free(s);
	}
}

bool CiOS5000::Iterate()
{
	
	return true;
}

bool CiOS5000::OnStartUp()
{
	// happens after connection is completely usable
	// ... not when it *should* happen. oh well...
	
	return true;
}

