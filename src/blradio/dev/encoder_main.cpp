#include <Arduino.h>
#include <Ticker.h>
#include <HardwareSerial.h>
#include "rotencoder.h"

#define SERIAL_SPEED 57600
#define PIN_RX 27
#define PIN_TX 26 // not used XD
#define ROT5_PINA 16
#define ROT5_PINB 26

char line[128];
HardwareSerial DriverPort( 1 );
RotaryEncoder *encoder;

Ticker periodicTicker;
void timer_isr() {
    encoder->service();
}



//   avoid using pins with LEDs attached

void setup() {
  Serial.begin(921600);
  DriverPort.begin(SERIAL_SPEED, SERIAL_8N1, PIN_RX, -1 );
  delay(500);
  Serial.println("Rot test before soldering:");
  encoder = new RotaryEncoder(ROT5_PINB, ROT5_PINA, -1);
  periodicTicker.attach_ms(1, timer_isr);

}

void loop() {

    switch (encoder->getDirection()) {
            case RotaryEncoder::RIGHT:
                #ifndef RELEASE
                    memset(line, 0, 128);
                    sprintf(line,"encoder #%d CW (Right)", 1);
                    Serial.println(line);
                #endif
                break;
            case RotaryEncoder::LEFT:
                #ifndef RELEASE
                    memset(line, 0, 128);
                    sprintf(line,"encoder #%d CCW (Left)", 1);
                    Serial.println(line);
                #endif
                break;
        }

}