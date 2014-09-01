#ifndef CAPABILITIES_H_
#define CAPABILITIES_H_
#include "serial.h"

//Hardware capabilities for RCP MK1

//configuration
#define MAX_CHANNELS			200
#define MAX_TRACKS				240

//Input / output Channels
#define ANALOG_CHANNELS 		8
#define IMU_CHANNELS			7
#define	GPIO_CHANNELS			3
#define TIMER_CHANNELS			3
#define PWM_CHANNELS			4
#define CAN_CHANNELS			2

//sample rates
#define MAX_SENSOR_SAMPLE_RATE	1000
#define MAX_GPS_SAMPLE_RATE		50

#define TELEMETRY_PORT			SERIAL_TELEMETRY
#define WIRELESS_PORT			SERIAL_WIRELESS

#endif /* CAPABILITIES_H_ */