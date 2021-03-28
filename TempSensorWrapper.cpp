/*
 * TempSensorWrapper.cpp
 *
 *  Created on: Mar 23, 2021
 *      Author: tsasala
 */

#include "TempSensorWrapper.h"

// Setup a oneWire instance
OneWire oneWire(ONE_WIRE_BUS);

// Setup temperature sensor library
DallasTemperature sensors(&oneWire);

TempSensorWrapper::TempSensorWrapper()
{
	// TODO Auto-generated constructor stub

}

TempSensorWrapper::~TempSensorWrapper()
{
	// TODO Auto-generated destructor stub
}

void TempSensorWrapper::initialize()
{
	LOG_DEBUG("Initializing temperature sensors");

	// Start up the temperature library
	sensors.begin();
	sensors.setResolution(12);
	sensors.setWaitForConversion(false);

	deviceCount = sensors.getDS18Count();
	if (deviceCount > 0)
	{
		LOG_DEBUG1("Detected temperature sensors: ", deviceCount);

		currentTemperatures = (float*) malloc(deviceCount * sizeof(float));
		if (currentTemperatures == NULL)
		{
			LOG_ERROR("Unable to allocate current temperature buffer.");
		}
		previousTemperatures = (float*) malloc(deviceCount * sizeof(float));
		if (previousTemperatures == NULL)
		{
			LOG_ERROR("Unable to allocate previous temperature buffer.");
		}
	}
	else
	{
		LOG_ERROR("Unable to locate any devices on the bus.");
	}

	// Get temperatures
	LOG_DEBUG("Requesting temperatures...");
	sensors.requestTemperatures();

	LOG_DEBUG("Temperature sensor initialization complete.")
}

/**
 * Sets the callback function that gets called
 * when a temperature alarm is generated.
 */
void TempSensorWrapper::setAlarmCallback(void (*cb)())
{
	this->callbackTemperatureAlarm = cb;
}

float TempSensorWrapper::getCurrentTemperature(uint8_t index)
{
	float f = -127.0;
	if (currentTemperatures != NULL && index < deviceCount)
	{
		f = currentTemperatures[index];
	}
	return f;
}

float TempSensorWrapper::getCurrentMaxTemperature()
{
	return currentMaxTemperature;
}

float TempSensorWrapper::getPreviousTemperature(uint8_t index)
{
	float f = -127.0;
	if (previousTemperatures != NULL && index < deviceCount)
	{
		f = previousTemperatures[index];
	}
	return f;
}

float TempSensorWrapper::getPreviousMaxTemperature()
{
	return previousMaxTemperature;
}

/**
 * Returns the temperature trend - UP, DOWN, NO_CHANGE
 */
TempTrend TempSensorWrapper::getTrend()
{
	return trend;
}

/**
 * Samples temperatures and calls alarm as required
 */
void TempSensorWrapper::sampleTemperatures()
{
	if (previousTemperatures == NULL || currentTemperatures == NULL)
	{
		// LOG ERROR
		return;
	}

	// flip flop current and previous temperatures
	float *t = previousTemperatures;
	previousTemperatures = currentTemperatures;
	currentTemperatures = t;

	previousMaxTemperature = currentMaxTemperature;
	currentMaxTemperature = -127.0;

	for (uint8_t i = 0; i < deviceCount; i++)
	{
		currentTemperatures[i] = sensors.getTempCByIndex(i);
		LOG_DEBUG3("Current Temp (", i, ")=", currentTemperatures[i]);
		if (currentTemperatures[i] > currentMaxTemperature)
		{
			currentMaxTemperature = currentTemperatures[i];
		}
	}

	// If previous is default, set to current
	// This will happen the first time we sample the temps
	if (previousMaxTemperature == -127.0)
	{
		previousMaxTemperature = currentMaxTemperature;
	}

	// Set the trend variable.  Used for hysteresis if required.
	if (currentMaxTemperature > previousMaxTemperature)
	{
		trend = TempTrend::UP;
	}
	else if (currentMaxTemperature < previousMaxTemperature)
	{
		trend = TempTrend::DOWN;
	}
	else
	{
		trend = TempTrend::NO_CHANGE;
	}

	// Get temperatures
	sensors.requestTemperatures();

	// If callback is set, test alarm condition
	if (callbackTemperatureAlarm)
	{
		if (currentMaxTemperature > alarmTemperature)
		{
			// Call alarm function
			callbackTemperatureAlarm();
		}
	}

	LOG_DEBUG3("Current Max: ", currentMaxTemperature, " Previous Max: ", previousMaxTemperature );

}

