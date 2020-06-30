//
//  PPI.hpp
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

// implement the intel 8255

#ifndef PPI_hpp
#define PPI_hpp

#include <stdio.h>
#include "Types.h"

namespace DK86PC {
    class PPI {
    public:
        PPI() : a(16), b(16), c(0), control(0) {};
        void setB(byte value);
        void setControl(byte value);
        byte readA();
        byte readB();
        byte readC();
    private:
        byte a, b, c, control; // registers
    };
}

#endif /* PPI_hpp */
