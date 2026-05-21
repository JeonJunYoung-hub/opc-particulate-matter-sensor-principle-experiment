#include "Adafruit_PM25AQI.h"

const int SET_PIN = 2;
const int RESET_PIN = 3;

Adafruit_PM25AQI pms5003;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
  Serial.println("PMS5003 UART baseline test");

  pinMode(SET_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(SET_PIN, HIGH);
  digitalWrite(RESET_PIN, HIGH);

  delay(3000);

  if (!pms5003.begin_UART(&Serial1)) {
    Serial.println("Could not find PMS5003 sensor.");
    while (true) {
      delay(10);
    }
  }

  Serial.println("Waiting for PMS5003 to stabilize...");
  delay(20000);
  Serial.println("PMS5003 ready.");
}

void loop() {
  PM25_AQI_Data data;

  if (pms5003.read(&data)) {
    Serial.println("PMS5003 air quality data:");
    Serial.printf("PM1.0: %d ug/m3\n", data.pm10_standard);
    Serial.printf("PM2.5: %d ug/m3\n", data.pm25_standard);
    Serial.printf("PM10 : %d ug/m3\n", data.pm100_standard);
    Serial.printf("NC > 0.3 um: %d\n", data.particles_03um);
    Serial.printf("NC > 0.5 um: %d\n", data.particles_05um);
    Serial.printf("NC > 1.0 um: %d\n", data.particles_10um);
    Serial.printf("NC > 2.5 um: %d\n", data.particles_25um);
    Serial.printf("NC > 10 um : %d\n", data.particles_100um);
    Serial.println("---");
  }

  delay(2500);
}
