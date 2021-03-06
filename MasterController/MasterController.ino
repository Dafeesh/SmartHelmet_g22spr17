#include "System.h"
#include "SignalData.h"
#include "Sensors.h"
#include "CommClient.h"
#include "TurnSignal.h"
bool ___init;

#define SERIAL_LOCAL_RATE		9600
#define SEND_DELAY				250
#define PULSE_DELAY				400
#define PRINT_DELAY				5000

#define PIN_LED_STATUS			13
#define PIN_COMM_RX				8
#define PIN_COMM_TX				9

#define PIN_SIGNAL_LEFT			A0
#define PIN_SIGNAL_RIGHT		A1

#define PIN_SENSOR_READ			A3
#define PIN_SENSOR_SELECT_A		2
#define PIN_SENSOR_SELECT_B		3
#define PIN_SENSOR_SELECT_C		4

SignalData left, right;

void setup(void)
{
	System::init(SERIAL_LOCAL_RATE, PIN_LED_STATUS);
	System::toggleStatusLED(true);
	delay(100);

	debugPrintln(F("Starting Bluetooth..."));
	CommClient::init(PIN_COMM_RX, PIN_COMM_TX);
	delay(100);

	debugPrintln(F("Starting Sensors..."));
	Sensors::init(
		PIN_SENSOR_READ, PIN_SENSOR_SELECT_A, PIN_SENSOR_SELECT_B, PIN_SENSOR_SELECT_C);
	delay(100);

	debugPrintln(F("Initializing turn signals..."));
	TurnSignal::init(PIN_SIGNAL_LEFT, PIN_SIGNAL_RIGHT);
	delay(100);

	debugPrintln(F("Done!"));
	delay(100);

	for (int i = 0; i < 10; i++) {
		System::toggleStatusLED(true);
		delay(50);
		System::toggleStatusLED(false);
		delay(50);
	}

	debugPrintln(F("--- Start ---"));
}

unsigned long lastSend = 0;
unsigned long lastPulse = 0;
unsigned long lastPrint = 0;
void loop(void)
{
	Sensors::getReadings(left, right);

	if (lastSend + SEND_DELAY < millis())
	{
		CommClient::sendSignalData(left);
		CommClient::sendSignalData(right);
		lastSend = millis();
	}
	if (lastPulse + PULSE_DELAY < millis())
	{
		Sensors::pulseChain();
		lastPulse = millis();
	}
	if (lastPrint + PRINT_DELAY < millis())
	{
		debugPrint("Left:  ");
		debugPrintln((int)left.dangerMagnitude);
		debugPrint("Right: ");
		debugPrintln((int)right.dangerMagnitude);
		lastPrint = millis();
	}

	delay(10);
}