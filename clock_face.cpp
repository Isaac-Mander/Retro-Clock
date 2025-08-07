#include <Arduino.h>
#include "clock_face.h"
#include "matrix.h"



ClockFace::ClockFace(
    const size_t* hourLut,
    const size_t* minuteLut,
    const size_t* pmLed,
    size_t minuteLeds,
    LedMatrix* ledMatrix
) {
    this->hourLut = hourLut;
    this->minuteLut = minuteLut;
    this->pmLed = pmLed;
    this->minuteLeds = minuteLeds;

    this->ledMatrix = ledMatrix;

    this->ledOnTime = 50;        // Default LED on duration in ms
    this->blankTime = 10;        // Default blank time between strobes
    this->lastOnTime = 0;

    this->switchedOff = false;
    this->currentlyDisplaying = HOURS;
}

void ClockFace::init() {
    ledMatrix->init();
}


void ClockFace::lightHour(uint8_t hour) {

    // Get col and row from 2d array
    size_t col = hourLut[hour];
    size_t row = hourLut[hour + 1];

    ledMatrix->turnOnLed(col, row);
}

void ClockFace::lightMinute(uint8_t minute) {
    size_t minuteIndex = (minute * minuteLeds) / 60;

    // Get col and row from 2d array
    size_t col = hourLut[minuteIndex];
    size_t row = hourLut[minuteIndex + 1];

    ledMatrix->turnOnLed(col, row);
}

void ClockFace::lightPm(bool isPm) {
    if (isPm) {
        size_t col = pmLed[0];
        size_t row = pmLed[1];
        ledMatrix->turnOnLed(col, row);
    }
}

// Lights hte given time on the clock LED matrix
// This function is intended to be called constantly from the main loop
void ClockFace::lightTime(uint8_t hour, uint8_t minute, bool isPm) {

    unsigned long offTime = lastOnTime + ledOnTime;
    if (millis() > offTime && !switchedOff) { // Switch LED off after offTime has elapsed
        ledMatrix->switchLed(ledMatrix->lastCol, ledMatrix->lastRow, LOW);
        switchedOff = true;
    } else if (millis() > offTime + blankTime) { // Turn next LED on after blankTime has elapsed

        // Cycle through displays hours, minutes, and the am/pm indicator
        switch (currentlyDisplaying) {
            case HOURS:
                lightMinute(minute);
                currentlyDisplaying = MINUTES;
                break;
            case MINUTES:
                lightPm(isPm);
                currentlyDisplaying = AMPM;
                break;
            case AMPM:
                lightHour(hour);
                currentlyDisplaying = HOURS;
                break;
        }

        lastOnTime = millis();
        switchedOff = false;
    }

}