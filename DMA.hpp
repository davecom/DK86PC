//
//  DMA.hpp
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

// implement the intel 8237

#ifndef DMA_hpp
#define DMA_hpp

#include <stdio.h>
#include "Memory.hpp"

namespace DK86PC {
    class DMA {
    public:
        DMA() {
            for (int i = 0; i < 4; i++) {
                addressRegisters[i] = 0;
                counterRegisters[i] = 0;
                pageRegisters[i] = 0;
            }
        }
        ~DMA() {
        }
        void setAddress(byte channel, word address);
        void setCounter(byte channel, word count);
        word readAddress(byte channel);
        word readCounter(byte channel);
        void setPage(byte channel, byte page);
        void setMode(byte m);
        void singleChannelMask(byte m);
        void multiChannelMask(byte m);
        
        void writeCommand(byte command);
        void masterReset();
    private:
        word addressRegisters[4];
        word counterRegisters[4];
        byte pageRegisters[4];
        byte status = 0;
        bool enabled = true;
        bool flipflop = false; // false send low byte, true high
        byte masks = 0;
        byte mode = 0;
    };
}

#endif /* DMA_hpp */
