// Developer: Apirak Tooltham
// IoTES LAB, Nakhon Phanom University
// 21 May 2023
// Version 1.6

// Timer
#include "secrets.h"
#include <millisDelay.h>
millisDelay tempDelay;
millisDelay pumpDelay;

// Temperature Sensor DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
const uint8_t oneWireBus = 4;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

// Line Notify
#include <TridentTD_LineNotify.h>
const char* WIFI_NAME = WIFI_SSID;
const char* WIFI_PASS = WIFI_PASSWORD;

// Relays (control 2 pumps)
const uint8_t relay_1 = 26;
const uint8_t relay_2 = 25;
bool pumpState = false;

// Flow Switch
const uint8_t flowSW_1 = 18;
const uint8_t flowSW_2 = 19;
bool flow_1;
bool flow_2;

void setup() {
  // Start the Serial Monitor
  Serial.begin(115200);
  Serial.println();
  // Line Notify
  Serial.println(LINE.getVersion());
  WiFi.begin(WIFI_NAME, WIFI_PASS);
  Serial.printf("WiFi connecting to %s\n", WIFI_NAME);
  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 20) {
    Serial.print(".");
    delay(500);
    retryCount++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nWiFi connected\nIP : ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi Connection Failed! Will retry in background.");
  }

  LINE.setToken(LINE_NOTIFY_TOKEN);
  LINE.notify("FIT INNOVATION");

  pinMode(relay_1, OUTPUT);
  pinMode(relay_2, OUTPUT);
  pinMode(flowSW_1, INPUT_PULLUP);
  pinMode(flowSW_2, INPUT_PULLUP);

  // Start the DS18B20 sensor
  sensors.begin();

  // Initialize Pump
  digitalWrite(relay_1, LOW);
  digitalWrite(relay_2, HIGH);
  delay(1000);
  checkFlow();

  // Start delay
  tempDelay.start(2000);      // delay เวลาวัดอุณหภูมิ
  pumpDelay.start(600000);     // delay เวลาของ Pump
}

void checkTemp() {
  if (tempDelay.justFinished()) {
    tempDelay.repeat();
    sensors.requestTemperatures();
    float temperatureC = sensors.getTempCByIndex(0);
    
    char msg[64];
    if (temperatureC == -127.0) { // DEVICE_DISCONNECTED_C
      Serial.println("Sensor Error: DS18B20 Disconnected");
      LINE.notify("ERROR: เซ็นเซอร์อุณหภูมิหลอดหรือขัดข้อง!");
      return;
    }

    snprintf(msg, sizeof(msg), "Water Temp = %.2f °C", temperatureC);
    Serial.println(msg);

    if (temperatureC > 30.0) {
      LINE.notify(msg);
      LINE.notifySticker("Water so HOT !!!", 4, 274);
    }
  }
}

void checkFlow() {
  // Read state from Flow Switch
  flow_1 = digitalRead(flowSW_1);
  Serial.print("Flow Switch 1: ");
  Serial.println(flow_1);
  flow_2 = digitalRead(flowSW_2);
  Serial.print("Flow Switch 2: ");
  Serial.println(flow_2);
}

void pumpOperate() {
  // check if delay has timed out
  if (pumpDelay.justFinished()) {
    // Start delay again without drift
    pumpDelay.repeat();
    // toggle the pumps state
    pumpState = !pumpState;
    if (pumpState) {
      digitalWrite(relay_1, HIGH);
      digitalWrite(relay_2, LOW);
      delay(2000); // Grace period for flow to stabilize
      checkFlow();
      if (flow_1 == HIGH) { // Assume HIGH means NO FLOW for PULLUP
        LINE.notify("WARNING: ปั๊ม 1 ทำงานแต่ตรวจไม่พบการไหล!");
      } else {
        LINE.notify("Pump 1: ระบบไหลเวียนปกติ");
      }
    } else {
      digitalWrite(relay_1, LOW);
      digitalWrite(relay_2, HIGH);
      delay(2000);
      checkFlow();
      if (flow_2 == HIGH) {
        LINE.notify("WARNING: ปั๊ม 2 ทำงานแต่ตรวจไม่พบการไหล!");
      } else {
        LINE.notify("Pump 2: ระบบไหลเวียนปกติ");
      }
    }
  }
}

void checkWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    static unsigned long lastRetry = 0;
    if (millis() - lastRetry > 10000) { // Retry every 10 seconds
      Serial.println("WiFi disconnected! Attempting to reconnect...");
      WiFi.disconnect();
      WiFi.begin(WIFI_NAME, WIFI_PASS);
      lastRetry = millis();
    }
  }
}

void loop() {
  checkWiFi();
  checkTemp();
  pumpOperate();
}
