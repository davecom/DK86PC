//
//  Memory.hpp
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

#ifndef Memory_hpp
#define Memory_hpp

#include <vector>
#include "Types.h"
#ifdef DEBUG
#include <set>
#endif

using namespace std;

namespace DK86PC {

    class Memory {
    public:
        Memory(unsigned int ramSize = 1048576) {
            ram = new byte[ramSize];
        }
        ~Memory() {
            delete ram;
        }
        void loadData(vector<byte> &data, address location);
        byte readByte(address location);
        word readWord(address location);
        void setByte(address location, byte data);
        void setWord(address location, word data);
        byte& readByteRef(address location);
        void loadBIOS(string filename);
#ifdef DEBUG
        void setWatch(address location) {
            watchLocations.insert(location);
        }
#endif
    private:
        byte *ram;
#ifdef DEBUG
        set<address> watchLocations;
#endif
    };
}

#endif /* Memory_hpp */
