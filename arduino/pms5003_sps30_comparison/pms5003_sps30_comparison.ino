#include "Adafruit_PM25AQI.h"
#include "SensirionI2cSps30.h"
#include "Wire.h"

const int PMS_SET_PIN = 2;
const int PMS_RESET_PIN = 3;
const uint8_t SPS30_I2C_ADDRESS = 0x69;

Adafruit_PM25AQI pms5003;
SensirionI2cSps30 sps30;

void setup() {
  Serial.begin(115200);

  setupPms5003();
  setupSps30();
}

void loop() {
  readPms5003();
  delay(2500);

  readSps30();
  delay(2000);
}

void setupPms5003() {
  Serial1.begin(9600);
  Serial.println("PMS5003 sensor test");

  pinMode(PMS_SET_PIN, OUTPUT);
  pinMode(PMS_RESET_PIN, OUTPUT);
  digitalWrite(PMS_SET_PIN, HIGH);
  digitalWrite(PMS_RESET_PIN, HIGH);

  delay(3000);

  if (!pms5003.begin_UART(&Serial1)) {
    Serial.println("Could not find PMS5003 sensor.");
    while (true) {
      delay(10);
    }
  }

  Serial.println("Waiting 10 seconds for PMS5003 stabilization...");
  delay(10000);
  Serial.println("PMS5003 ready.");
}

void setupSps30() {
  Wire.begin();
  Serial.println("SPS30 sensor test");

  sps30.begin(Wire, SPS30_I2C_ADDRESS);
  sps30.startMeasurement(SPS30_OUTPUT_FORMAT_OUTPUT_FORMAT_FLOAT);

  Serial.println("Waiting 10 seconds for SPS30 stabilization...");
  delay(10000);
  Serial.println("SPS30 ready.");
}

void readPms5003() {
  PM25_AQI_Data data;

  if (pms5003.read(&data)) {
    Serial.println("PMS5003 mass concentration:");
    Serial.printf("PM1.0: %d ug/m3\n", data.pm10_standard);
    Serial.printf("PM2.5: %d ug/m3\n", data.pm25_standard);
    Serial.printf("PM10 : %d ug/m3\n", data.pm100_standard);
    Serial.println("---");
  }
}

void readSps30() {
  float mc1p0;
  float mc2p5;
  float mc4p0;
  float mc10p0;
  float nc0p5;
  float nc1p0;
  float nc2p5;
  float nc4p0;
  float nc10p0;
  float typicalParticleSize;

  sps30.readMeasurementValuesFloat(
    mc1p0,
    mc2p5,
    mc4p0,
    mc10p0,
    nc0p5,
    nc1p0,
    nc2p5,
    nc4p0,
    nc10p0,
    typicalParticleSize
  );

  Serial.println("SPS30 mass concentration:");
  Serial.printf("PM1.0: %.2f ug/m3\n", mc1p0);
  Serial.printf("PM2.5: %.2f ug/m3\n", mc2p5);
  Serial.printf("PM4.0: %.2f ug/m3\n", mc4p0);
  Serial.printf("PM10 : %.2f ug/m3\n", mc10p0);

  Serial.println("SPS30 number concentration:");
  Serial.printf("0.5 um: %.2f\n", nc0p5);
  Serial.printf("1.0 um: %.2f\n", nc1p0);
  Serial.printf("2.5 um: %.2f\n", nc2p5);
  Serial.printf("4.0 um: %.2f\n", nc4p0);
  Serial.printf("10 um : %.2f\n", nc10p0);
  Serial.printf("Typical particle size: %.2f nm\n", typicalParticleSize);
  Serial.println("---");
}
