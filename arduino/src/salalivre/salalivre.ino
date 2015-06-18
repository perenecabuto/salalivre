// http://archive.wired.com/geekdad/2012/09/using-motion-detectors-with-an-arduino/
// https://github.com/prampec/arduino-softtimer/blob/wiki/WhyHow.md
// https://nurdspace.nl/ESP8266
// http://contractorwolf.com/esp8266-wifi-arduino-micro
// http://www.instructables.com/id/ESP8266-Wifi-Temperature-Logger/

#include <AltSoftSerial.h>
#include <SoftTimer.h>

String ID = "1D";
String SSID = "";
String PASS = "";

int SERVER_PORT = 80;
String SERVER_ADDRESS = "10.70.3.81";
String SERVER_HOSTNAME = "salalivre.cloud.globoi.com";
String healthcheckAction = "GET /healthcheck/" + ID;
String eventAction = "POST /room/" + ID + "/event";

int sensor = 2;
int light = 13;
bool presenceDetected = false;

AltSoftSerial wifi(8, 9); // RX, TX

Task wifiTask(30000, sendStatus);
Task sensorTask(1200, detectPresence);
Task blinkTask(500, pblink);


void setup() {
    pinMode(sensor, INPUT);
    pinMode(light, OUTPUT);

    Serial.begin(115200);
    while(!connectWiFi()) {
      Serial.println("Retrying connect to network");
    }

    digitalWrite(light, HIGH);

    SoftTimer.add(&blinkTask);
    SoftTimer.add(&sensorTask);
    SoftTimer.add(&wifiTask);
}

boolean toggle = false;
void pblink(Task* me) {
    if (presenceDetected) {
      toggle = !toggle;
      digitalWrite(light, toggle ? LOW : HIGH);  
      Serial.print("blink ");
      Serial.println(toggle);
    }
}

void detectPresence(Task* me) {
    if (!presenceDetected) {
      presenceDetected = digitalRead(sensor) == HIGH;
    }

    Serial.println(presenceDetected ? "There is something here" : "No presence detected");
}

void sendStatus(Task* me) {
    if (presenceDetected) {
        registerEvent();
        presenceDetected = false;
    } else {
        healthcheck();
    }
}


void healthcheck() {
    request(SERVER_ADDRESS, SERVER_PORT, healthcheckAction, SERVER_HOSTNAME);
}

void registerEvent() {
    request(SERVER_ADDRESS, SERVER_PORT, eventAction, SERVER_HOSTNAME);
}


void request(String host, int port, String action, String hostName) {

    if (!hostName) {
      hostName = host;
    }

    wifi.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port);
    delay(2000);
    if (wifi.find("Error")) {
        Serial.println("Connection error");
        return;
    }
 
    Serial.print("Requesting " + action + "... ");
    action = action + " HTTP/1.0\r\nHost: " + hostName + "\r\n\r\n";
    wifi.print("AT+CIPSEND=");
    delay(100);
    wifi.println(action.length());
    delay(500);
    wifi.println(action);
    delay(1000);
    wifi.println("AT+CIPCLOSE");
    Serial.println("ok");

    wifi.flush();
}

boolean connectWiFi() {    
    Serial.print("Connecting... ");
    wifi.begin(9600);

    wifi.println("AT");
    if (!wifi.find("OK")) {
        Serial.println("error connecting module");
        return false;
    }
    
    wifi.println("AT+CIPSTATUS");
    if (wifi.find("STATUS:3") || wifi.find("STATUS:5")) {
        Serial.println("already connected");
        return true;
    }

    wifi.println("AT+RST");
    wifi.println("AT+CWMODE=1");
    delay(1000); //delay after mode change
    wifi.println("AT+CWJAP=\"" + SSID + "\",\"" + PASS + "\"");
    delay(1000);
    if (!wifi.find("OK")) {
        Serial.println("error connecting AP");
        return false;
    }

    Serial.println("ok");
    return true;
}
