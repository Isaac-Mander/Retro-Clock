#include <Arduino.h>
#include "matrix.h"
#include "clock_face.h"
#include "GPSModule.h"



const size_t MATRIX_COLS = 5;
const size_t MATRIX_ROWS = 5;
const uint8_t COL_PINS[MATRIX_COLS] = {7, 6, 8, 9, 10};
const uint8_t ROW_PINS[MATRIX_ROWS] = {1, 11, 2, 0, 12};
const DiodeDirecton DIODE_DIRECTION = COL2ROW;

const size_t MINUTE_LEDS = 12;

// Look up table for hour LEDs on the matrix
// Intended to be indexed by the hour
const size_t HOUR_LUT[24] = {
//C, R
  0, 2,  // 1
  1, 2,  // 2
  0, 0,  // 3
  1, 0,  // ...
  2, 0,
  3, 0,
  4, 0,
  0, 1,
  1, 1,
  2, 1,
  3, 1,
  4, 1   // 12
};


// Look up table for minute LEDs on the matrix
// Intended to be indexed by how far through the hour we are
// E.g. 30 minutes = 0.5 hours, 12 LEDs * 0.5 hours = 6 (index)
const size_t MINUTE_LUT[MINUTE_LEDS * 2] = {
//C, R
  2, 2,
  3, 2,
  4, 2,
  0, 3,
  1, 3,
  2, 3,
  3, 3,
  4, 3,
  0, 4,
  1, 4,
  2, 4,
  3, 4
};

const size_t AM_PM_LED[2] = {4, 4};


LedMatrix ledMatrix(ROW_PINS, COL_PINS, MATRIX_ROWS, MATRIX_COLS, COL2ROW);
ClockFace clockFace(HOUR_LUT, MINUTE_LUT, AM_PM_LED, MINUTE_LEDS, &ledMatrix);



//Create a gps object
TinyGPS gps;


const char *nmeaSentence = "$GPGGA,082311.00,3751.65,S,14507.36,E,1,08,1.0,10.0,M,0.0,M,,*71\r\n";

unsigned long lastActionTime = 0;
const unsigned long actionInterval = 3000; // 3 seconds


void setup() {
  // Serial.begin(115200); // Using serial will activate some random LEDs on the matrix
  // clockFace.init();
  Serial.begin(115200);
  delay(2000);  // wait for Serial ready

  Serial.println("Starting GPS feed test...");

  uint8_t calculated = calculateNMEAChecksum(nmeaSentence);
  uint8_t sent = getSentenceChecksum(nmeaSentence);

  Serial.print("Calculated checksum: 0x");
  if (calculated < 16) Serial.print('0');
  Serial.println(calculated, HEX);

  Serial.print("Sent checksum:       0x");
  if (sent < 16) Serial.print('0');
  Serial.println(sent, HEX);

  if (calculated == sent) {
    Serial.println("Checksum MATCHES");
  } else {
    Serial.println("Checksum DOES NOT match");
  }
}

void loop() {
  unsigned long now = millis();
  if (now - lastActionTime >= actionInterval) {
    feedNMEA(nmeaSentence, gps);
    printGPSData(gps);
    lastActionTime = now;
  }
  //Print the current gps data for debug
  // printGPSData(gps);
}
