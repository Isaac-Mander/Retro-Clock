#include "matrix.h"
#include <Arduino.h>

LedMatrix::LedMatrix(
    const uint8_t* rowPins,
    const uint8_t* colPins,
    size_t matrixRows,
    size_t matrixCols,
    DiodeDirecton diodedirection
) {
    this->rowPins = rowPins;
    this->colPins = colPins;
    this->matrixCols = matrixCols;
    this->matrixRows = matrixRows;

    this->diodeDirection = diodeDirection;

    this->lastCol = 0;
    this->lastRow = 0;
}

static void initPinArray(const uint8_t* pins, size_t n, uint8_t mode) {
    for (int i = 0; i < n; i++) {
        pinMode(pins[i], mode);
    }
}

static void writePins(const uint8_t* pins, size_t n, bool state) {
    for (int i = 0; i < n; i++) {
        digitalWrite(pins[i], state);
    }
}

// Set row and column pins so no LEDs are on
void LedMatrix::turnOffMatrix() {
    bool colState = diodeDirection; // Colstate is LOW if COL2ROW, HIGH if ROW2COL
    bool rowState = !colState;
    writePins(colPins, matrixCols, colState);
    writePins(rowPins, matrixRows, rowState);
}

void LedMatrix::init() {
    initPinArray(rowPins, matrixRows, OUTPUT);
    initPinArray(colPins, matrixCols, OUTPUT);

    turnOffMatrix();
}

// Turns on/off the LED at (col,row) in the matrix
void LedMatrix::switchLed(size_t col, size_t row, bool state) {
    int colPin = colPins[col];
    int rowPin = rowPins[row];

    bool colState = !diodeDirection; // Colstate is HIGH if COL2ROW, LOW if ROW2COL

    // Invert state if we want to turn an LED off not on
    if (state == LOW) {
        colState = !colState;
    } else {
        lastCol = col;
        lastRow = row;
    }

    bool rowState = !colState;

    digitalWrite(colPin, colState);
    digitalWrite(rowPin, rowState);
}

// Turns off the last LED then turns on the LED at (col,row) in the matrix
void LedMatrix::turnOnLed(size_t col, size_t row) {
    switchLed(lastCol, lastRow, LOW);
    switchLed(col, row, HIGH);
}

