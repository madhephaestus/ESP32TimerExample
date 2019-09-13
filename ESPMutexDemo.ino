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
/**
 * Enable for all H-bridges
 */
#define H_BRIDGE_ENABLE             13
// Hardware interface objects
ESP32Encoder encoder;
ESP32PWM motor;
// variables
float setpoint = 10000; // Arbitrary encoder value target
float position =0; // The current position
float err = 0;  // The P term error
float pGain = 5.0; // a P gain value
float outvalue = 0; // the value that is output to the PWM

void setup() {
	Serial.begin(115200);
	Serial.println("Start ESPMutexDemo ");
	// Attach the encoder pins to the encoder object
	encoder.attachHalfQuad(MOTOR1_ENCA, MOTOR1_ENCB);
	// start the PWM on the PWM pins
	motor.attachPin(MOTOR1_PWM, 20000, 8);
	// Motor direction pins
	pinMode(MOTOR1_DIR, OUTPUT);
	// H-Bridge enable pin
	pinMode(H_BRIDGE_ENABLE, OUTPUT);
	// Enble the h-bridge
	digitalWrite(H_BRIDGE_ENABLE, 1);
}

void loop() {
	// Run at 1khz
	delay(1);
	// get the current position from the encoder object
	position=encoder.getCount();
	// Compute the error between target and current position
	err = setpoint-position;
	// Scale the error to an output correction value
	outvalue = err * pGain;
	// Set the direction flag based on if the correction is positive or negative
	if (outvalue > 0) {
		digitalWrite(MOTOR1_DIR, HIGH); // turn the LED on (HIGH is the voltage level)
	} else {
		digitalWrite(MOTOR1_DIR, LOW);
	}
	// Write the Absolute value of the correction to the PWM
	motor.write(abs(outvalue));
	Serial.println("Motor data "+String(outvalue)+" Setpoint "+String(setpoint)+" pos "+String(position));
}
