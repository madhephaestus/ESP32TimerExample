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

ESP32Encoder encoder;
ESP32PWM motor;

float setpoint = 10000;
float position =0;
float output =  0;
float err = 0;
float pGain = 5.0;
float outvalue = 0;

void setup() {
	Serial.begin(115200);
	Serial.println("Start ESPMutexDemo ");
	encoder.attachHalfQuad(MOTOR1_ENCA, MOTOR1_ENCB);
	motor.attachPin(MOTOR1_PWM, 20000, 8);
	pinMode(MOTOR1_DIR, OUTPUT);
	pinMode(H_BRIDGE_ENABLE, OUTPUT);
	digitalWrite(H_BRIDGE_ENABLE, 1);
}

void loop() {

	delay(1);
	position=encoder.getCount();
	err = setpoint-position;
	outvalue = err * pGain;
	if (outvalue > 0) {
		digitalWrite(MOTOR1_DIR, HIGH); // turn the LED on (HIGH is the voltage level)
	} else {
		digitalWrite(MOTOR1_DIR, LOW);
	}
	motor.write(abs(outvalue));
	Serial.println("Motor data "+String(outvalue)+" Setpoint "+String(setpoint)+" pos "+String(position));
}
