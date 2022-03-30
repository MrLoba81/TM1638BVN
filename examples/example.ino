/*
example.ino - A simple demostration for TM1638BVN Library.

Copyright (C) 2022 Paolo Loberto - MrLoba81

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
#include <TM1638BVN.h>

const uint8_t strobe = 7;
const uint8_t clock = 9;
const uint8_t data = 8;

TM1638BVN display(data, clock, strobe);

uint8_t percentage = 0;
uint8_t statusBar = 0;
unsigned long millisCheck = 0;
unsigned long statusCheck = 0;

void setup()
{
  display.printString("Test", UPPER);
  delay(2000);
  display.printString("CLR", LOWER);
  delay(2000);
  display.clearDisplay(LOWER);

  delay(2000);
  display.printNumber(123, UPPER, 2);
  delay(2000);
  display.printNumber(9876, LOWER, 4);
  delay(2000);

  delay(2000);
  display.printString("SCRL", LOWER);
  delay(2000);
  display.clearDisplay(UPPER);
  delay(2000);

  display.setScrollingString("MrLoba81 Made it!", 18, UPPER);
}

void loop()
{
  display.loop();

  unsigned long curMillis = millis();

  if (curMillis - statusCheck >= 1000)
  {
    statusCheck = curMillis;
    display.setStatusBar(statusBar);
    statusBar = (statusBar + 1) % 4;
  }

  if (curMillis - millisCheck >= 500)
  {
    millisCheck = curMillis;

    display.printNumber(percentage, LOWER);
    display.setPercentage(percentage);

    percentage = (percentage + 1) % 101;
  }
}
