//
//  FDC.cpp
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

// implement the intel 8272a

#include "FDC.hpp"

namespace DK86PC {

void FDC::writeControl(byte command) {
    selectedDevice = (command & 3);
    dmaEnabled = (command & 8);
    for (int i = 0; i < 4; i++) {
        motorActive[i] = ((command >> (4 + i)) & 1);
    }
    if (!(command & 4)) { // reset mode
        return; // reset mode what should we do?
    }
}

}
