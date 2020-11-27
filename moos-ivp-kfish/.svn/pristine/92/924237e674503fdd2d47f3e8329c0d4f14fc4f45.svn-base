/*
 * uj.h
 *
 *  Created on: Jan 14 2015
 *      Author: Alon Yaari
 */

#ifndef M200SIM_H_
#define M200SIM_H_

//#include <iostream>
#include <vector>
#include <deque>
#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"


class valuePair
{
public:
  double              timeStamp;
  double              dVal;
};

class jData
{
public:
  std::string         msgName;
  unsigned int        maxNum;
  unsigned int        countToDate;
  std::string         fileName;
  std::deque<valuePair>  valuePairs;
};

class uJSON : public AppCastingMOOSApp
{
public:
                    uJSON();
    virtual         ~uJSON() {};


protected:
    bool            RegisterForMOOSMessages();
    bool            Iterate();
    bool            OnNewMail(MOOSMSG_LIST &NewMail);
    bool            OnConnectToServer();
    bool            OnStartUp();
    bool            IngestData(double dVal, jData* jd);
    bool            CreateJSON(jData* jd);
    std::string     FormatJSONdataLine(valuePair vp);
    bool            PublishToFile(std::string str, std::string fName);
    bool            buildReport();

    bool            SetParam_JSON(std::string sVal);

    double          m_curDBuptime;
    std::string     m_lastJSON;
    std::vector<jData> m_data;

};





#endif


















//

