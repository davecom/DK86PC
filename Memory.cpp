//
//  Memory.cpp
//
//  DK86PC - An Intel 8086 and IBM PC 5150 emulator.
//  Copyright (C) 2019 David Kopec
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

#include "Memory.hpp"
#include <algorithm>

using namespace std;

namespace DK86PC {
    
    void Memory::loadData(vector<byte> &data, address location) {
        copy(data.begin(), data.end(), ram + location);
    }
    
    byte Memory::readByte(address location) {
#ifdef DEBUG
        if (watchLocations.find(location) != watchLocations.end()) {
            cout << "read byte "  << hex << uppercase <<  (int) ((byte) ram[location]) << dec << " from " << location << endl;
        }
#endif
        return ram[location];
    }
    
    word Memory::readWord(address location) {
#ifdef DEBUG
        if (watchLocations.find(location) != watchLocations.end()) {
            cout << "read word " << hex << uppercase << (int) ((((word)ram[location + 1]) << 8) | ram[location]) << dec << " from " << location << endl;
        }
#endif
        return (((word)ram[location + 1]) << 8) | ram[location];
    }
    
    void Memory::setByte(address location, byte data) {
#ifdef DEBUG
        if (watchLocations.find(location) != watchLocations.end()) {
            cout << "wrote byte "  << hex << uppercase << (int) data << dec << " to " << location << endl;
        }
#endif
        ram[location] = data;
    }
    
    void Memory::setWord(address location, word data) {
        #ifdef DEBUG
                if (watchLocations.find(location) != watchLocations.end()) {
                    cout << "wrote word "  << hex << uppercase << (int) data << dec << " to " << location << endl;
                }
        #endif
        ram[location] = lowByte(data);
        ram[location + 1] = highByte(data);
    }
    
    byte& Memory::readByteRef(address location) {
        return ram[location];
    }
    
}
