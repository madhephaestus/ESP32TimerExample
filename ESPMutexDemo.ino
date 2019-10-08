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
#include "soc/efuse_periph.h"
#include "esp_err.h"
#include "assert.h"
#include "esp_adc_cal.h"


double interruptCounter50Hz = 0;
double interruptCounter500Hz = 0;
double sinComp = 0;
double sinComp50Hz = 0;
double numSamples = 20;
double frequencyTarget = 400;
int count = (1.0 / (frequencyTarget / 1000000.0)) / numSamples;
double increment = (2 * PI) / numSamples;
double increment50Hz = increment / 10.0;
// Pointer to the timer object
hw_timer_t *timer = NULL;
portMUX_TYPE DRAM_ATTR timerMux = portMUX_INITIALIZER_UNLOCKED;
TaskHandle_t complexHandlerTask;
int lastAdc = 0;


int IRAM_ATTR local_adc1_read(int channel) {
	uint16_t adc_value;
	SENS.sar_meas_start1.sar1_en_pad = (1 << channel); // only one channel is selected
	while (SENS.sar_slave_addr1.meas_status != 0)
		;
	SENS.sar_meas_start1.meas1_start_sar = 0;
	SENS.sar_meas_start1.meas1_start_sar = 1;
	while (SENS.sar_meas_start1.meas1_done_sar == 0)
		;
	adc_value = SENS.sar_meas_start1.meas1_data_sar;
	return adc_value;
}

esp_err_t esp_adc_cal_get_voltage(adc_channel_t channel,
                                  const esp_adc_cal_characteristics_t *chars,
                                  uint32_t *voltage)
{

    *voltage = esp_adc_cal_raw_to_voltage((uint32_t)local_adc1_read(0), chars);
    return ESP_OK;
}


void runDDS(){
	interruptCounter500Hz += increment;
	if (interruptCounter500Hz >= 2 * PI) {
		interruptCounter500Hz = 0;
	}
	interruptCounter50Hz += increment50Hz;
	if (interruptCounter50Hz >= 2 * PI) {
		interruptCounter50Hz = 0;
	}
	sinComp = (126.0 / 3 * sin(interruptCounter500Hz)) + 128.0 / 3;
	sinComp50Hz = (126.0 / 3 * sin(interruptCounter50Hz)) + 128.0 / 3;
	analogWrite(26, (uint8_t) (sinComp50Hz + sinComp));
}

void studentCode(){
	int adc = local_adc1_read(0);

	if(digitalRead(0)!=0)
		analogWrite(25, (uint8_t) ((adc+lastAdc)>>4));
	else
		analogWrite(25, adc>>4);
	lastAdc=adc;
}


void complexHandler(void *param) {
	while (true) {
		// Sleep until the ISR gives us something to do, or for 1 second
		ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000));
		runDDS();
		studentCode();
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
	analogRead(36);
#define V_REF 1100  // ADC reference voltage

    // Configure ADC
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_11db);

    // Calculate ADC characteristics i.e. gain and offset factors
    esp_adc_cal_characteristics_t characteristics;
    esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, &characteristics);
    uint32_t voltage=0;
    // Read ADC and obtain result in mV
    uint32_t err = esp_adc_cal_get_voltage(ADC_CHANNEL_0, &characteristics,&voltage);
    printf("%d mV\n",voltage);
}

void loop() {

}
