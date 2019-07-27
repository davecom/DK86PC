//
//  CPU.cpp
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

#include "CPU.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

namespace DK86PC {
    
    
    
    // Utility Stuff
    #define NEXT_INSTRUCTION ((((address)cs) << 4) + ip)
    
    
    
    inline word CPU::getRegWord(byte reg) {
        switch (reg) {
            case 0b000:         // w = 0
                return al;
                break;
            case 0b001:
                return cl;
                break;
            case 0b010:
                return dl;
                break;
            case 0b011:
                return bl;
                break;
            case 0b100:
                return ah;
                break;
            case 0b101:
                return ch;
                break;
            case 0b110:
                return dh;
                break;
            case 0b111:
                return bh;
                break;
            case 0b1000:        // w = 1
                return ax;
                break;
            case 0b1001:
                return cx;
                break;
            case 0b1010:
                return dx;
                break;
            case 0b1011:
                return bx;
                break;
            case 0b1100:
                return sp;
                break;
            case 0b1101:
                return bp;
                break;
            case 0b1110:
                return si;
                break;
            case 0b1111:
                return di;
                break;
        }
        
        return 0;
    }
    
    inline byte CPU::getRegByte(byte reg) {
        switch (reg) {
            case 0b000:         // w = 0
                return al;
                break;
            case 0b001:
                return cl;
                break;
            case 0b010:
                return dl;
                break;
            case 0b011:
                return bl;
                break;
            case 0b100:
                return ah;
                break;
            case 0b101:
                return ch;
                break;
            case 0b110:
                return dh;
                break;
            case 0b111:
                return bh;
                break;
            case 0b1000:        // w = 1
                return ax;
                break;
            case 0b1001:
                return cx;
                break;
            case 0b1010:
                return dx;
                break;
            case 0b1011:
                return bx;
                break;
            case 0b1100:
                return sp;
                break;
            case 0b1101:
                return bp;
                break;
            case 0b1110:
                return si;
                break;
            case 0b1111:
                return di;
                break;
        }
        
        return 0;
    }
    
    inline byte CPU::getModRMWord(ModRegRM mrr) {
        if (mrr.mod == 0b11) {
            return getRegWord(mrr.rm);
        }
        word disp = 0;
        if (mrr.mod == 0b01) {
            disp = signExtend(memory.readByte(NEXT_INSTRUCTION + 2));
        } else if (mrr.mod == 0b10) {
            disp = memory.readWord(NEXT_INSTRUCTION + 2);
        }
        
        address ea = bx + si + disp; // 0b000
        switch (mrr.rm) {
            case 0b001:
                ea = bx + di + disp;
                break;
            case 0b010:
                ea = bp + si + disp;
                break;
            case 0b011:
                ea = bp + di + disp;
                break;
            case 0b100:
                ea = si + disp;
                break;
            case 0b101:
                ea = di + disp;
                break;
            case 0b110:
                ea = bp + disp;
                break;
            case 0b111:
                ea = bx + disp;
                break;
        }
        
        return memory.readWord(((*currentSegment << 4) + ea));
    }
    
    inline byte CPU::getModRMByte(ModRegRM mrr) {
        if (mrr.mod == 0b11) {
            return getRegByte(mrr.rm);
        }
        word disp = 0;
        if (mrr.mod == 0b01) {
            disp = signExtend(memory.readByte(NEXT_INSTRUCTION + 2));
        } else if (mrr.mod == 0b10) {
            disp = memory.readWord(NEXT_INSTRUCTION + 2);
        }
        
        address ea = bx + si + disp; // 0b000
        switch (mrr.rm) {
            case 0b001:
                ea = bx + di + disp;
                break;
            case 0b010:
                ea = bp + si + disp;
                break;
            case 0b011:
                ea = bp + di + disp;
                break;
            case 0b100:
                ea = si + disp;
                break;
            case 0b101:
                ea = di + disp;
                break;
            case 0b110:
                ea = bp + disp;
                break;
            case 0b111:
                ea = bx + disp;
                break;
        }
        
        return memory.readByte(((*currentSegment << 4) + ea));
    }
    
    template <typename T>
    inline void CPU::setReg(byte reg, T data) {
        switch (reg) {
            case 0b000:         // w = 0
                al = data;
                break;
            case 0b001:
                cl = data;
                break;
            case 0b010:
                dl = data;
                break;
            case 0b011:
                bl = data;
                break;
            case 0b100:
                ah = data;
                break;
            case 0b101:
                ch = data;
                break;
            case 0b110:
                dh = data;
                break;
            case 0b111:
                bh = data;
                break;
            case 0b1000:        // w = 1
                ax = data;
                break;
            case 0b1001:
                cx = data;
                break;
            case 0b1010:
                dx = data;
                break;
            case 0b1011:
                bx = data;
                break;
            case 0b1100:
                sp = data;
                break;
            case 0b1101:
                bp = data;
                break;
            case 0b1110:
                si = data;
                break;
            case 0b1111:
                di = data;
                break;
        }
    }
    
    inline void CPU::rol(ModRegRM mrr, byte amount) {
        
    }
    
    inline void CPU::ror(ModRegRM mrr, byte amount) {
        
    }
    
    inline void CPU::rcl(ModRegRM mrr, byte amount) {
        
    }
    
    inline void CPU::rcr(ModRegRM mrr, byte amount) {
        
    }
    
    inline void CPU::shl(ModRegRM mrr, byte amount) {
        
    }
    
    inline void CPU::shr(ModRegRM mrr, byte amount) {
        
        
        int count = amount;
        if (count == 1) { overflow = }
    }
    
    inline void CPU::sar(ModRegRM mrr, byte amount) {
        
    }
    
    
    void CPU::reset() {
        cycleCount = 0;
        
        // reset registers
        ax = 0;
        bx = 0;
        cx = 0;
        dx = 0;
        sp = 0;
        bp = 0;
        si = 0;
        di = 0;
        cs = 0xFFFF;
        ds = 0;
        es = 0;
        ss = 0;
        ip = 0; // reset vector at OxFFFF0 (cs << 4 + ip)
        flags = 0xF000;
    }
    
    // returns the number of cycles counted
    uint64_t CPU::step() {
        bool jump = false;
        bool lock = false;
        bool repeatCX = false;
        bool repeatZF = false;
        byte instructionLength = 1;
        
        byte opcode = memory.readByte(NEXT_INSTRUCTION);
        
        
        currentSegment = &ds;
        
        // check for prefix to opcode
        while (true) {
            switch (opcode) {
                case 0xF0:
                    lock = true;
                    ip += 1;
                    break;
                case 0xF3:
                    repeatCX = true;
                    ip += 1;
                    break;
                case 0xF2:
                    repeatZF = true;
                    ip += 1;
                    break;
                case 0x2E:
                    currentSegment = &cs;
                    ip += 1;
                    break;
                case 0x36:
                    currentSegment = &ss;
                    ip += 1;
                    break;
                case 0x3E:
                    currentSegment = &ds;
                    ip += 1;
                    break;
                case 0x26:
                    currentSegment = &es;
                    ip += 1;
                    break;
                default:
                    
                    goto actualOpcode;
            }
        }
        
    actualOpcode:
        cout << hex << uppercase << (int)opcode << dec << endl;
        switch (opcode) {
            
            // JO jump on overflow
            case 0x70:
                if (overflow) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // JNO jump on not overflow
            case 0x71:
                if (!overflow) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // JC/JB/JNAE jump on carry
            case 0x72:
                if (carry) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
                
            // JNC/JNB/JAE jump not carry
            case 0x73:
                if (!carry) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // JE/JZ jump on equal/zero
            case 0x74:
                if (zero) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // JNE/JNZ jump on NOT equal/zero
            case 0x75:
                if (!zero) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // JBE/JNA Jump on below or equal/not above
            case 0x76:
                if (zero || carry) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // JNBE/JA Jump on not below or equal above
            case 0x77:
                if (!(zero | carry)) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // JS Jump on Sign
            case 0x78:
                if (sign) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // JS Jump on Not Sign
            case 0x79:
                if (!sign) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // JP/JPE Jump on Parity
            case 0x7A:
                if (parity) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // JNP/JPO Jump on Not Parity
            case 0x7B:
                if (!parity) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // JL/JNGE Jump if neither greater nor equal
            case 0x7C:
                if (sign ^ overflow) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // JNL/JGE Jump if not less
            case 0x7D:
                if (!(sign ^ overflow)) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // JLE/JNG Jump if not greater
            case 0x7E:
                if ((sign ^ overflow) | zero) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // JNLE/JG Jump if greater
            case 0x7F:
                if (!((sign ^ overflow) | zero)) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // SAHF store AH into lower byte of flags
            case 0x9E:
                flags = ((flags & 0xFF00) | ah);
                break;
            
            // LAHF copy low byte of flags word to AH
            case 0x9F:
                ah = ((byte) (flags & 0x00FF));
                break;
                
            // MOV Byte move data to register
            case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB7:
                setReg(lowNibble(opcode), memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                break;
            
            // ROL/ROR/RCL/RCR/SHL/SHR/SAR/ROL
            case 0xD2:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                switch (mrr.reg) {
                    case 0b000:
                        rol(mrr, cl);
                        break;
                    case 0b001:
                        ror(mrr, cl);
                        break;
                    case 0b010:
                        rcl(mrr, cl);
                        break;
                    case 0b011:
                        rcr(mrr, cl);
                        break;
                    case 0b100:
                        shl(mrr, cl);
                        break;
                    case 0b101:
                        shr(mrr, cl);
                        break;
                    case 0b110:
                        cout << "Unused opcode extension 110 for 0xD2" << endl;
                        break;
                    case 0b111:
                        sar(mrr, cl);
                        break;
                    
                }
                break;
            }
                
            
            // JCXZ Jump if CX is zero
            case 0xE3:
                if (cx == 0) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
                
            // JMP direct
            case 0xEA:
            { // scope for new variables
                // next two instructions are new ip
                // can't change ip until after have read CS
                word nextIp = memory.readWord(NEXT_INSTRUCTION + 1);
                // next two after that are new cs
                word nextCs = memory.readWord(NEXT_INSTRUCTION + 3);
                ip = nextIp;
                cs = nextCs;
                jump = true;
                instructionLength = 5;
                break;
            }
            
            // CLI clear interrupt
            case 0xFA:
                interrupt = false;
                break;
            
            // Unknown Opcode
            default:
                cout << "Unknown opcode!" << endl;
                return -1;
        }
        
        // if we didn't jump, move the instruction pointer forward
        if (!jump) { ip += instructionLength; }
        
        // dummy for now until we have accurate cycle counts
        cycleCount += 1;
        return 1;
    }
    
}
