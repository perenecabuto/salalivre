// http://archive.wired.com/geekdad/2012/09/using-motion-detectors-with-an-arduino/

#include <AltSoftSerial.h>


String ID = "1D";
String SSID = "";
String PASS = "";

int SERVER_PORT = 5000;
String SERVER_ADDRESS = "192.168.23.104";
String healthcheckAction = "GET /healthcheck/" + ID;
String eventAction = "POST /room/" + ID + "/event";

AltSoftSerial wifi(8, 9); // RX, TX

int sensor = 2;
int light = 13;


void setup() {
    pinMode(sensor, INPUT);
    pinMode(light, OUTPUT);

    Serial.begin(115200);
    while(!connectWiFi()) {
      Serial.println("Retrying connect to network");
    }
    
    digitalWrite(light, HIGH);
}

void loop() {
    int sensorValue = digitalRead(sensor);
    if (sensorValue == HIGH) {
        registerEvent();
        digitalWrite(light, LOW);
        delay(500);
        digitalWrite(light, HIGH);
    } else {
        healthcheck();
    }
}


void healthcheck() {
    request(SERVER_ADDRESS, SERVER_PORT, healthcheckAction);
}

void registerEvent() {
    request(SERVER_ADDRESS, SERVER_PORT, eventAction);
}

void request(String host, int port, String action) {
    wifi.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port);
    delay(2000);
    if (wifi.find("Error")) {
        Serial.println("Connection error");
        return;
    }
 
    Serial.print("Requesting " + action + "... ");
    action = action + " HTTP/1.0\r\n\r\n";
    wifi.print("AT+CIPSEND=");
    delay(500);
    wifi.println(action.length());
    delay(500);
    wifi.println(action);
    delay(1000);
    //wifi.println("AT+CIPCLOSE");
    Serial.println("ok");
}

boolean connectWiFi() {
    wifi.begin(9600);

    Serial.print("Connecting... ");

    wifi.println("AT");
    if (!wifi.find("OK")) {
        Serial.println("error connecting module");
        return false;
    }

    wifi.println("AT+RST");
    wifi.println("AT+CWMODE=1");
    delay(500); //delay after mode change
    wifi.println("AT+CWJAP=\"" + SSID + "\",\"" + PASS + "\"");
    if (wifi.find("OK")) {
        Serial.println("error connecting AP");
        return false;
    }

    Serial.println("ok");
    return true;
}
