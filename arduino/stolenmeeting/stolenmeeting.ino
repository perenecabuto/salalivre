// http://archive.wired.com/geekdad/2012/09/using-motion-detectors-with-an-arduino/

int sensor = 2;
int light = 13;

void setup() {
    pinMode(sensor, INPUT);
    pinMode(light, OUTPUT);
}

void loop () {
    digitalWrite(light, LOW);
    delay(1000);

    int sensorValue = digitalRead(sensor);

    if (sensorValue == HIGH) {
        digitalWrite(light, HIGH);
        delay(500);
        digitalWrite(light, LOW);
        delay(500);
    }
}
