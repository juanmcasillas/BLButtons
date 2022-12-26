#include <Arduino.h>
#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <ESP32Encoder.h>
#include <HardwareSerial.h>

#define SERIAL_SPEED 57600
#define PIN_RX 27
#define PIN_TX 26 // not used XD

HardwareSerial DriverPort( 1 );
ESP32Encoder encoder;
//   avoid using pins with LEDs attached

void setup() {
  Serial.begin(921600);
  DriverPort.begin(SERIAL_SPEED, SERIAL_8N1, PIN_RX, -1 );

  Serial.println("Basic NoInterrupts Test:");
  ESP32Encoder::useInternalWeakPullResistors=UP;
  encoder.attachHalfQuad(26, 16);
}

long position  = -999;

void loop() {
  Serial.println("Encoder count = " + String((int32_t)encoder.getCount()));
  delay(1);
}