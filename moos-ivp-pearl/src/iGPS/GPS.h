/*
 * GPS.h
 * 
 * Created on: 1/12/2021
 * Author: Ethan Rolland
 * 
 */

#ifndef GPSDEVICE_H_
#define GPSDEVICE_H_

#include <map>
#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"
#include "MOOS/libMOOS/ThirdParty/AppCasting/AppCastingMOOSApp.h"
#include "SerialComms.h"
#include "gpsParser.h"

#define BAD_DOUBLE -99999.99

