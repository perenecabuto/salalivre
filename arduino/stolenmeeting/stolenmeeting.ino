// http://archive.wired.com/geekdad/2012/09/using-motion-detectors-with-an-arduino/

#include <stdlib.h>
#include <SoftwareSerial.h>

String ID = "1D";
String SSID = "";
String PASS = "";

int SERVER_PORT = 5000;
String SERVER_ADDRESS = "192.168.23.104";
String healthcheckAction = "GET /healthcheck/" + ID + " HTTP/1.0\r\n\r\n\r\n";
String eventAction = "POST /room/" + ID + "/event HTTP/1.0\r\n\r\n\r\n";

SoftwareSerial wifi(10, 11); // RX, TX

int sensor = 2;
int light = 13;


void setup() {
    pinMode(sensor, INPUT);
    pinMode(light, OUTPUT);

    Serial.begin(115200);
    while (!connectWiFi()) {
        Serial.println("Retring to reconnect to WIFI");
    }
}

void loop() {
    digitalWrite(light, LOW);
    healthcheck();

    int sensorValue = digitalRead(sensor);

    if (sensorValue == HIGH) {
        registerEvent();
        digitalWrite(light, HIGH);
        delay(500);
        digitalWrite(light, LOW);
        delay(500);
    }
}


boolean connectWiFi() {
    wifi.begin(9600);

    Serial.print("Connecting... ");

    wifi.println("AT+RST");
    wifi.println("AT+CWMODE=1");
    delay(500); //delay after mode change
    wifi.println("AT+RST");
    wifi.println("AT+CWJAP=\"" + SSID + "\",\"" + PASS + "\"");

    if (wifi.find("ready")) {
        Serial.println("ok");
        return true;
    }

    Serial.println("error");
    return false;
}

void healthcheck() {
    request(SERVER_ADDRESS, SERVER_PORT, healthcheckAction);
}

void registerEvent() {
    request(SERVER_ADDRESS, SERVER_PORT, eventAction);
}

void request(String host, int port, String action) {
    String cmd = "AT+CIPSTART=\"TCP\",\"" + host + "\"," + port;
    wifi.println(cmd);
    delay(2000);

    if (wifi.find("Error")) {
        Serial.println("Connection error");
        return;
    }

    Serial.println("Requesting: " + action);
    wifi.print("AT+CIPSEND=");
    wifi.println(action.length());
    delay(2000);
    wifi.println(action);
}
