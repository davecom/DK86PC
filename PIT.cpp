//
//  PIT.cpp
//
//  DK86PC - An Intel 8086 and IBM PC 5150 emulator.
//  Copyright (C) 2020 David Kopec
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

// implement the intel 8253

#include "PIT.hpp"

namespace DK86PC {

byte PIT::readCounter(int counterIndex) {
    switch (latches[counterIndex]) {
        case 0b01:
            return counters[counterIndex] & 0x00FF;
        case 0b10:
            return counters[counterIndex] >> 8;
        case 0b11:
            latchStatus[counterIndex] = !latchStatus[counterIndex];
            return latchStatus[counterIndex] ? (counters[counterIndex] & 0xFF00) >> 8 : counters[counterIndex] & 0x00FF;
        default:
            return counters[counterIndex];
    }
}

void PIT::writeCounter(int counterIndex, byte value) {
    switch (latches[counterIndex]) {
        case 0b01:
            counters[counterIndex] = (counters[counterIndex] & 0xFF00) | ((word)value & 0x00FF);
            break;
        case 0b10:
            counters[counterIndex] = (counters[counterIndex] & 0x00FF) | ((word)value << 8);
            break;
        case 0b11:
            latchStatus[counterIndex] = !latchStatus[counterIndex];
            if (latchStatus[counterIndex]) {
                counters[counterIndex] = (counters[counterIndex] & 0x00FF) | ((word)value << 8);
            } else {
                counters[counterIndex] = (counters[counterIndex] & 0xFF00) | ((word)value & 0x00FF);
            }
            break;
        default:
            counters[counterIndex] = value;
            break;
    }
    count[counterIndex] = counters[counterIndex];
}

void PIT::writeControl(byte value) {
    byte counterSelect = (value & 0b11000000) >> 6;
    if (counterSelect == 0b11) {
        printf("This is a writeControl for an 8254, but this is an 8253.");
        return;
    }
    byte counterLatch = (value & 0b00110000) >> 4;
    latches[counterSelect] = counterLatch;
    latchStatus[counterSelect] = true;
    byte mode = (value & 0b00001110) >> 1;
    modes[counterSelect] = mode;
    bcd[counterSelect] = value & 1;
}

void PIT::update() {
    for (int i = 0; i < NUM_COUNTERS; i++) {
        switch(modes[i]) {
            case 0:
                if (count[i] > 0) { // only fire once
                    count[i]--;
                    if (count[i] == 0 && i == 0) {
                        pic.requestInterrupt(0);
                    }
                }
                break;
            case 1:
                count[i]--;
                if (count[i] == 0) {
                    if (i == 0) {
                        pic.requestInterrupt(0);
                    }
                    // reset
                    count[i] = counters[i];
                }
                break;
            case 2:
                count[i]--;
                if (count[i] == 1) {
                    if (i == 0) {
                        pic.requestInterrupt(0);
                    }
                    // reset
                    count[i] = counters[i];
                }
                break;
            case 3: // supposed to be square wave, but not really implemented right now
                count[i]--;
                if (count[i] == 0) {
                    if (i == 0) {
                        pic.requestInterrupt(0);
                    }
                    // reset
                    count[i] = counters[i];
                }
                break;
            default:
                cout << "Unimplemented timer mode " << modes[i] << endl;
                break;
        }
        if (modes[i] == 1 && counters[i] == 0) { continue; }
        counters[i]--;
    }
}

}
