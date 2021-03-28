/*
 * TempSensorWrapper.h
 *
 *  Created on: Mar 23, 2021
 *      Author: tsasala
 */

#pragma once

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Logger.h>
#include "Hardware.h"


enum class TempTrend { DOWN, NO_CHANGE, UP };
enum class TempStatusCode { OK, FAIL, OUT_OF_MEMORY };


class TempSensorWrapper
{
public:
	TempSensorWrapper();
	virtual ~TempSensorWrapper();

	void initialize();
	void setAlarmTemperature(float alarmTemperature);
	void setAlarmCallback(void (*cb)());

	uint8_t getDeviceCount();

	float getCurrentTemperature(uint8_t index);
	float getCurrentMaxTemperature();
	float getPreviousTemperature(uint8_t index);
	float getPreviousMaxTemperature();

	TempTrend getTrend();

	void sampleTemperatures();


private:
	uint8_t deviceCount = 0;
	float *currentTemperatures = NULL;
	float *previousTemperatures = NULL;
	float currentMaxTemperature = -127.0;
	float previousMaxTemperature = -127.0;
	float alarmTemperature = -127.0;
	TempTrend trend = TempTrend::NO_CHANGE;
	void (*callbackTemperatureAlarm)() = NULL;

};

