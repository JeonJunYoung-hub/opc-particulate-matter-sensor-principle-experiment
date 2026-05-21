# OPC Particulate Matter Sensor Principle Experiment

Arduino/Teensy experiments for studying particulate matter sensing principles with optical particle counter style readout and reference PM sensor modules.

This repository keeps only code and public-facing notes. Vendor datasheets, training decks, and local experiment documents are intentionally excluded.

## Experiments

- `arduino/opc_photodetector_readout/`  
  Reads a Hamamatsu C9118-style driver and S11865-64G-style 64-pixel photodiode array, applies baseline correction, detects peaks, and bins relative peak heights as an OPC principle experiment.

- `arduino/pms5003_uart_baseline/`  
  Baseline UART test for a Plantower PMS5003 particulate matter sensor.

- `arduino/pms5003_sps30_comparison/`  
  Side-by-side serial output from PMS5003 and Sensirion SPS30 for comparing PM mass concentration and particle count style outputs.

## Hardware Notes

The sketches assume an Arduino-compatible board with:

- Hardware serial for PMS5003, commonly `Serial1`
- I2C for SPS30
- Teensy-style PWM/ADC behavior for the photodetector array experiment
- 3.3 V logic where required by the connected modules

Pin assignments are documented at the top of each sketch.

## Dependencies

Install through Arduino Library Manager or vendor packages:

- `Adafruit PM25 AQI Sensor`
- `Sensirion I2C SPS30`

The OPC readout sketch uses Teensy APIs such as `analogWriteFrequency` and `Serial.printf`.

## Why This Exists

The goal is to understand the measurement path behind low-cost PM sensing:

1. Optical signal acquisition from a photodetector array
2. Baseline correction and peak detection
3. Simple peak-height binning for particle-size intuition
4. Comparison with packaged PM sensors such as PMS5003 and SPS30

## Safety

Use proper eye-safety controls for any laser or focused light source. The code is an educational experiment and does not replace calibrated aerosol instrumentation.
