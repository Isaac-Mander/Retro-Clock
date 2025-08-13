#include "GPSModule.h"

// Constructor to initialize SoftwareSerial pins and baud rate
GPSModule::GPSModule(int rxPin, int txPin, long baud)
  : gpsSerial(rxPin, txPin), _baud(baud) {}

// Call this once in setup() to begin serial communication with GPS module
void GPSModule::begin() {
  gpsSerial.begin(_baud);
}

// Reads incoming GPS data, parses it, and returns a GPSData struct with current info
GPSData GPSModule::getGPSData() {
  GPSData result;
  result.valid = false;  // Default: no valid data

  // Check if we are being sent data
  while (gpsSerial.available()) {
    // Read the data
    char c = gpsSerial.read();
    
    // If the data is in the right format
    if (gps.encode(c)) {
      // Get positional data
      float lat = 0.0, lng = 0.0;
      unsigned long age_of_data;

      // f_get_position fills lat, lng, and sets age_of_data (how old fix is in ms)
      gps.f_get_position(&lat, &lng, &age_of_data);

      // Get time data
      int year;
      byte month, day, hour, minute, second, hundredths;

      // Extract the time information from the GPS data
      gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age_of_data);

      // If the position is exactly 0 then it implies the GPS is not working
      // Generally if the GPS is faulty it will just output 0
      // CLOCK WILL NOT WORK IF YOU HAPPEN TO BE AT EXACTLY 0 LAT OR 0 LONG
      bool validPosition = (lat != 0.0f || lng != 0.0f);

      // Check the time is something reasonable, this unfortunately kills using this clock while time traveling
      bool validDateTime = (year > 2000 && month >= 1 && month <= 12 && day >= 1 && day <= 31);

      // If everything is valid then we can return the data
      if (validPosition && validDateTime) {
        result.latitude = lat;
        result.longitude = lng;

        result.year = year;
        result.month = month;
        result.day = day;
        result.hour = hour;
        result.minute = minute;
        result.second = second;
        result.hundredths = hundredths;

        result.age_of_data = age_of_data;

        // Flag that the data can be trusted
        result.valid = true;
        return result;
      }
    }
  }
  // If we aren't receiving valid data, or any data at all then return an empty info struct
  return result;
}

// Convert a hex number to a char number
uint8_t hexCharToVal(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return 0;
}

// Calculate the current NMEA checksum from the string
uint8_t calculateNMEAChecksum(const char* sentence) {
  uint8_t checksum = 0;
  for (const char* p = sentence + 1; *p != '*' && *p != '\0'; ++p) {
    checksum ^= static_cast<uint8_t>(*p);
  }
  return checksum;
}

// Copy the expected checksum from the inputted string
uint8_t getSentenceChecksum(const char* sentence) {
  size_t len = strlen(sentence);
  if (len < 3) return 0;
  return (hexCharToVal(sentence[len - 4]) << 4) + hexCharToVal(sentence[len - 3]);
}

// Print the gps data in a human readable way
void printGPSData(TinyGPS &gps) {
  // Get positional info
  float lat, lon;
  unsigned long age;
  gps.f_get_position(&lat, &lon, &age);

  // Print pos info
  Serial.print("LAT=");
  Serial.print(lat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lat, 6);
  Serial.print(" LON=");
  Serial.print(lon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lon, 6);

  // Show if the data is valid or not
  unsigned long chars;
  unsigned short sentences, failed;
  gps.stats(&chars, &sentences, &failed);
  Serial.print(" CSUM ERR=");
  Serial.println(failed);
}

// Send a NMEA char to be decoded
void feedNMEA(const char* sentence, TinyGPS &gps) {
  Serial.print("Feeding NMEA: ");
  Serial.print(sentence);
  for (const char* p = sentence; *p != '\0'; ++p) {
    gps.encode(*p);
    delay(5);  // Simulate serial timing
  }
}

