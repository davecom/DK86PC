//
//  DMA.cpp
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

#include "DMA.hpp"

using namespace std;

namespace DK86PC {
    
void DMA::setAddress(byte channel, word address) {
    addressRegisters[channel] = address;
}

void DMA::setCounter(byte channel, word count) {
    counterRegisters[channel] = count;
}

word DMA::readAddress(byte channel) {
    return addressRegisters[channel];
}

word DMA::readCounter(byte channel) {
    return counterRegisters[channel];
}

void DMA::setPage(byte channel, byte page) {
    pageRegisters[channel] = page;
}

void DMA::setMode(byte m) {
    mode = m;
}

void DMA::singleChannelMask(byte m) {
    byte channel = m & 3;
    // clear channel
    masks &= ~(1 << channel);
    // set channel
    masks |= (((m >> 2) & 1) << channel);
}

void DMA::multiChannelMask(byte m) {
    masks = ((m & 0xF) | (masks & 0xF0));
}

void DMA::writeCommand(byte command) {
    if (command & 4) { // bit 2 disables DMA
        enabled = false;
    }
}

void DMA::masterReset() {
    flipflop = false;
    status = 0;
    masks = 0x0F;
}

void DMA::clearBytePointerFlipFlop() {
    flipflop = false;
}
    
}
