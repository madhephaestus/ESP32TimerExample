#include "Arduino.h"
#include "ESP32Servo.h"
#include "ESP32PWM.h"

const byte interruptPin = 25;
volatile double interruptCounter = 0;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
ESP32PWM pwm;
void  IRAM_ATTR  handleInterrupt() {
  portENTER_CRITICAL_ISR(&mux);
  interruptCounter+=1.0;
  portEXIT_CRITICAL_ISR(&mux);
}

void setup() {

  Serial.begin(115200);
  Serial.println("Monitoring interrupts: ");
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, CHANGE);
  pwm.attachPin(33, 40000, 8);
  pwm.writeScaled(0.5);

}

void loop() {

  //if(interruptCounter>0){

      //portENTER_CRITICAL(&mux);
      interruptCounter--;
      //portEXIT_CRITICAL(&mux);

      //numberOfInterrupts++;
      Serial.print("An interrupt has occurred. Total: ");
      Serial.println(interruptCounter);
  //}
}
