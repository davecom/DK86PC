//
//  CPU.hpp
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

#ifndef CPU_hpp
#define CPU_hpp

namespace DK86PC {

    class CPU {
    public:
        CPU(Memory &mem) : memory(mem) {
            reset();
        };
        void reset();
        void step();
    private:
        // External Constructs
        Memory &memory;
        
        // Registers
        union generalRegister {
            struct {
                byte high;
                byte low;
            } inner;
            word main;
        };
        generalRegister ar, br, cr, dr; // general purpose registers
        
        // this would be a lot easier if c++ supported anonymous unions
        // like c11
        #define ax ar.main
        #define al ar.inner.low
        #define ah ar.inner.high
        #define bx br.main
        #define bl br.inner.low
        #define bh br.inner.high
        #define cx cr.main
        #define cl cr.inner.low
        #define ch cr.inner.high
        #define dx dr.main
        #define dl dr.inner.low
        #define dh dr.inner.high
        
        word sp, bp, si, di; // stack pointer, base pointer, source index, destination index
        word cs, ds, es, ss; // current segment, data segment, extra, stack
        address ip; // instruction pointer
        struct {
            byte nothing: 4; //bits 12-15 unused
            byte overflow: 1; // bit 11
            byte direction: 1; // bit 10
            byte interrupt: 1; // bit 9
            byte trace: 1; // bit 8
            byte sign: 1; // bit 7
            byte zero: 1; // bit 6
            byte nothing2: 1; // bit 5
            byte auxiliaryCarry: 1; // bit 4
            byte nothing3: 1; // bit 3
            byte parity: 1; // bit 2
            byte nothing4: 1; // bit 1
            byte carry: 1; // bit 0
        } flags;
    };

}
#endif /* CPU_hpp */
