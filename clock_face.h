
#ifndef CLOCK_FACE_H 
#define CLOCK_FACE_H 

#include <Arduino.h>
#include "matrix.h"

typedef enum {
    HOURS,
    MINUTES,
    AMPM,
} CurrentlyDisplaying;

class ClockFace {
    private:
        LedMatrix* ledMatrix;

        // Matrix LED LUTS
        // Expected 2D array of columns then rows 
        const size_t* hourLut; 
        const size_t* minuteLut; 
        const size_t* pmLed;

        size_t minuteLeds;

        unsigned long lastOnTime; // The time in ms when the last LED was switched on
        bool switchedOff;
        CurrentlyDisplaying currentlyDisplaying;

        void lightHour(uint8_t hour);
        void lightMinute(uint8_t minute);
        void lightPm(bool isPm);

    public:
        uint8_t ledOnTime; // How many ms an LED stays on for when strobing matrix
        uint8_t blankTime; // How many ms all LEDs stay off inbetween lighting LEDs during strobe

        ClockFace(const size_t* hourLut, const size_t* minuteLut, const size_t* pmLed, size_t minuteLeds, LedMatrix* LedMatrix);
        void init();
        void lightTime(uint8_t hour, uint8_t minute, bool isPm);
        
};

#endif