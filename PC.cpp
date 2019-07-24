//
//  PC.cpp
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

#include <string>
#include <fstream>
#include <iterator>
#include <vector>
#include "PC.hpp"

using namespace std;

namespace DK86PC {
    
    // On the original PC BIOS exists from 0xE0000 to 0xFFFFF
    // so implicitly must be <= 128k
    void PC::loadBIOS(string filename) {
        ifstream input(filename, ios::in | ios::binary);
        vector<byte> buffer(istreambuf_iterator<char>(input), {});
        address biosPlace = 0x100000 - (address) buffer.size();
        memory.loadData(buffer, biosPlace);
    }
    
    void PC::run() {
        
    }
}
