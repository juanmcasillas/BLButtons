// http://www.ignorantofthings.com/2018/07/the-perfect-multi-button-input-resistor.html

// #define RELEASE 1

#define BUTTONS 2
#define RESOLUTION 1023
#define ANALOG_PIN 33
#define DIGITAL_PIN 32

int readAnalogButton(unsigned int pin_num) {
  float avg = RESOLUTION / float(BUTTONS);
  int val = analogRead(pin_num);
  Serial.print("read: ");
  Serial.println(val);
  if (val > (BUTTONS - 0.5) * avg) { 
    return 0; 
  }

  for (int i = 0; i < BUTTONS; i++) {
    if (val < round((i + 0.5) * avg)) { 
      return i + 1; 
    }
  }

  return 0;
}


void setup() {
#ifndef RELEASE
    pinMode(ANALOG_PIN, INPUT);
    pinMode(DIGITAL_PIN, OUTPUT);
    Serial.begin(115200);
    Serial.println("Ladder Started");
    delay(1000);
    digitalWrite(DIGITAL_PIN,HIGH);
#endif
}

void loop() {
    int value = analogRead(ANALOG_PIN);
    Serial.print("Button pressed: ");
    Serial.println(value);
    delay(100);
}

