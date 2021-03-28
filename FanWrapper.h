/*
 * FanWrapper.h
 *
 *  Created on: Mar 23, 2021
 *      Author: tsasala
 */
#pragma once

#include <Arduino.h>
#include "Hardware.h"


enum class FanStatusCode { OK, FAIL, OUT_OF_MEMORY };



class FanWrapper
{
public:
	FanWrapper();
	virtual ~FanWrapper();

	void initialize();
	void setDutyCyle(uint8_t duty);
	uint8_t getDutyCycle();

private:
	uint8_t duty = 0;

};

