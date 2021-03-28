/*
 * PortWrapper.h
 *
 *  Created on: Mar 25, 2021
 *      Author: tsasala
 */

#pragma once

#include <Arduino.h>
#include <Adafruit_MCP23017.h>

#include "Hardware.h"
#include "Logger.h"

enum class PortStatusCode { OK, FAIL, OUT_OF_MEMORY };
enum class PinAttribute { PULL_UP, PULL_DOWN };
enum class PinType { PIN_INPUT, PIN_OUTPUT };
enum class Port { PORT_A, PORT_B };

namespace Values {
enum PinValue { OFF=0, ON=1 };
}

namespace Outputs{
enum Output {LED1=0, LED2=1, LED3=2, LED4=3, TOUCH=4, SRR1=5, SRR2=6, SRR3=7, SRR4=8 };
}
namespace Inputs {
enum Input {SW1=0, SW2=1, SW3=2, SW4=3, TOUCH=4 };
}



class PortWrapper
{
public:
	PortWrapper();
	virtual ~PortWrapper();

	PortStatusCode initialize();
	void sampleInputs();

	boolean inputChanged();
	void clearInputChanged();

	void clear();
	void setAll();

	void setOutput(Outputs::Output led, Values::PinValue v);
	void setLed(Outputs::Output led, Values::PinValue v);
	void setSrr(Outputs::Output srr, Values::PinValue v);
	Values::PinValue getInput(Inputs::Input sw);
	Values::PinValue getSwitch(Inputs::Input sw);


private:
	Adafruit_MCP23017 port;
	uint8_t *inputValues = NULL;
	uint8_t *outputValues = NULL;
	boolean bInputChanged = false;


};

