//
//  PIC.hpp
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

#ifndef PIC_hpp
#define PIC_hpp

#include <stdio.h>
#include "Types.h"

#define NO_INTERRUPT 255

namespace DK86PC {
    class PIC {
    public:
        PIC() :  interruptRequestRegister(0), inServiceRegister(0), interruptMaskRegister(0), initializationWordNumber(1),
        needICW4(false), readInService(false), baseVectorAddress(0)
        {};
        void writeCommand(byte command);
        byte readStatus();
        void writeData(byte mask);
        byte readData();
        void requestInterrupt(byte irq);
        byte getInterrupt();
        
    private:
        byte baseVectorAddress;
        int initializationWordNumber;
        bool needICW4;
        bool readInService;
        byte interruptRequestRegister;
        byte inServiceRegister;
        byte interruptMaskRegister;
    };
}

#endif /* PIC_hpp */
