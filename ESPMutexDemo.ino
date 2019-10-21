#include "Arduino.h"
#include "ESP32Servo.h"
#include "analogWrite.h"
#include <soc/sens_reg.h>
#include <soc/sens_struct.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <stdint.h>
#include "esp_types.h"
#include "driver/adc.h"
//#include "soc/efuse_periph.h"
#include "esp_err.h"
#include "assert.h"
#include "esp_adc_cal.h"


double interruptCounter50Hz = 0;
double interruptCounter1kHz = 0;
double sinComp = 0;
double sinComp50Hz = 0;
double numSamples = 20;
double frequencyTarget = 1000;
double frequencyTarget2 = 50;
int count = (1.0 / (frequencyTarget / 1000000.0)) / numSamples;
double increment = (2 * PI) / numSamples;
double increment50Hz = increment / (frequencyTarget/frequencyTarget2);
// Pointer to the timer object
hw_timer_t *timer = NULL;
portMUX_TYPE DRAM_ATTR timerMux = portMUX_INITIALIZER_UNLOCKED;
TaskHandle_t complexHandlerTask;


void runDDS() {
	interruptCounter1kHz += increment;
	if (interruptCounter1kHz >= 2 * PI) {
		interruptCounter1kHz = 0;
	}
	interruptCounter50Hz += increment50Hz;
	if (interruptCounter50Hz >= 2 * PI) {
		interruptCounter50Hz = 0;
	}
	sinComp = (126.0  * sin(interruptCounter1kHz)) + 128.0 ;
	sinComp50Hz = (126.0 * sin(interruptCounter50Hz)) + 128.0;
	analogWrite(26, (uint8_t) ( sinComp));
	analogWrite(25, (uint8_t) (sinComp50Hz ));
}



void complexHandler(void *param) {
	while (true) {
		// Sleep until the ISR gives us something to do, or for 1 second
		ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000));
		runDDS();
	}
}
// Place the Interrupt handeler in IRAM section of memory
void IRAM_ATTR onTimer() {
	// A mutex protects the handler from reentry (which shouldn't happen, but just in case)
	portENTER_CRITICAL_ISR(&timerMux);

	// Notify complexHandlerTask that the buffer is full.
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	vTaskNotifyGiveFromISR(complexHandlerTask, &xHigherPriorityTaskWoken);
	if (xHigherPriorityTaskWoken) {
		portYIELD_FROM_ISR();
	}

	portEXIT_CRITICAL_ISR(&timerMux);

}

void setup() {
	xTaskCreate(complexHandler, "Handler Task", 8192, NULL, 1,
			&complexHandlerTask);
	Serial.begin(115200);
	Serial.println(
			"Start ESPMutexDemo " + String(count)
					+ " microsecond timer interrupts");
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
	delay(100);
}
