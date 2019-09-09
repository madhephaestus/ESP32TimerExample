#include "Arduino.h"
#include "ESP32Servo.h"
#include "analogWrite.h"

double interruptCounter=0;
double sinComp=0;
double numSamples = 30;
double frequencyTarget = 500;
double increment = (2*PI)/numSamples;
// Pointer to the timer object
hw_timer_t *timer = NULL;

// Place the Interrupt handeler in IRAM section of memory
void  IRAM_ATTR onTimer() {
	interruptCounter+=increment;
	if (interruptCounter>=2*PI){
		interruptCounter=0;
	}
	sinComp=(126.0* sin(interruptCounter))+128.0;
	analogWrite( 26, (uint8_t)sinComp);
}

void setup() {
	int count = (1.0/(frequencyTarget/1000000.0))/numSamples;
	Serial.begin(115200);
	Serial.println("Start ESPMutexDemo "+String(count)+" microsecond timer interrupts");
	// get a pointer to a timer to use
	timer = timerBegin(3, // Timer 3
			80, // Divider from the system clock to get to us
			true); // Count up
	timerAttachInterrupt(timer, // Timer to attach the interrupt
			&onTimer, // Interrupt handler passed to timer
			true); // Rising edge of timer
	timerAlarmWrite(timer, // The timer object to use
			count, // count, now now in us
			true); // Reload after finishes, run again and again
	timerAlarmEnable(timer); // Enable timer
}

void loop() {
	Serial.println("Value = "+String(sinComp));
}
