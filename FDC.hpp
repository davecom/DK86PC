//
//  FDC.hpp
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

#ifndef FDC_hpp
#define FDC_hpp

#include <stdio.h>
#include "Types.h"

namespace DK86PC {
    class FDC {
    public:
        FDC() :  dmaEnabled(false), selectedDevice(0)
        {
            for (int i = 0; i < 4; i++) {
                motorActive[i] = false;
            }
        };
        void writeControl(byte command); // Digital Output Register or Digital Control Port
        byte readStatus();
        
    private:
        bool motorActive[4];
        bool dmaEnabled;
        byte selectedDevice;
    };
}

#endif /* FDC_hpp */
