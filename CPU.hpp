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

#ifndef CPU_hpp
#define CPU_hpp

#include "Memory.hpp"
#include "PortInterface.hpp"

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
        CPU(PortInterface &p, Memory &mem) : portInterface(p), memory(mem) {
            reset();
        };
        void reset();
        void hardwareInterrupt(byte info);
        void step();
        bool isHalted() { return halted; };
        bool canInterrupt() { return interrupt; };
        #ifdef DEBUG
        void setTestingFlags(word testFlags) {
            flags = testFlags;
        };
        void setCSIP(word cs, word ip) {
            this->cs = cs;
            this->ip = ip;
        }
        #endif
    private:
        uint64_t cycleCount;
        bool halted;
        // Private methods
        
        // Flag Set Methods
        inline void setSignFlagByte(byte data);
        template <typename T>
        inline void setZeroFlag(T data);
        template <typename T>
        inline void setParityFlag(T data);
        inline void setSZPFlagsByte(byte data);
        
        inline void setSignFlagWord(word data);
        inline void setSZPFlagsWord(word data);
        
        inline void setFlagsDefaults();
        
        // Get & Change Reg/Memory Methods
        inline address calcEffectiveAddress(ModRegRM mrr);
        inline address calcPhysicalAddress(ModRegRM mrr);
        inline word getModRMWord(ModRegRM mrr);
        inline word getRegWord(byte reg);
        inline byte getModRMByte(ModRegRM mrr);
        inline byte getRegByte(byte reg);
        template <typename T>
        inline void setReg(byte reg, T data);
        inline void setRegByte(byte reg, byte data);
        inline void setRegWord(byte reg, word data);
        inline void setModRMByte(ModRegRM mrr, byte data);
        inline void setModRMWord(ModRegRM mrr, word data);
        inline void modInstructionLength(ModRegRM mrr, byte &il);
        inline word getSegmentRegWord(byte reg);
        inline void setSegmentRegWord(byte reg, word data);
        
        //ROL/ROR/RCL/RCR/SHL/SHR/SAR
        inline void rolByte(ModRegRM mrr, byte amount);
        inline void rorByte(ModRegRM mrr, byte amount);
        inline void rclByte(ModRegRM mrr, byte amount);
        inline void rcrByte(ModRegRM mrr, byte amount);
        inline void shlByte(ModRegRM mrr, byte amount);
        inline void shrByte(ModRegRM mrr, byte amount);
        inline void sarByte(ModRegRM mrr, byte amount);
        inline void rolWord(ModRegRM mrr, byte amount);
        inline void rorWord(ModRegRM mrr, byte amount);
        inline void rclWord(ModRegRM mrr, byte amount);
        inline void rcrWord(ModRegRM mrr, byte amount);
        inline void shlWord(ModRegRM mrr, byte amount);
        inline void shrWord(ModRegRM mrr, byte amount);
        inline void sarWord(ModRegRM mrr, byte amount);
        
        //XOR/OR/AND/SUB/ADD/INC/DEC
        inline void xorByte(byte &left, byte right);
        inline void xorWord(word &left, word right);
        inline void orByte(byte &left, byte right);
        inline void orWord(word &left, word right);
        inline void andByte(byte &left, byte right);
        inline void andWord(word &left, word right);
        inline void subByte(byte &left, byte right);
        inline void subWord(word &left, word right);
        inline void subByteWithBorrow(byte &left, byte right);
        inline void subWordWithBorrow(word &left, word right);
        inline void addByte(byte &left, byte right);
        inline void addWord(word &left, word right);
        inline void addByteWithCarry(byte &left, byte right);
        inline void addWordWithCarry(word &left, word right);
        inline void incByte(byte &left);
        inline void incWord(word &left);
        inline void decByte(byte &left);
        inline void decWord(word &left);
        
        // stack and interrupts
        inline void push(word value);
        inline word pop();
        inline void performInterrupt(byte type);
        
        // Debug
        inline void debugPrint(byte opcode);
        
        // External Constructs
        PortInterface &portInterface;
        Memory &memory;
        
        // Registers
        union generalRegister {
            struct {
                byte low;
                byte high;
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
        #define Ch cr.inner.high
        #define Dx dr.main
        #define dl dr.inner.low
        #define dh dr.inner.high
        
        word sp, bp, si, di; // stack pointer, base pointer, source index, destination index
        word cs, ds, es, ss; // code segment, data segment, extra, stack
        word *currentSegment;
        bool segmentOverride = false;
        word ip; // instruction pointer
        byte prefixCount; // how many prefixes before current opcode
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
