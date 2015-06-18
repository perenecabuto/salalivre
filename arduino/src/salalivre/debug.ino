#include <AltSoftSerial.h>

AltSoftSerial wifi(8, 9); // RX, TX

void setup() {
  Serial.begin(9600);
  wifi.begin(9600);
}

void loop() {
     while (Serial.available() > 0) {
      wifi.write(Serial.read());
    }
    while (wifi.available() > 0) {
      Serial.write(wifi.read());
    } 
}
