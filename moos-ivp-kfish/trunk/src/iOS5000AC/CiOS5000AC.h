// NAME: Michael "Misha" Novitzky
// BASED ON: .......
// (c) 2004 
// CiOS5000AC.h: interface for the CiOS5000 class.
////////////////////////////////////////////////

/*
#ifndef __CiOS5000AC_h__
#define __CiOS5000AC_h__



// YARECLIPSE is a convenience definition for eclipse to find .h files in nearby projects
// Project properties -> C/C++ General -> Paths and Symbols -> Symbols tab -> Add YARECLIPSE with value TRUE
#ifdef YARECLIPSE
#include "/Users/yar/moos-ivp/MOOS_V10.0.0/MOOSCore/Core/libMOOS/Thirdparty/AppCasting/include/MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#else
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#endif

#include "MOOS/libMOOS/MOOSLib.h"
#include "CSerialPort.h"


class CiOS5000AC : public AppCastingMOOSApp
{
public:
	CiOS5000AC();
	virtual ~CiOS5000AC() {};

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
        bool buildReport();

protected:
	// insert local vars here
	CSerialPort *pt;

	double prerotation;

	pthread_t thr;
	static void *trampoline(void *arg) {
		((CiOS5000AC *)arg)->thread();
		return NULL; }

	void thread(void);
	std::map<string,double> m_AppCastValues;
	std::map<string,double> m_AppCastTimestamp;
	std::string m_headingMsgName;
	std::string m_yawMsgName;
	bool m_verbose;
};

#endif


*/






//
