/*
TM1638BVN.cpp - Library implementation for TM1638BVN.

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
#include "TM1638BVN.h"

TM1638BVN::TM1638BVN(byte dataPin, byte clockPin, byte strobePin)
{
    memset(this->upperStr, ' ', 4);
    memset(this->lowerStr, ' ', 4);
    this->upperStr[4] = '\0';
    this->lowerStr[4] = '\0';
    this->dotLower = 0;
    this->dotUpper = 0;
    this->percentage = 0;
    this->millisCheck = 0;
    this->scrollActive = false;
    this->scrlBase = -SCRL_BASE_DEFAULT;

    this->dataPin = dataPin;
    this->clockPin = clockPin;
    this->strobePin = strobePin;
    this->displays = 8;

    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(strobePin, OUTPUT);

    digitalWrite(strobePin, HIGH);
    digitalWrite(clockPin, HIGH);

    sendCommand(0x40);
    sendCommand(0x80 | 8 | 0);

    digitalWrite(strobePin, LOW);
    send(0xC0);
    for (int i = 0; i < 16; i++)
    {
        send(0x00);
    }
    digitalWrite(strobePin, HIGH);
}

void TM1638BVN::printString(const char *string, Display display, uint8_t dot)
{
    this->print(string, display, "%-4s", dot);
}

void TM1638BVN::printNumber(uint16_t number, Display display, uint8_t dot)
{
    this->print(number, display, "%4d", dot);
}

void TM1638BVN::setScrollingString(const char *string, uint8_t len, Display display)
{
    this->scrlDisplay = display;
    this->scrlString = string;
    this->scrollActive = true;
    this->scrlLen = len;
}

void TM1638BVN::clearScrolling()
{
    this->scrlString = nullptr;
    this->scrollActive = false;
    this->scrlLen = 0;
}

void TM1638BVN::loop()
{
    static boolean blinkPercentage = false;

    unsigned long curMillis = millis();
    if (curMillis - this->millisCheck >= 500)
    {
        this->millisCheck = curMillis;

        // Scroll display
        if (this->scrollActive)
        {
            const char *pos = this->scrlString + this->scrlBase + SCRL_BASE_DEFAULT;
            if (pos >= this->scrlString && pos + SCRL_BASE_DEFAULT < this->scrlString + this->scrlLen)
            {
                this->printString(pos, this->scrlDisplay);
            }
            else
            {
                this->clearDisplay(this->scrlDisplay);
            }

            this->scrlBase++;

            if (this->scrlBase == this->scrlLen - SCRL_BASE_DEFAULT)
            {
                this->scrlBase = -SCRL_BASE_DEFAULT;
            }
        }

        // update the percentage bar
        if ((this->percentage >= 12 && this->percentage <= 24) ||
            (this->percentage >= 36 && this->percentage <= 48) ||
            (this->percentage >= 60 && this->percentage <= 72) ||
            (this->percentage > 84 && this->percentage <= 99))
        {
            blinkPercentage = !blinkPercentage;
            this->_setPercentage(blinkPercentage);
        }
        else
        {
            blinkPercentage = false;
            this->_setPercentage(false);
        }
    }
}

template <typename T>
void TM1638BVN::print(T data, Display display, const char *formatString, uint8_t dot)
{
    if (display == UPPER)
    {
        snprintf(this->upperStr, 5, formatString, data);
        this->upperStr[4] = '\0';
        this->dotUpper = dot;
    }
    else
    {
        snprintf(this->lowerStr, 5, formatString, data);
        this->lowerStr[4] = '\0';
        this->dotLower = dot;
    }

    String _strDisplay = String(this->upperStr);
    _strDisplay.concat(this->lowerStr);

    this->setDisplayToString(_strDisplay.c_str(), (dotUpper << 4) | dotLower);
}

void TM1638BVN::setStatusBar(uint8_t status)
{
    // Starts from 0x09 GRID5 - SEG9 segment E
    for (uint8_t i = 0; i < 4; i++)
    {
        uint16_t addr = 0x09 + (i * 2);
        sendData(addr, addr == (uint16_t)(0x09 + (status * 2)) ? 1 : 0);
    }
}

void TM1638BVN::setPercentage(uint8_t percentage)
{
    this->percentage = percentage;
}

void TM1638BVN::_setPercentage(boolean blink)
{
    byte levels[] = {0x00, 0x01, 0x03, 0x07, 0x0F};
    byte data;
    byte adr = 0x07;

    uint8_t level = (this->percentage / 25) % 5;
    if (blink)
        level = (level + 1) % 5;

    data = levels[level];

    for (uint8_t j = 0; j < 4; j++)
    {
        sendData(adr, data & 0x01);
        data = data >> 1;
        adr -= 2;
    }
}

void TM1638BVN::clearDisplay(Display display)
{
    if (display == UPPER)
    {
        memset(this->upperStr, ' ', 4);
        this->upperStr[4] = '\0';
        this->dotUpper = 0;
    }
    else
    {
        memset(this->lowerStr, ' ', 4);
        this->lowerStr[4] = '\0';
        this->dotLower = 0;
    }

    String _strDisplay = String(this->upperStr);
    _strDisplay.concat(this->lowerStr);

    this->setDisplayToString(_strDisplay.c_str(), 0);
}

void TM1638BVN::clearDisplay()
{
    sendCommand(0x40); // set auto increment mode

    digitalWrite(this->strobePin, LOW);
    shiftOut(this->dataPin, this->clockPin, LSBFIRST, 0xc0); // set starting address to 0
    for (uint8_t i = 0; i < 16; i++)
    {
        shiftOut(this->dataPin, this->clockPin, LSBFIRST, 0x00);
    }
    digitalWrite(this->strobePin, HIGH);
}

/****
 * Code grabbed from TM16XX.cpp - Library implementation for TM16XX.
 * Copyright (C) 2011 Ricardo Batista (rjbatista <at> gmail <dot> com)
 * https://github.com/rjbatista/tm1638-library
 ****/
void TM1638BVN::setDisplayToString(const char *string, const word dots, const byte ignored, const byte font[])
{
    byte values[displays];
    boolean done = false;

    memset(values, 0, displays * sizeof(byte));

    for (int i = 0; i < displays; i++)
    {
        if (!done && string[i] != '\0')
        {
            values[i] = font[string[i] - 32] | (((dots >> (displays - i - 1)) & 1) << 7);
        }
        else
        {
            done = true;
            values[i] = (((dots >> (displays - i - 1)) & 1) << 7);
        }
    }

    setDisplay(values, displays);
}

void TM1638BVN::sendCommand(byte cmd)
{
    digitalWrite(strobePin, LOW);
    send(cmd);
    digitalWrite(strobePin, HIGH);
}

void TM1638BVN::sendData(byte address, byte data)
{
    sendCommand(0x44);
    digitalWrite(strobePin, LOW);
    send(0xC0 | address);
    send(data);
    digitalWrite(strobePin, HIGH);
}

void TM1638BVN::send(byte data)
{
    for (int i = 0; i < 8; i++)
    {
        digitalWrite(clockPin, LOW);
        digitalWrite(dataPin, data & 1 ? HIGH : LOW);
        data >>= 1;
        digitalWrite(clockPin, HIGH);
    }
}

void TM1638BVN::setDisplay(const byte values[], uint8_t length)
{
    for (int i = 0; i < displays; i++)
    {
        int val = 0;

        for (uint8_t j = 0; j < length; j++)
        {
            val |= ((values[j] >> i) & 1) << (displays - j - 1);
        }

        sendData(i << 1, val);
    }
}
