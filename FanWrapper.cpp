/*
 * FanWrapper.cpp
 *
 *  Created on: Mar 23, 2021
 *      Author: tsasala
 */

#include "FanWrapper.h"

FanWrapper::FanWrapper()
{
}

FanWrapper::~FanWrapper()
{
}

void FanWrapper::initialize()
{
	// Set up fan pin
	pinMode(FAN_PIN, OUTPUT);

	ledcSetup(0, 25000, 8);
	ledcAttachPin(FAN_PIN, 0);
	ledcWrite(0, 0);
}

void FanWrapper::setDutyCyle(uint8_t duty)
{
	this->duty = duty;
	ledcWrite(0, duty);
}

uint8_t FanWrapper::getDutyCycle()
{
	return duty;
}
