/*
 * PortWrapper.cpp
 *
 *  Created on: Mar 25, 2021
 *      Author: tsasala
 */

#include "PortWrapper.h"

const uint8_t gInputPins[] = {PIN_SW1, PIN_SW2, PIN_SW3, PIN_SW4, PIN_TOUCH_PROBE };
const uint8_t gNumInputPins = ( sizeof(gInputPins) / sizeof(uint8_t) );;

const uint8_t gOutputPins[] = {PIN_SW1_LED, PIN_SW2_LED, PIN_SW3_LED, PIN_SW4_LED, PIN_TOUCH_LED, PIN_SRR1, PIN_SRR2, PIN_SRR3, PIN_SRR4 };
const uint8_t gNumOutputPins = ( sizeof(gOutputPins) / sizeof(uint8_t) );

const uint8_t gLedPins[] = {PIN_SW1_LED, PIN_SW2_LED, PIN_SW3_LED, PIN_SW4_LED, PIN_TOUCH_LED };
const uint8_t gNumLedPins =  ( sizeof(gLedPins) / sizeof(uint8_t) );

//const uint8_t gSrrPins[] = { PIN_SRR1, PIN_SRR2, PIN_SRR3, PIN_SRR4 };
//const uint8_t gNumSrrPins =  ( sizeof(gSrrPins) / sizeof(uint8_t) );


PortWrapper::PortWrapper()
{
}

PortWrapper::~PortWrapper()
{
	if( inputValues )
	{
		free(inputValues);
	}
}

PortStatusCode PortWrapper::initialize()
{
	PortStatusCode psc = PortStatusCode::FAIL;

	LOG_DEBUG("Allocating memory");
	if( !inputValues )
	{
		inputValues = (uint8_t *)malloc( sizeof(uint8_t) * gNumInputPins);
		if( !inputValues )
		{
			LOG_ERROR("Error allocating memory");
			psc = PortStatusCode::OUT_OF_MEMORY;
			return psc;
		}
		else
		{
			for(uint8_t i=0; i<gNumInputPins; i++)
			{
				inputValues[i] = 0;
			}
		}
	}

	if( !outputValues )
	{
		outputValues = (uint8_t *)malloc( sizeof(uint8_t) * gNumOutputPins);
		if( !outputValues )
		{
			LOG_ERROR("Error allocating memory");
			psc = PortStatusCode::OUT_OF_MEMORY;
			return psc;
		}
		else
		{
			for(uint8_t i=0; i<gNumOutputPins; i++)
			{
				outputValues[i] = 0;
			}
		}
	}


	LOG_DEBUG("Setting up port extender");

	Wire.begin( );
	port.begin(0x27, &Wire );

	for(uint8_t i=0; i<gNumInputPins; i++)
	{
		LOG_DEBUG1("Setting Input: ", gInputPins[i]);
		port.pinMode( gInputPins[i], INPUT );
		port.pullUp(gInputPins[i],  PULLUP );
	}

	for(uint8_t i=0; i<gNumOutputPins; i++)
	{
		LOG_DEBUG1("Setting Output: ", gOutputPins[i]);
		port.pinMode( gOutputPins[i], OUTPUT );
	}


	// Zero the port
	port.writeGPIOAB(0);

	boolean value = true;
	for(uint8_t i=0; i<2; i++)
	{
		for(uint8_t j=0; j<gNumLedPins; j++)
		{
			port.digitalWrite(gLedPins[j], value);
			delay(250);
		}
		value = false;
	}

	psc = PortStatusCode::OK;

	return psc;
}

boolean PortWrapper::inputChanged()
{
	return bInputChanged;
}

void PortWrapper::clearInputChanged()
{
	bInputChanged = false;
}

void PortWrapper::sampleInputs()
{
	uint8_t pinValue;
	for(uint8_t i=0; i<gNumInputPins; i++ )
	{
		pinValue = port.digitalRead( gInputPins[i]);
		if( pinValue == 255 )
		{
			LOG_ERROR1("Error reading input pin ", i);
		}
		else
		{
			if( pinValue != inputValues[i])
			{
				inputValues[i] = pinValue;
				bInputChanged = true;
			}
		}
	}
}

void PortWrapper::setOutput(Outputs::Output output, Values::PinValue value)
{
	if( outputValues[output] != value )
	{
		LOG_DEBUG3("setOutput(", output, ")=", value);
		outputValues[output] = value;
		port.digitalWrite( gOutputPins[output], value);
	}
}

void PortWrapper::setLed(Outputs::Output led, Values::PinValue v)
{
	setOutput( led, v);
}

void PortWrapper::setSrr(Outputs::Output srr, Values::PinValue v)
{
	setOutput( srr, v);
}

Values::PinValue PortWrapper::getInput(Inputs::Input input)
{
	return (Values::PinValue)inputValues[input];
}

Values::PinValue PortWrapper::getSwitch(Inputs::Input sw)
{
	// Switches are active low, so invert the return value
	if( inputValues[sw] == Values::ON)
	{
		return Values::OFF;
	}
	else
	{
		return Values::ON;
	}
}


void PortWrapper::clear()
{
	port.writeGPIOAB(0);
}

void PortWrapper::setAll()
{
	port.writeGPIOAB( 0x00ffff );
}

