// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#pragma once

#define _LOG_LEVEL LOG_LEVEL_DEBUG


#include "Arduino.h"
#include <Ticker.h>
#include <Logger.h>
#include <TinyPico.h>

#include "TempSensorWrapper.h"
#include "FanWrapper.h"
#include "PortWrapper.h"


#define TIMER_PERIOD	 	250
#define TEMP_SAMPLE_COUNT	(5000/TIMER_PERIOD)


