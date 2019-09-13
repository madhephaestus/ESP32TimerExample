#include "Arduino.h"
#include "ESP32Servo.h"
#include "ESP32Encoder.h"
/**
 * Drive motor 1 10Khz full duty PWM pin
 */
#define MOTOR1_PWM 5
/**
 * Pin for setting the direction of the H-Bridge
 */
#define MOTOR1_DIR 4
//Encoder pins
#define MOTOR1_ENCA 19
#define MOTOR1_ENCB 18

// Pointer to the timer object
hw_timer_t *timer = NULL;

ESP32Encoder encoder;
ESP32PWM motor;

float setpoint = 1000;
float position =0;
float output =  0;
float err = 0;
float pGain = 1.0;
float outvalue = 0;
// Place the Interrupt handeler in IRAM section of memory
void  IRAM_ATTR onTimer() {
	position=encoder.getCount();
	err = setpoint-position;
	outvalue = err * pGain;
	if (outvalue > 0) {
		digitalWrite(MOTOR1_DIR, HIGH); // turn the LED on (HIGH is the voltage level)
	} else {
		digitalWrite(MOTOR1_DIR, LOW);
	}
	motor.write(abs(outvalue));
}

void setup() {
	int count = 5000;
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

	encoder.attachHalfQuad(MOTOR1_ENCA, MOTOR1_ENCB);
	motor.attachPin(MOTOR1_PWM, 20000, 8);
	pinMode(MOTOR1_DIR, OUTPUT);

}

void loop() {
	// do nothing
}
