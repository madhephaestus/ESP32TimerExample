#include "Arduino.h"

volatile int interruptCounter;
volatile int totalInterruptCounter;

// Pointer to the timer object
hw_timer_t * timer = NULL;
// Mutex object used for protecting variables
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// Place the Interrupt handeler in IRAM section of memory
void IRAM_ATTR onTimer() {
	// Lock the mutex
	//portENTER_CRITICAL_ISR(&timerMux);
	// modify the global variable in the System process
	interruptCounter++;
	// release the lock
	//portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {

	Serial.begin(115200);
	Serial.println("Start ESPMutexDemo");
	// get a pointer to a timer to use
	timer = timerBegin(3, // Timer 3
			80, // Divider from the system clock to get to us
			true); // Count up
	timerAttachInterrupt(timer, // Timer to attach the interrupt
			&onTimer, // Interrupt handeler passed to timer
			true); // Rising edge of timer
	timerAlarmWrite(timer, // The timer object to use
			10, // count, now now in us
			true); // Reload after finishes, run again and again
	timerAlarmEnable(timer); // Enable timer

}

void loop() {
	// take the lock in the user code
	//portENTER_CRITICAL(&timerMux);
	// reset the latch, modify the global memory in user process
	interruptCounter--;
	// release the lock
	//portEXIT_CRITICAL(&timerMux);
}
