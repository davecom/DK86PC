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
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>

namespace DK86PC {

void FDC::writeControl(byte command) {
    selectedDevice = (command & 3);
    dmaEnabled = (command & 8);
    for (int i = 0; i < 4; i++) {
        motorActive[i] = ((command >> (4 + i)) & 1);
    }
    if (normalMode && !(command & 4)) { // do reset
        RQM = true;
        pic.requestInterrupt(6);
    }
    if (command & 4) {
        normalMode = true;
    } else {
        normalMode = false;
    }
}

byte FDC::readStatus() {
    return RQM << 7 | DIO << 6 | dmaEnabled << 5 | fdcBusy << 4;
}

byte FDC::readCommand() {
    byte temp = commandBuffer[commandBufferIndex];
    commandBufferIndex++;
    if (commandBufferIndex >= commandLength) {
        DIO = false;
        pic.requestInterrupt(6);
        
    }
    return temp;
}

void FDC::writeCommand(byte command) {
    switch (command & 0xF) {
        case 0x2: // READ TRACK
            break;
        case 0x3: // SPECIFY
            break;
        case 0x4: // CHECK STAT
            break;
        case 0x5: // WRITE SECTOR
            break;
        case 0x6: // READ SECTOR
            break;
        case 0x7: // CALIBRATE
            break;
        case 0x8: // CHECK INTTERUPT
            // byte 0 status registor 0 (ST0)
            commandBuffer[0] = 0;
            // byte 1 current cylinder
            commandBuffer[1] = currentCylinder;
            commandLength = 2;
            commandBufferIndex = 0;
            DIO = true;
            break;
        case 0x9: // WRITE DELETE SECTOR
            break;
        case 0xA: // READ ID SECTOR
            break;
        case 0xC: // READ DELETE SECTOR
            break;
        case 0xD: // FORMAT TRACK
            break;
        case 0xF: // SEEK
            break;
    }
}

void FDC::loadDisk(string filename) {
        
    // open input stream
    ifstream input(filename, ios::in | ios::binary);
    // check if the file opened successfully
    if (!input.is_open() || input.fail()) {
        cout << "Disk File " << filename << " can't be opened!";
        return;
    }
    
    copy(istreambuf_iterator<char>(input),
         istreambuf_iterator<char>(),
         back_inserter(diskAData));
    
}

}
