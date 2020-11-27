/*
 * uj.cpp
 *
 *  Created on: Jan 14 2015
 *      Author: Alon Yaari
 */

#include <sstream>
#include "MBUtils.h"
#include "uj.h"

using namespace std;

uJSON::uJSON()
{
  m_curDBuptime = -1.0;
  m_lastJSON    = "";
}

bool uJSON::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
  MOOSMSG_LIST::iterator p;
  for (p=NewMail.begin(); p!=NewMail.end(); ++p) {
    CMOOSMsg & rMsg = *p;
    if (MOOSStrCmp(rMsg.GetKey(), "DB_UPTIME"))
      m_curDBuptime = rMsg.GetDouble();
    for (size_t i = 0; i < m_data.size(); i++) {
      if (MOOSStrCmp(rMsg.GetKey(), m_data.at(i).msgName))
        IngestData(rMsg.GetDouble(), &m_data.at(i)); } }
  return UpdateMOOSVariables(NewMail);
}

bool uJSON::IngestData(double dVal, jData* jd)
{
  cout << "Incoming message  " << jd->msgName << "   time: " << m_curDBuptime << "   value: " << dVal << endl;
  valuePair vp = { m_curDBuptime, dVal };
  jd->valuePairs.push_back(vp);
  while (jd->valuePairs.size() > jd->maxNum)
    jd->valuePairs.pop_front();
  jd->countToDate++;
  return CreateJSON(jd);
}

string uJSON::FormatJSONdataLine(valuePair vp)
{
  stringstream line;
  line << "{\"c\":[{\"v\":\"";
  line << vp.timeStamp;
  line << "\",\"f\":null},{\"v\":\"";
  line << vp.dVal;
  line << "\",\"f\":null}]}";
  return line.str();
}

bool uJSON::CreateJSON(jData* jd)
{
  int valSize = jd->valuePairs.size();
  if (valSize < 2)
    return true;

  if (m_curDBuptime < 0.0)
    return true;

  stringstream json;
  json << "  {" << endl;
  json << "   \"cols\": [" << endl;
  json << "         {\"label\":\"DB_TIME\",\"type\":\"number\"}," << endl;
  json << "         {\"label\":\"";
  json << jd->msgName;
  json << "\",\"type\":\"number\"}";
  json << "       ]," << endl;
  json << "   \"rows\": [" << endl;
  for (int i = 0; i < valSize; i++) {
    json << "         ";
    json << FormatJSONdataLine(jd->valuePairs.at(i));
    if (i < valSize - 1)
      json << ",";
    json << endl; }
  json << "        ]" << endl;
  json << "  }" << endl;

  return PublishToFile(json.str(), jd->fileName);
}

bool uJSON::PublishToFile(string str, string fName)
{
  m_lastJSON = str;
  if (str.empty())
    return true;
  cout << " -------------------------------- " << endl;
  cout << str << endl;

  ofstream jFile;
  jFile.open(fName.c_str());
  jFile << str;
  jFile.close();
  return true;
}

bool uJSON::Iterate()
{
  AppCastingMOOSApp::Iterate();
  AppCastingMOOSApp::PostReport();
  return true;
}

bool uJSON::OnConnectToServer()
{
  return true;
}

bool uJSON::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  STRING_LIST sParams;
  if (!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  bool bHandled = true;
  STRING_LIST::iterator p;
  for (p = sParams.begin(); p != sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;

    if (param == "JSON")
      bHandled = SetParam_JSON(value);
    else
      reportUnhandledConfigWarning(orig); }
  RegisterForMOOSMessages();
  return true;
}

bool uJSON::RegisterForMOOSMessages()
{
  AppCastingMOOSApp::RegisterVariables();
  m_Comms.Register("DB_UPTIME", 0);
  for (size_t i = 0; i < m_data.size(); i++)
    m_Comms.Register(m_data.at(i).msgName, 0);
  return RegisterMOOSVariables();
}

bool uJSON::SetParam_JSON(string sVal)
{
  if (sVal.empty()) {
    reportConfigWarning("Mission file parameter JSON must not be blank; line is ignored.");
    return false; }
  sVal = removeWhite(sVal);
  vector<string> items = parseString(sVal, ',');
  if (items.size() != 3 || !isNumber(items.at(1), false)) {
    string warning = "Mission file parameter JSON must have three comma-delimited fields: messageName, number, fileName. ";
    warning += "Ignored: ";
    warning += sVal;
    reportConfigWarning(warning);
    return false; }
  jData jd;
  jd.msgName  = items.at(0);
  jd.maxNum   = strtol(items.at(1).c_str(), NULL ,10);
  jd.fileName = items.at(2);
  m_data.push_back(jd);
  return true;
}

bool uJSON::buildReport()
{
  int count = m_data.size();
  if (count == 0) {
    m_msgs << "Not creating any JSON output." << endl;
    return true; }
  for (int i = 0; i < count; i++) {
    m_msgs << "   " << m_data.at(i).msgName;
    for (size_t j = 0; j < 22 - m_data.at(i).msgName.length(); j++)
      m_msgs << " ";
    m_msgs << m_data.at(i).countToDate << endl; }
  m_msgs << "Last JSON: " << m_lastJSON << " ... " << endl;
  return true;
}


























//
