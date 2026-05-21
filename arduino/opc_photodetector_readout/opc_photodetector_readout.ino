#include <algorithm>

// Pin configuration for a Teensy-style board.
const int MCLK_PIN = 33;
const int M_RESET = 34;
const int M_GAIN = 35;

const int TRIGGER_PIN = 36;
const int LEOS_PIN = 37;
const int VIDEO_PIN = A14;

const int NUM_PIXELS = 64;
const int MAX_PEAKS = 10;

volatile int pixelIndex = 0;
volatile bool lineReady = false;

float pixelData[NUM_PIXELS];
float corrected[NUM_PIXELS];

struct Peak {
  int index;
  float height;
  float fwhm;
};

Peak peakList[MAX_PEAKS];
int peakCount = 0;

int detectMultiplePeaks(float *data, int count, Peak *list, int maxPeaks) {
  float sorted[NUM_PIXELS];
  for (int i = 0; i < count; i++) {
    sorted[i] = data[i];
  }

  std::sort(sorted, sorted + count);
  float baseline = 0.5f * (sorted[(count / 2) - 1] + sorted[count / 2]);
  Serial.printf("Baseline: %.3f V\n", baseline);

  for (int i = 0; i < count; i++) {
    corrected[i] = data[i] - baseline;
  }

  const float threshold = 0.003f;
  int detected = 0;

  for (int i = 2; i < count - 2; i++) {
    float localBase = (
      corrected[i - 2] + corrected[i - 1] +
      corrected[i + 1] + corrected[i + 2]
    ) * 0.25f;

    bool isLocalPeak = corrected[i] > corrected[i - 1] &&
                       corrected[i] > corrected[i + 1];
    bool isAboveNoise = corrected[i] > localBase + threshold;

    if (isLocalPeak && isAboveNoise) {
      list[detected].index = i;
      list[detected].height = corrected[i];
      list[detected].fwhm = 0.0f;

      detected++;
      if (detected >= maxPeaks) {
        break;
      }
    }
  }

  return detected;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting OPC photodetector array readout");

  analogReadResolution(12);
  analogWriteFrequency(MCLK_PIN, 200000);
  analogWrite(MCLK_PIN, 128);

  pinMode(M_RESET, OUTPUT);
  pinMode(M_GAIN, OUTPUT);
  digitalWrite(M_GAIN, HIGH);

  pinMode(TRIGGER_PIN, INPUT);
  pinMode(LEOS_PIN, INPUT);
  pinMode(VIDEO_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), onTrigger, RISING);
  attachInterrupt(digitalPinToInterrupt(LEOS_PIN), onEndOfScan, FALLING);

  sendResetPulse();
}

void loop() {
  if (!lineReady) {
    return;
  }

  lineReady = false;
  peakCount = detectMultiplePeaks(pixelData, NUM_PIXELS, peakList, MAX_PEAKS);

  Serial.printf("Peak count: %d\n", peakCount);
  Serial.println("=== Line Data ===");
  for (int i = 0; i < NUM_PIXELS; i++) {
    Serial.printf(
      "Pixel %02d | raw: %8.4f V | corrected: %8.4f V\n",
      i + 1,
      pixelData[i],
      corrected[i]
    );
  }

  Serial.println("---- Peak Results ----");
  for (int i = 0; i < peakCount; i++) {
    Serial.printf(
      "Peak %d @ pixel %d | height: %.4f V\n",
      i + 1,
      peakList[i].index + 1,
      peakList[i].height
    );
  }

  if (peakCount == 0) {
    Serial.println("No peaks detected.");
  } else {
    printPeakHistogram();
  }

  sendResetPulse();
  delay(1000);
}

void printPeakHistogram() {
  float maxPeak = 0.0f;
  for (int i = 0; i < peakCount; i++) {
    if (peakList[i].height > maxPeak) {
      maxPeak = peakList[i].height;
    }
  }

  int bin100 = 0;
  int bin80 = 0;
  int bin50 = 0;
  int bin20 = 0;

  for (int i = 0; i < peakCount; i++) {
    float pct = (peakList[i].height / maxPeak) * 100.0f;

    if (pct >= 80.0f) {
      bin100++;
    } else if (pct >= 50.0f) {
      bin80++;
    } else if (pct >= 20.0f) {
      bin50++;
    } else {
      bin20++;
    }
  }

  Serial.println("---- Relative Peak Histogram ----");
  Serial.printf("100-80%% bin: %d\n", bin100);
  Serial.printf("80-50%%  bin: %d\n", bin80);
  Serial.printf("50-20%%  bin: %d\n", bin50);
  Serial.printf("20-0%%   bin: %d\n", bin20);
}

void onTrigger() {
  if (pixelIndex < NUM_PIXELS) {
    int adcValue = analogRead(VIDEO_PIN);
    pixelData[pixelIndex] = (adcValue / 4095.0f) * 3.3f;
    pixelIndex++;
  }
}

void onEndOfScan() {
  lineReady = true;
  pixelIndex = 0;
}

void sendResetPulse() {
  digitalWrite(M_RESET, HIGH);
  delayMicroseconds(105);
  digitalWrite(M_RESET, LOW);
  delayMicroseconds(100);
}
