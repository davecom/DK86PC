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
#include <fstream>
#include <iostream>
#include <iterator>

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

    static vector<byte> loadFile(string filename) {
        vector<byte> buffer;
        // open input stream
        ifstream input(filename, ios::in | ios::binary);
        // check if the file opened successfully
        if (!input.is_open() || input.fail()) {
            cout << "File " << filename << " can't be opened!";
        }
        
        copy(istreambuf_iterator<char>(input),
             istreambuf_iterator<char>(),
             back_inserter(buffer));
        return buffer;
    }

    // On the original PC BIOS exists from 0xE0000 to 0xFFFFF
    // so implicitly must be <= 128k
    void Memory::loadBIOS(string filename) {
        
        vector<byte> buffer = loadFile(filename);
        // in original IBM PC BIOS is right before end of 1 MB of memory
        address biosPlace = 0x100000 - (address) buffer.size();
        loadData(buffer, biosPlace);
    }

    void Memory::loadCasetteBASIC(string filename1, string filename2, string filename3, string filename4) {
        vector<byte> buffer1 = loadFile(filename1);
        loadData(buffer1, 0xF6000);
        vector<byte> buffer2 = loadFile(filename2);
        loadData(buffer2, 0xF8000);
        vector<byte> buffer3 = loadFile(filename3);
        loadData(buffer3, 0xFA000);
        vector<byte> buffer4 = loadFile(filename4);
        loadData(buffer4, 0xFC000);
    }
    
}
