//
//  PIC.cpp
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

// implement the intel 8259

#include "PIC.hpp"

namespace DK86PC {

// note ocw2 not implemented
void PIC::writeCommand(byte command) {
    if (initializationWordNumber == 1) { // icw1
        if (!(command & 16)) {
            cout << "Expected icw1 to have bit 4 set." << endl;
        }
        // ignoring most of these settings
        // bit 0 set means sending icw4
        if (command & 1) {
            needICW4 = true;
        }
        initializationWordNumber++;
    } else if (command & 8){ // ocw3 has bit 4 set
        if ((command & 3) == 2) {
            readInService = false;
        }
        if ((command & 3) == 3) {
            readInService = true;
        }
    } else { // ocw2 is last possibility
        byte irq = (command & 7); // first 3 bits are which irq
        if (command & 32) { // end of interrupt send
            inServiceRegister = inServiceRegister & ~(1 << irq);
        }
    }
}

byte PIC::readStatus() {
    if (readInService) {
        return inServiceRegister;
    } else {
        return interruptRequestRegister;
    }
    
}

void PIC::writeData(byte value) {
    if (initializationWordNumber == 2) {
        baseVectorAddress = value & 0b11111000;
        initializationWordNumber++;
    } else if (initializationWordNumber == 3) {
        // supposed to set if slave is on, but ignoring
        initializationWordNumber++;
    } else if (initializationWordNumber == 4 && needICW4) {
        // ignoring most of this, but icw4 should have
        // bit 0 set to 1
        if (!(value & 1)) {
            cout << "Expected icw4 to have bit 0 set." << endl;
        }
        needICW4 = false;
        initializationWordNumber++;
    }
    
    // done with initialization, must be ocw1 to set mask
    else if (!needICW4 && (initializationWordNumber > 3)) {
        interruptMaskRegister = value;
    }
}

byte PIC::readData() {
    return interruptMaskRegister;
}

void PIC::requestInterrupt(byte irq) {
    // ignore if masked
    if (interruptMaskRegister & (1 << irq)) {
        return;
    }
    interruptRequestRegister = interruptRequestRegister | (1 << irq);
}

byte PIC::getInterrupt() {
    if (interruptRequestRegister == 0) {
        return NO_INTERRUPT;
    }
    for (int i = 0; i < 8; i++) {
        if (interruptRequestRegister & (1 << i)) {
            // turn off request
            interruptRequestRegister &= ~((byte) 1 << i);
            // set in service
            inServiceRegister = inServiceRegister | (1 << i);
            // return correct location
            return baseVectorAddress + i;
        }
    }
    return NO_INTERRUPT;
}

}
