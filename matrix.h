#ifndef MATRIX_H
#define MATRIX_H

#include <Arduino.h>

typedef enum {
    COL2ROW = 0,
    ROW2COL = 1,
} DiodeDirecton;

class LedMatrix {
    private:
        const uint8_t* rowPins;
        const uint8_t* colPins;
        
        // Size of the arrays above
        size_t matrixRows;
        size_t matrixCols;

        DiodeDirecton diodeDirection;

        void turnOffMatrix();
    public:
        // Index of the LED that is currently on for the row and column pins
        size_t lastRow;
        size_t lastCol;

        LedMatrix(const uint8_t* rowPins, const uint8_t* colPins, size_t matrixRows, size_t matrixCols, DiodeDirecton diodeDirecton);
        void init();
        void switchLed(size_t col, size_t row, bool state);
        void turnOnLed(size_t col, size_t row);
};

#endif