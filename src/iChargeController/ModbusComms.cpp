/*
 * ModbusComms.cpp
 * 
 * Created on: 3/27/2021
 * Author: Ethan Rolland
 *
 */

#include "ModbusComms.h"
#include "ColorParse.h"
#include <string.h>
#include <cerrno>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

ModbusComms::ModbusComms(string port, int baud)
{
	on = false;
	
	Modbus = modbus_new_rtu((char*)port.c_str(), baud, 'N', 8, 1);
	modbus_set_slave(Modbus, 1);
	modbus_set_debug(Modbus, TRUE);
	modbus_set_debug(Modbus, FALSE);
	modbus_set_error_recovery(Modbus, (modbus_error_recovery_mode)(MODBUS_ERROR_RECOVERY_LINK|MODBUS_ERROR_RECOVERY_PROTOCOL));
	
	if (modbus_connect(Modbus)==-1) {
		modbus_free(Modbus);
		return;
	}
	else {
		on = true;
	}
	
	updateRegTab(regTab, netCurr);
}

ModbusComms::~ModbusComms()
{
	modbus_close(Modbus);
}

bool ModbusComms::getOn()
{
	int ret = -1;
	ret = updateRegTab(regTab,netCurr);
	if (ret!=-1) {
		return true; }
	else {
		return false; }
}

void ModbusComms::setMaxRetries(int m)
{
	maxRetries = m;
}

int ModbusComms::updateRegTab(uint16_t* Tab, uint16_t* netCurr)
{
	int registers_read = -1;
	int registerAddress = 0x3100;
	int numBytes = 0x16;
	for(int w = 0 ; w < MAX_RETRIES ; w ++)
	{
		registers_read = modbus_read_registers(Modbus,registerAddress,numBytes,Tab);
		modbus_read_registers(Modbus,0x331B,0x01,netCurr);
		if(registers_read != -1)
			break;
	}
	if (registers_read!=-1)
	{
		return 1;
	} else {
		return 0;
	}
}

int ModbusComms::updateAll()
{
	int ret;
	ret = updateRegTab(regTab,netCurr);
	
	pvVolt         = regTab[0x00]/100.0;
	pvCurr         = regTab[0x01]/100.0;
	pvPowerL       = regTab[0x02]/100.0;
	pvPowerH       = regTab[0x03]/100.0;
	batteryVolt    = regTab[0x04]/100.0;
	batteryCurr    = regTab[0x05]/100.0;
	batteryPowerL  = regTab[0x06]/100.0;
	batteryPowerH  = regTab[0x07]/100.0;
	loadVolt       = regTab[0x0C]/100.0;
	loadCurr       = regTab[0x0D]/100.0;
	loadPowerL     = regTab[0x0E]/100.0;
	loadPowerH     = regTab[0x0F]/100.0;
	batteryTemp    = regTab[0x10]/100.0;
	deviceTemp     = regTab[0x11]/100.0;
	compTemp       = regTab[0x12]/100.0;
	batterySOC     = regTab[0x1A]/100.0;
	
	batteryNetCurr = netCurr[0x1B]/100.0;
	
	return ret;	
}

double ModbusComms::getPVVolt()
{
	return pvVolt;
}

double ModbusComms::getPVCurr()
{
	return pvCurr;
}

double ModbusComms::getPVPower()
{
	return pvPowerL;
}

double ModbusComms::getBatteryVolt()
{
	return batteryVolt;
}

double ModbusComms::getBatteryCurr()
{
	return batteryCurr;
}

double ModbusComms::getBatteryPower()
{
	return batteryPowerL;
}

double ModbusComms::getLoadVolt()
{
	return loadVolt;
}

double ModbusComms::getLoadCurr()
{
	return loadCurr;
}

double ModbusComms::getLoadPower()
{
	return loadPowerL;
}

double ModbusComms::getBatteryTemp()
{
	return batteryTemp;
}

double ModbusComms::getDeviceTemp()
{
	return deviceTemp;
}

double ModbusComms::getCompTemp()
{
	return compTemp;
}

double ModbusComms::getBatterySOC()
{
	return batterySOC;
}

double ModbusComms::getBatteryNetCurr()
{
	return batteryNetCurr;
}	

