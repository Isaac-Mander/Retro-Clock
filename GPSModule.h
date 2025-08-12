#ifndef GPSMODULE_H
#define GPSMODULE_H

#include <TinyGPS.h>
#include <SoftwareSerial.h>

// Struct holds all positional and time data from GPS module
struct GPSData {
  // Positional data
  float latitude;
  float longitude;

  // Time data
  int year;
  byte month;
  byte day;
  byte hour;
  byte minute;
  byte second;
  byte hundredths;

  // Age of data in milliseconds (how old this GPS fix info is)
  unsigned long age_of_data;

  // Flag for if the data can be trusted
  bool valid;
};

// Class to handle GPS module communication and data parsing
class GPSModule {
public:
  // Constructor: specify RX and TX pins, and baud rate (default 9600)
  GPSModule(int rxPin, int txPin, long baud = 9600);

  // Call in setup() to initialize serial communication with GPS module
  void begin();

  // Call repeatedly in loop() to get the latest GPS data
  GPSData getGPSData();

private:
  SoftwareSerial gpsSerial;
  TinyGPS gps;
  long _baud;
};

uint8_t hexCharToVal(char c);
uint8_t calculateNMEAChecksum(const char* sentence);
uint8_t getSentenceChecksum(const char* sentence);
void printGPSData(TinyGPS gps);


#endif
