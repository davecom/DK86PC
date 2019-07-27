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

    union ModRegRM {
        struct {
            byte rm: 3; //0-2
            byte reg: 3; //3-5
            byte mod: 2; //6-7
        };
        byte full;
        ModRegRM(byte thing) : full(thing) { }
    };
    
    class CPU {
    public:
        CPU(Memory &mem) : memory(mem) {
            reset();
        };
        void reset();
        unsigned long long step();
    private:
        uint64_t cycleCount;
        
        // Private methods
        inline byte getModRMWord(ModRegRM mrr);
        inline byte getRegWord(byte reg);
        inline byte getModRMByte(ModRegRM mrr);
        inline byte getRegByte(byte reg);
        template <typename T>
        inline void setReg(byte reg, T data);
        //ROL/ROR/RCL/RCR/SHL/SHR/SAR
        inline void rol(ModRegRM mrr, byte amount);
        inline void ror(ModRegRM mrr, byte amount);
        inline void rcl(ModRegRM mrr, byte amount);
        inline void rcr(ModRegRM mrr, byte amount);
        inline void shl(ModRegRM mrr, byte amount);
        inline void shr(ModRegRM mrr, byte amount);
        inline void sar(ModRegRM mrr, byte amount);
        
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
        
        // this would be a lot easier if c++ supported anonymous structs like c11
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
        word cs, ds, es, ss; // code segment, data segment, extra, stack
        word *currentSegment;
        word ip; // instruction pointer
        union {
            struct {
                bool carry: 1; // bit 0
                bool nothing4: 1; // bit 1
                bool parity: 1; // bit 2
                bool nothing3: 1; // bit 3
                bool auxiliaryCarry: 1; // bit 4
                bool nothing2: 1; // bit 5
                bool zero: 1; // bit 6
                bool sign: 1; // bit 7
                bool trace: 1; // bit 8
                bool interrupt: 1; // bit 9
                bool direction: 1; // bit 10
                bool overflow: 1; // bit 11
                bool nothing: 4; //bits 12-15 unused
            };
            word flags;
        };
    };

}
#endif /* CPU_hpp */
