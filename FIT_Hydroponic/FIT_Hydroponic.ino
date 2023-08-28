// Developer: Apirak Tooltham
// IoTES LAB, Nakhon Phanom University
// 21 May 2023
// Version 1.6

// Timer
#include <millisDelay.h>
millisDelay tempDelay;
millisDelay pumpDelay;

// Temperature Sensor DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
const int oneWireBus = 4;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

// Line Notify
#include <TridentTD_LineNotify.h>
String lineTemp;
// -------------- Line Notify --------------
#define SSID        "IoT-LAB"
#define PASSWORD    "1a2b3c4d5e"
#define LINE_TOKEN  "MOzwWOk18IvRssfXygic4fI4yQ4GAIT1pyTPGZ7citM"
// -----------------------------------------

// Relays (control 2 pumps)
int relay_1 = 26;
int relay_2 = 25;
boolean pumpState = false;

// Flow Switch
int flowSW_1 = 18;
int flowSW_2 = 19;
boolean flow_1;
boolean flow_2;

void setup() {
  // Start the Serial Monitor
  Serial.begin(115200);
  Serial.println();
  // Line Notify
  Serial.println(LINE.getVersion());
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n", SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
  LINE.setToken(LINE_TOKEN);
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
  // LINE.notify("Pump-2: ทำงาน");
  // pumpState = false;
  if (flow_2 == 0) {
    digitalWrite(relay_1, LOW);
    digitalWrite(relay_2, HIGH);
    LINE.notify("Pump-2: ทำงาน");
  } else {
    digitalWrite(relay_1, HIGH);
    digitalWrite(relay_2, LOW);
    LINE.notify("Pump-1: ทำงาน");
  }

  // Start delay
  tempDelay.start(2000);      // delay เวลาวัดอุณหภูมิ
  pumpDelay.start(10000);     // delay เวลาของ Pump
}

void checkTemp() {
  // check if delay has timed out
  if (tempDelay.justFinished()) {
    // Start delay again without drift
    tempDelay.repeat();
    sensors.requestTemperatures();
    float temperatureC = sensors.getTempCByIndex(0);
    String t1 = "Water Temp = ";
    String t2 = "°C";
    lineTemp = t1 + String(temperatureC) + t2;
    Serial.println(lineTemp);
    if (temperatureC > 30) {
      // Send notification to user by line !!!
      LINE.notify(lineTemp);
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
      delay(1000);
      checkFlow();
      if (flow_1 == 0) {
        digitalWrite(relay_1, HIGH);
        digitalWrite(relay_2, LOW);
        LINE.notify("Pump-1: ทำงาน");
      } else {
        digitalWrite(relay_1, LOW);
        digitalWrite(relay_2, HIGH);
        LINE.notify("Pump-2: ทำงาน");
      }
    } else {
      digitalWrite(relay_1, LOW);
      digitalWrite(relay_2, HIGH);
      delay(1000);
      checkFlow();
      if (flow_2 == 0) {
        digitalWrite(relay_1, LOW);
        digitalWrite(relay_2, HIGH);
        LINE.notify("Pump-2: ทำงาน");
      } else {
        digitalWrite(relay_1, HIGH);
        digitalWrite(relay_2, LOW);
        LINE.notify("Pump-1: ทำงาน");
      }
    }
  }
}

void loop() {
  checkTemp();
  pumpOperate();
}
