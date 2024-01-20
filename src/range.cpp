#include <Arduino.h>

const unsigned long TIMEOUT_MICROS = 1000000;
const unsigned long ECHO_WAIT_MICROS = 60000;
const int MAX_DISTANCE = 60;

const int STATE_ERROR = -1;
const int STATE_READY = 0;
const int STATE_TRIGGERED = 1;
const int STATE_ECHO_STARTED = 2;
const int STATE_ECHO_ENDED = 3;

const PinName trigger_pin = digitalPinToPinName(PA0);
const PinName echo_pin = digitalPinToPinName(PA1);

volatile int state = STATE_READY;

volatile unsigned long time_triggered = 0;
volatile unsigned long time_echo_start = 0;
volatile unsigned long time_echo_end = 0;


int range;

void echo_change();

void setup()
{
  Serial.begin(9600);
  pinMode(pinNametoDigitalPin(trigger_pin), OUTPUT);
  pinMode(pinNametoDigitalPin(echo_pin), INPUT);
  attachInterrupt(digitalPinToInterrupt(pinNametoDigitalPin(echo_pin)), echo_change, CHANGE);
  Serial.println("started");
}

void loop()
{
  unsigned long time = micros();

  if (state != STATE_READY) {
    if (state == STATE_ERROR) {
      Serial.println("error detected. resetting state");
      state = STATE_READY;
      return;
    } else if (time < time_triggered) {
      Serial.println("overflow detected. resetting state");
      state = STATE_READY;
      return;
    } else if (time - time_triggered >= 10) {
      digitalWriteFast(trigger_pin, LOW);
    }
  }

  switch (state) {
  case STATE_READY:
    if (time - time_echo_end > ECHO_WAIT_MICROS) {
      state = STATE_TRIGGERED;
      digitalWriteFast(trigger_pin, HIGH);
      time_triggered = time;
    }
    break;
  case STATE_ECHO_ENDED:
    range = (int)((time_echo_end - time_echo_start) / 58);
    if (range > MAX_DISTANCE) {
      range = MAX_DISTANCE;
    }
    Serial.print("distance: ");
    Serial.print(range);
    Serial.println("cm");
    state = STATE_READY;
    break;
  default:
    if (time - time_triggered > TIMEOUT_MICROS) {
      Serial.println("timeout detected. resetting state");
      state = STATE_READY;
    }
  }
}

void echo_change()
{
  unsigned long time = micros();
  int value = digitalReadFast(echo_pin);
  if (value == HIGH) {
    time_echo_start = time;
    if (state != STATE_TRIGGERED) {
      state = STATE_ERROR;
    }
    state = STATE_ECHO_STARTED;
  }
  else {
    time_echo_end = time;
    if (state != STATE_ECHO_STARTED) {
      state = STATE_ERROR;
    }
    state = STATE_ECHO_ENDED;
  }
}