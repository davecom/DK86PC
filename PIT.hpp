//
//  PIT.hpp
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
#ifndef PIT_hpp
#define PIT_hpp

#include <stdio.h>
#include "Types.h"

#define NUM_COUNTERS 3

namespace DK86PC {
    class PIT {
    public:
        PIT() {
            for (int i = 0; i < NUM_COUNTERS; i++) {
                counters[i] = 0;
                modes[i] = 0;
                latchStatus[i] = true;
                latches[i] = 0;
                bcd[i] = false;
            }
        }
        word readCounter(int counterIndex);
        void writeCounter(int counterIndex, word value);
        void writeControl(byte value);
        void update();
    private:
        word counters[NUM_COUNTERS];
        byte latches[NUM_COUNTERS];
        byte latchStatus[NUM_COUNTERS];
        byte modes[NUM_COUNTERS];
        bool bcd[NUM_COUNTERS];
    };
}

#endif /* PIT_hpp */
