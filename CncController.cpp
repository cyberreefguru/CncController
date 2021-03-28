// Fan_Temp_Control.ino ##############################################
/* This code was written to control the temperature inside a server cabinet
  by measuring the temperature with a DS18b20 Temperature probe and
  outputting a PWM signal with the Arduino Nano to control a 4-Pin fan.
  Unlike the cheap control boards from Amazon or Ebay this code switches
  the fan off when the temperature is low enough. */


/*

  The following constants should be changed according to the use case:

  constant (default value) - description

  tempLow (35) - Below this temperature (minus half hysteresis) the fan shuts off.
               It shuts on again at this temperature plus half hysteresis

  tempHigh (50) - At and above this temperature the fan is at maximum speed

  hyteresis (5) - Hysteresis to prevent frequent on/off switching at the threshold

  minDuty (10) - Minimum fan speed to prevent stalling
  maxDuty (100) - Maximum fan speed to limit noise

*/
#include "CncController.h"

TempSensorWrapper tsw;
FanWrapper fw;
TinyPICO tp = TinyPICO();
PortWrapper port;

Ticker ticker;

volatile boolean gInputSample = false;
volatile uint8_t gTempSampleCounter = 0;
volatile uint8_t gLedCounter = 0;
volatile uint32_t gLedColor = 0;

void timerHandler();

float temps[] = {25, 28, 31, 34 };
uint8_t duties[] = {50, 100, 175, 255};

/**
 * Setup function
 */
void setup()
{
	// start serial port
	Serial.begin(115200);

	LOG_INFO("** Begin Initialize **");

	// Initialize fan
	fw.initialize();
	fw.setDutyCyle(0);

	// Initialize TinyPICO
	tp.initialize();
	tp.setLedColor( 0 );
	tp.setLedBrightness( 25 );

	for(uint8_t i=0; i< 5; i++)
	{
		tp.setLedColor( 0x00ff0000 );
		delay(250);
		tp.setLedColor( 0x00000ff );
		delay(250);
	}

	// Set LED color to blue
	gLedColor = 0x0000ff;
	tp.setLedColor( gLedColor );

	// Initialize ports
	port.initialize();

	// Initialize temp sensor
	tsw.initialize();


	// Initialize timer
	ticker.attach_ms( TIMER_PERIOD, timerHandler );

	LOG_INFO("** End Initialize **");

}


/**
 * Main loop
 *
 */
void loop() {

	// current duty cycle
	uint8_t duty = 0;

	if(gTempSampleCounter == TEMP_SAMPLE_COUNT)
	{
		gTempSampleCounter = 0;
		tsw.sampleTemperatures();
		float temp = tsw.getCurrentMaxTemperature();

		if (temp > temps[3])
		{
			duty = duties[3];
			gLedColor = tp.getColor(0xff, 0x00, 0x00);
		}
		else if ( temp > temps[2])
		{
			duty = duties[2];
			gLedColor = tp.getColor(0xff, 0x00, 0xff);
		}
		else if ( temp > temps[1])
		{
			duty = duties[1];
			gLedColor = tp.getColor(0xff, 0xff, 0x00);
		}
		else if ( temp > temps[0])
		{
			duty = duties[0];
			gLedColor = tp.getColor(0x00, 0xff, 0x00);
		}
		else
		{
			duty = 0;
			gLedColor = tp.getColor(0x00, 0x00, 0xff);
		}
		fw.setDutyCyle(duty);

	} // end if new sample

	// If we are ready to sample inputs, do so here
	// I could not sample inputs in the ISR because the
	// ISR fired while I was writing port values here
	// which caused a kernel panic.
	if( gInputSample )
	{
		gInputSample = false;
		port.sampleInputs();
	}

	if(port.inputChanged())
	{
		if( port.getSwitch(Inputs::SW1) == Values::ON )
		{
			port.setLed(Outputs::LED1, Values::ON);
			port.setSrr(Outputs::SRR1, Values::ON);
		}
		else if( port.getSwitch(Inputs::SW1) == Values::OFF )
		{
			port.setLed(Outputs::LED1, Values::OFF);
			port.setSrr(Outputs::SRR1, Values::OFF);
		}
		if( port.getSwitch(Inputs::SW2) == Values::ON )
		{
			port.setLed(Outputs::LED2, Values::ON);
			port.setSrr(Outputs::SRR2, Values::ON);
		}
		else if( port.getSwitch(Inputs::SW2) == Values::OFF )
		{
			port.setLed(Outputs::LED2, Values::OFF);
			port.setSrr(Outputs::SRR2, Values::OFF);
		}
		if( port.getSwitch(Inputs::SW3) == Values::ON )
		{
			port.setLed(Outputs::LED3, Values::ON);
			port.setSrr(Outputs::SRR3, Values::ON);
		}
		else if( port.getSwitch(Inputs::SW3) == Values::OFF )
		{
			port.setLed(Outputs::LED3, Values::OFF);
			port.setSrr(Outputs::SRR3, Values::OFF);
		}
		if( port.getSwitch(Inputs::SW4) == Values::ON )
		{
			port.setLed(Outputs::LED4, Values::ON);
			port.setSrr(Outputs::SRR4, Values::ON);
		}
		else if( port.getSwitch(Inputs::SW4) == Values::OFF )
		{
			port.setLed(Outputs::LED4, Values::OFF);
			port.setSrr(Outputs::SRR4, Values::OFF);
		}
		if( port.getSwitch(Inputs::TOUCH) == Values::ON )
		{
			port.setLed(Outputs::TOUCH, Values::ON);
		}
		else if( port.getSwitch(Inputs::TOUCH) == Values::OFF )
		{
			port.setLed(Outputs::TOUCH, Values::OFF);
		}

		port.clearInputChanged();
	}
}

void timerHandler()
{
	gLedCounter++;
	gTempSampleCounter++;
	gInputSample = true;

	// Create heart beat - on 1 cycle, off 2 cycles, on 1 cycle, off 6 cycles
	switch( gLedCounter )
	{
	case 1:
		tp.setLedColor(gLedColor);
		break;
	case 2:
		tp.clearLed();
		break;
	case 4:
		tp.setLedColor(gLedColor);
		break;
	case 5:
		tp.clearLed();
		break;
	case 11:
		gLedCounter=0;
		break;
	}

}
