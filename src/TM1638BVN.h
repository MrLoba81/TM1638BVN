/*
TM1638BVN.h - Library for TM1638BVN.

Copyright (C) 2022 Paolo Loberto - MrLoba81
https://github.com/MrLoba81/TM1638BVN

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef TM1638BVN_h
#define TM1638BVN_h

#include "Arduino.h"
#include "TM16XXFonts.h"

#define SCRL_BASE_DEFAULT (int8_t)2

enum Display
{
    UPPER,
    LOWER
};

class TM1638BVN
{
public:
    TM1638BVN(byte dataPin, byte clockPin, byte strobePin);
    void setStatusBar(uint8_t status);
    void setPercentage(uint8_t percentage);
    void clearDisplay();
    void clearDisplay(Display display);
    void printString(const char *string, Display display, uint8_t dot = 0);
    void printNumber(uint16_t number, Display display, uint8_t dot = 0);
    void setScrollingString(const char *string, uint8_t len, Display display);
    void clearScrolling();
    void loop();
private:
    char upperStr[5];
    char lowerStr[5];
    uint8_t dotUpper;
    uint8_t dotLower;
    const char *scrlString;
    Display scrlDisplay;
    int8_t scrlBase;
    boolean scrollActive;
    uint8_t scrlLen;
    uint8_t percentage;
    unsigned long millisCheck;

    template <typename T>
    void print(T data, Display display, const char *formatString, uint8_t dot);
    void _setPercentage(boolean blink);
    /****
     * Code grabbed from TM16XX.h - Library for TM1638.
     * Copyright (C) 2011 Ricardo Batista <rjbatista at gmail dot com>
     * https://github.com/rjbatista/tm1638-library
     ****/
    byte displays;
    byte dataPin;
    byte clockPin;
    byte strobePin;
    void setDisplayToString(const char *string, const word dots = 0, const byte pos = 0,
                            const byte font[] = FONT_DEFAULT);
    void sendCommand(byte cmd);
    void sendData(byte address, byte data);
    void send(byte data);
    void setDisplay(const byte values[], uint8_t length);
};
#endif
