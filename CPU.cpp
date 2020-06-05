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

#include "Instructions.h"
#include "CPU.hpp"
#include <iostream>
#include <iomanip>
#include "PC.hpp"

using namespace std;

namespace DK86PC {
    
    
    
    // Utility Stuff
    #define NEXT_INSTRUCTION ((((address)cs) << 4) + ip)
    
    inline void CPU::setSignFlagByte(byte data) {
        if (highBitByte(data)) {
            sign = true;
        } else {
            sign = false;
        }
    }
    
    template <typename T>
    inline void CPU::setZeroFlag(T data) {
        if (data == 0) {
            zero = true;
        } else {
            zero = false;
        }
    }
    
    template <typename T>
    inline void CPU::setParityFlag(T data) {
        // source: http://graphics.stanford.edu/~seander/bithacks.html#ParityNaive
        byte tested = lowByte(data);
        bool p = true;
        
        while (tested) {
            p = !p;
            tested = tested & (tested - 1);
        }
        
        parity = p;
    }
    
    inline void CPU::setSZPFlagsByte(byte data) {
        setSignFlagByte(data);
        setZeroFlag(data);
        setParityFlag(data);
    }
    
    inline void CPU::setSignFlagWord(word data) {
        if (highBitWord(data)) {
            sign = true;
        } else {
            sign = false;
        }
    }
    
    inline void CPU::setSZPFlagsWord(word data) {
        setSignFlagWord(data);
        setZeroFlag(data);
        setParityFlag(data);
    }
    
    inline word CPU::getRegWord(byte reg) {
        switch (reg) {
            case 0b000:        // w = 1
                return ax;
                break;
            case 0b001:
                return cx;
                break;
            case 0b010:
                return Dx;
                break;
            case 0b011:
                return bx;
                break;
            case 0b100:
                return sp;
                break;
            case 0b101:
                return bp;
                break;
            case 0b110:
                return si;
                break;
            case 0b111:
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
                return Ch;
                break;
            case 0b110:
                return dh;
                break;
            case 0b111:
                return bh;
                break;
        }
        
        return 0;
    }
    
    inline address CPU::calcEffectiveAddress(ModRegRM mrr) {
        word disp = 0;
        if (mrr.mod == 0b01) {
            disp = signExtend(memory.readByte(NEXT_INSTRUCTION + 2));
        } else if (mrr.mod == 0b10) {
            disp = memory.readWord(NEXT_INSTRUCTION + 2);
        }
        
        address ea = bx + si + disp; // 0b000
        switch (mrr.rm) {
            case 0b000:
                ea = bx + si + disp;
                break;
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
                if (mrr.mod == 0b00) {
                    ea = memory.readWord(NEXT_INSTRUCTION + 2); // direct addressing
                } else {
                    ea = bp + disp;
                }
                break;
            case 0b111:
                ea = bx + disp;
                break;
        }
        return ea;
    }
    
    inline word CPU::getModRMWord(ModRegRM mrr) {
        if (mrr.mod == 0b11) {
            return getRegWord(mrr.rm);
        }
        
        address ea = calcEffectiveAddress(mrr);
        address pa = ((*currentSegment << 4) + ea); // physical address
        return memory.readWord(pa);
    }
    
    inline byte CPU::getModRMByte(ModRegRM mrr) {
        if (mrr.mod == 0b11) {
            return getRegByte(mrr.rm);
        }
        address ea = calcEffectiveAddress(mrr);
        address pa = ((*currentSegment << 4) + ea); // physical address
        return memory.readByte(pa);
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
                Ch = data;
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
                Dx = data;
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
    
    inline void CPU::setRegByte(byte reg, byte data) {
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
                Ch = data;
                break;
            case 0b110:
                dh = data;
                break;
            case 0b111:
                bh = data;
                break;
        }
    }
    
    inline void CPU::setRegWord(byte reg, word data) {
        switch (reg) {
            case 0b000:        // w = 1
                ax = data;
                break;
            case 0b001:
                cx = data;
                break;
            case 0b010:
                Dx = data;
                break;
            case 0b011:
                bx = data;
                break;
            case 0b100:
                sp = data;
                break;
            case 0b101:
                bp = data;
                break;
            case 0b110:
                si = data;
                break;
            case 0b111:
                di = data;
                break;
        }
    }
    
    inline void CPU::setModRMByte(ModRegRM mrr, byte data) {
        if (mrr.mod == 0b11) {
            setRegByte(mrr.rm, data);
            return;
        }
        
        address ea = calcEffectiveAddress(mrr);
        address pa = ((*currentSegment << 4) + ea); // physical address
        memory.setByte(pa, data);
    }
    
    inline void CPU::setModRMWord(ModRegRM mrr, word data) {
        if (mrr.mod == 0b11) {
            setRegWord(mrr.rm, data);
            return;
        }
        
        address ea = calcEffectiveAddress(mrr);
        address pa = ((*currentSegment << 4) + ea); // physical address
        memory.setWord(pa, data);
    }
    
    // increment instruction length if necessary
    inline void CPU::modInstructionLength(ModRegRM mrr, byte &il) {
        if (mrr.mod == 0b01) {
            il += 1;
        } else if (mrr.mod == 0b10) {
            il += 2;
        } else if (mrr.mod == 0b00 && mrr.rm == 0b110) {
            il += 2;
        }
    }

    inline word CPU::getSegmentRegWord(byte reg) {
        switch (reg) {
            case 0b000:        // w = 1
                return es;
                break;
            case 0b001:
                return cs;
                break;
            case 0b010:
                return ss;
                break;
            case 0b011:
                return ds;
                break;
            default:
                cout << "invalid segment register" << endl;
                return ds;
        }
    }

    inline void CPU::setSegmentRegWord(byte reg, word data) {
        switch (reg) {
            case 0b000:        // w = 1
                es = data;
                break;
            case 0b001:
                cs = data;
                break;
            case 0b010:
                ss = data;
                break;
            case 0b011:
                ds = data;
                break;
            default:
                cout << "invalid segment register" << endl;
                return;
        }
    }
    
    // ROL
    inline void CPU::rolByte(ModRegRM mrr, byte amount) {
        int count = amount;
        byte operand = getModRMByte(mrr);
        while (count > 0) {
            carry = highBitByte(operand);
            operand = ((operand << 1) | carry);
            count--;
        }
        if (amount == 1) {
            overflow = highBitByte(operand) ^ carry;
        }
        setModRMByte(mrr, operand);
    }
    
    inline void CPU::rorByte(ModRegRM mrr, byte amount) {
        int count = amount;
        byte operand = getModRMByte(mrr);
        while (count > 0) {
            carry = lowBit(operand);
            operand >>= 1;
            operand |= (carry << 7);
            count--;
        }
        if (amount == 1) {
            overflow = highBitByte(operand) ^ carry;
        }
        setModRMByte(mrr, operand);
    }
    
    inline void CPU::rclByte(ModRegRM mrr, byte amount) {
        int count = amount;
        byte operand = getModRMByte(mrr);
        while (count > 0) {
            bool tempCarry = carry;
            carry = highBitByte(operand);
            operand = ((operand << 1) | tempCarry);
            count--;
        }
        if (amount == 1) {
            overflow = highBitByte(operand) ^ carry;
        }
        setModRMByte(mrr, operand);
    }
    
    inline void CPU::rcrByte(ModRegRM mrr, byte amount) {
        int count = amount;
        byte operand = getModRMByte(mrr);
        if (count == 1) {
            overflow = highBitByte(operand) ^ carry;
        }
        
        while (count > 0) {
            bool tempCarry = carry;
            carry = lowBit(operand);
            operand >>= 1;
            operand |= (tempCarry << 7);
            count--;
        }
        setModRMByte(mrr, operand);
    }
    
    inline void CPU::shlByte(ModRegRM mrr, byte amount) {
        int count = amount;
        byte operand = getModRMByte(mrr);
        
        while (count > 0) {
            carry = highBitByte(operand);
            operand <<= 1;
            count--;
        }
        if (amount == 1) { overflow = carry ^ highBitByte(operand); }
        setSZPFlagsByte(operand);
        setModRMByte(mrr, operand);
    }
    
    inline void CPU::shrByte(ModRegRM mrr, byte amount) {
        int count = amount;
        byte operand = getModRMByte(mrr);
        if (count == 1) { overflow = highBitByte(operand); }
        while (count > 0) {
            carry = lowBit(operand);
            operand >>= 1;
            count--;
        }
        setSZPFlagsByte(operand);
        setModRMByte(mrr, operand);
    }
    
    inline void CPU::sarByte(ModRegRM mrr, byte amount) {
        int count = amount;
        byte operand = getModRMByte(mrr);
        while (count > 0) {
            carry = lowBit(operand);
            int8_t temp = (int8_t) operand;
            temp /= 2;
            operand = (byte) temp;
            operand >>= 1;
            count--;
        }
        if (count == 1) { overflow = 0; }
        setSZPFlagsByte(operand);
        setModRMByte(mrr, operand);
    }
    
    inline void CPU::rolWord(ModRegRM mrr, byte amount) {
        int count = amount;
        word operand = getModRMWord(mrr);
        while (count > 0) {
            carry = highBitWord(operand);
            operand = ((operand << 1) | carry);
            count--;
        }
        if (amount == 1) {
            overflow = highBitWord(operand) ^ carry;
        }
        setModRMWord(mrr, operand);
    }
    
    inline void CPU::rorWord(ModRegRM mrr, byte amount) {
        int count = amount;
        word operand = getModRMWord(mrr);
        while (count > 0) {
            carry = lowBit(operand);
            operand >>= 1;
            operand |= (carry << 15);
            count--;
        }
        if (amount == 1) {
            overflow = highBitWord(operand) ^ carry;
        }
        setModRMWord(mrr, operand);
    }
    
    inline void CPU::rclWord(ModRegRM mrr, byte amount) {
        int count = amount;
        word operand = getModRMWord(mrr);
        while (count > 0) {
            bool tempCarry = carry;
            carry = highBitWord(operand);
            operand = ((operand << 1) | tempCarry);
            count--;
        }
        if (amount == 1) {
            overflow = highBitWord(operand) ^ carry;
        }
        setModRMWord(mrr, operand);
    }
    
    inline void CPU::rcrWord(ModRegRM mrr, byte amount) {
        int count = amount;
        word operand = getModRMWord(mrr);
        if (count == 1) {
            overflow = highBitWord(operand) ^ carry;
        }
        
        while (count > 0) {
            bool tempCarry = carry;
            carry = lowBit(operand);
            operand >>= 1;
            operand |= (tempCarry << 15);
            count--;
        }
        setModRMWord(mrr, operand);
    }
    
    inline void CPU::shlWord(ModRegRM mrr, byte amount) {
        int count = amount;
        word operand = getModRMWord(mrr);
        
        while (count > 0) {
            carry = highBitWord(operand);
            operand <<= 1;
            count--;
        }
        if (amount == 1) { overflow = carry ^ highBitWord(operand); }
        setSZPFlagsWord(operand);
        setModRMWord(mrr, operand);
    }
    
    inline void CPU::shrWord(ModRegRM mrr, byte amount) {
        int count = amount;
        word operand = getModRMWord(mrr);
        if (count == 1) { overflow = highBitWord(operand); }
        while (count > 0) {
            carry = lowBit(operand);
            operand >>= 1;
            count--;
        }
        setSZPFlagsWord(operand);
        setModRMWord(mrr, operand);
    }
    
    inline void CPU::sarWord(ModRegRM mrr, byte amount) {
        int count = amount;
        word operand = getModRMWord(mrr);
        while (count > 0) {
            carry = lowBit(operand);
            int16_t temp = (int16_t) operand;
            temp /= 2;
            operand = (word) temp;
            operand >>= 1;
            count--;
        }
        if (count == 1) { overflow = 0; }
        setSZPFlagsWord(operand);
        setModRMWord(mrr, operand);
    }

    inline void CPU::xorByte(byte &left, byte right) {
        left = left ^ right;
        overflow = 0;
        carry = 0;
        setSZPFlagsByte(left);
    }

    inline void CPU::xorWord(word &left, word right) {
        left = left ^ right;
        overflow = 0;
        carry = 0;
        setSZPFlagsWord(left);
    }

    inline void CPU::orByte(byte &left, byte right) {
        left = left | right;
        overflow = 0;
        carry = 0;
        setSZPFlagsByte(left);
    }

    inline void CPU::orWord(word &left, word right) {
        left = left | right;
        overflow = 0;
        carry = 0;
        setSZPFlagsWord(left);
    }

    inline void CPU::andByte(byte &left, byte right) {
        left = left & right;
        overflow = 0;
        carry = 0;
        setSZPFlagsByte(left);
    }

    inline void CPU::andWord(word &left, word right) {
        left = left & right;
        overflow = 0;
        carry = 0;
        setSZPFlagsWord(left);
    }
    
    inline void CPU::subByte(byte &left, byte right) {
        byte originalHigh = highBitByte(left);
        carry = (right > left);
        auxiliaryCarry = (lowNibble(right) > lowNibble(left));
        left = left - right;
        overflow = (highBitByte(originalHigh) ^ highBitByte(right)) && (originalHigh ^ highBitByte(left));
        setSZPFlagsByte(left);
    }

    inline void CPU::subWord(word &left, word right) {
        word originalHigh = highBitWord(left);
        carry = (right > left);
        auxiliaryCarry = (lowNibble(right) > lowNibble(left));
        left = left - right;
        overflow = (highBitWord(originalHigh) ^ highBitWord(right)) && (originalHigh ^ highBitWord(left));
        setSZPFlagsWord(left);
    }

    inline void CPU::addByte(byte &left, byte right) {
        byte originalHigh = highBitByte(left);
        carry = ((word)right + (word)left) > 0x00FF;
        auxiliaryCarry = ((lowNibble(right) + lowNibble(left)) > 0x0F);
        left = left + right;
        overflow = (highBitByte(originalHigh) == highBitByte(right)) && (originalHigh ^ highBitByte(left));
        setSZPFlagsByte(left);
    }

    inline void CPU::addWord(word &left, word right) {
        word originalHigh = highBitWord(left);
        carry = ((uint32_t)right + (uint32_t)left) > 0x0000FFFF;
        auxiliaryCarry = ((lowNibble(right) + lowNibble(left)) > 0x0F);
        left = left + right;
        overflow = (highBitWord(originalHigh) == highBitWord(right)) && (originalHigh ^ highBitWord(left));
        setSZPFlagsWord(left);
    }

    inline void CPU::incByte(byte &temp) {
        auxiliaryCarry = (lowNibble(temp) == 0x0F);
        temp++;
        overflow = (temp == 0);
        setSZPFlagsByte(temp);
    }

    inline void CPU::incWord(word &temp) {
        auxiliaryCarry = (lowNibble(temp) == 0x0F);
        temp++;
        overflow = (temp == 0);
        setSZPFlagsWord(temp);
    }
    
    inline void CPU::decByte(byte &temp) {
        auxiliaryCarry = (lowNibble(temp) == 0);
        temp--;
        overflow = (temp == 0xFF);
        setSZPFlagsByte(temp);
    }
    
    inline void CPU::decWord(word &temp) {
        auxiliaryCarry = (lowNibble(temp) == 0);
        temp--;
        overflow = (temp == 0xFFFF);
        setSZPFlagsWord(temp);
    }

    inline void CPU::push(word value) {
        sp -= 2;
        address topOfStack = (((address)ss) << 4) + sp;
        memory.setWord(topOfStack, value);
    }
        
    inline word CPU::pop() {
        address topOfStack = (((address)ss) << 4) + sp;
        sp += 2;
        return memory.readWord(topOfStack);
    }
        
    inline void CPU::performInterrupt(byte type) {
        push(flags);
        interrupt = false;
        trace = false;
        push(cs);
        push(ip);
        cs = memory.readWord(type * 4 + 2);
        ip = memory.readWord(type * 4);
        
    }

    inline void CPU::debugPrint(byte opcode) {
        cout << "\t" << "AX " << hex << uppercase << setfill('0') << setw(4) << ax << dec;
        cout << " BX " << hex << uppercase << setfill('0') << setw(4) << bx << dec;
        cout << " CX " << hex << uppercase << setfill('0') << setw(4) << cx << dec;
        cout << " DX " << hex << uppercase << setfill('0') << setw(4) << Dx << dec;
        cout << " CPAZSTIDO" << endl;
        cout << "\t" << "SP " << hex << uppercase << setfill('0') << setw(4) << sp << dec;
        cout << " BP " << hex << uppercase << setfill('0') << setw(4) << bp << dec;
        cout << " SI " << hex << uppercase << setfill('0') << setw(4) << si << dec;
        cout << " DI " << hex << uppercase << setfill('0') << setw(4) << di << dec;
        cout << " " << carry << parity << auxiliaryCarry << zero << sign << trace << interrupt << direction << overflow << endl;
        cout << "\t" << "CS " << hex << uppercase << setfill('0') << setw(4) << cs << dec;
        cout << " DS " << hex << uppercase << setfill('0') << setw(4) << ds << dec;
        cout << " ES " << hex << uppercase << setfill('0') << setw(4) << es << dec;
        cout << " SS " << hex << uppercase << setfill('0') << setw(4) << ss << dec;
        cout << " IP " << hex << uppercase << setfill('0') << setw(4) << ip << dec << endl;
        cout << hex << uppercase << (int)opcode << dec;
        Instruction instr = instructions[opcode];
        string mnemonic = instr.mnemonic;
        if (mnemonic.substr(0, 3) == "GRP") {
            mnemonic = getGroupMnemonic(instr, memory.readByte(NEXT_INSTRUCTION + 1) >> 3 & 7);
        }
        cout << " " << mnemonic;
        cout << endl;
    }
    
    
    void CPU::reset() {
        cycleCount = 0;
        
        // reset registers
        ax = 0;
        bx = 0;
        cx = 0;
        Dx = 0;
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

    void CPU::hardwareInterrupt(byte info) {
        if (interrupt) {
            //byte baseVector = info & 0b11111000;
            //byte irq = info & 0b00000111;
            //performInterrupt(baseVector / 4 + irq);
            performInterrupt(info);
        }
    }
    
    // returns the number of cycles counted
    void CPU::step() {
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
                    #ifdef DEBUG
                    debugPrint(opcode);
                    #endif
                    lock = true;
                    ip += 1;
                    break;
                case 0xF3:
                    #ifdef DEBUG
                    debugPrint(opcode);
                    #endif
                    repeatCX = true;
                    ip += 1;
                    break;
                case 0xF2:
                    #ifdef DEBUG
                    debugPrint(opcode);
                    #endif
                    repeatZF = true;
                    ip += 1;
                    break;
                case 0x2E:
                    #ifdef DEBUG
                    debugPrint(opcode);
                    #endif
                    currentSegment = &cs;
                    ip += 1;
                    break;
                case 0x36:
                    #ifdef DEBUG
                    debugPrint(opcode);
                    #endif
                    currentSegment = &ss;
                    ip += 1;
                    break;
                case 0x3E:
                    #ifdef DEBUG
                    debugPrint(opcode);
                    #endif
                    currentSegment = &ds;
                    ip += 1;
                    break;
                case 0x26:
                    #ifdef DEBUG
                    debugPrint(opcode);
                    #endif
                    currentSegment = &es;
                    ip += 1;
                    break;
                default:
                    
                    goto actualOpcode;
            }
            opcode = memory.readByte(NEXT_INSTRUCTION);
            
        }
        
    actualOpcode:
        #ifdef DEBUG
        debugPrint(opcode);
        #endif
        switch (opcode) {
            
            // ADD integer addition
            case 0x00:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                byte temp = getModRMByte(mrr);
                addByte(temp, getRegByte(mrr.reg));
                setModRMByte(mrr, temp);
                break;
            }
            
            case 0x01:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                word temp = getModRMWord(mrr);
                addWord(temp, getRegWord(mrr.reg));
                setModRMWord(mrr, temp);
                break;
            }
            
            case 0x02:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                byte temp = getRegByte(mrr.reg);
                addByte(temp, getModRMByte(mrr));
                setRegByte(mrr.reg, temp);
                break;
            }
            
            case 0x03:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                word temp = getRegWord(mrr.reg);
                addWord(temp, getModRMWord(mrr));
                setRegWord(mrr.reg, temp);
                break;
            }
            
            case 0x04:
                addByte(al, memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                break;
                
            case 0x05:
                addWord(ax, memory.readWord(NEXT_INSTRUCTION + 1));
                instructionLength = 3;
                break;
                
            // PUSH es
            case 0x06:
                push(es);
                break;
            
            // POP es
            case 0x07:
                es = pop();
                break;
            
            // OR inclusive or
            case 0x08:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                byte temp = getModRMByte(mrr);
                orByte(temp, getRegByte(mrr.reg));
                setModRMByte(mrr, temp);
                break;
            }
                
            case 0x09:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                word temp = getModRMWord(mrr);
                orWord(temp, getRegWord(mrr.reg));
                setModRMWord(mrr, temp);
                break;
            }
                
            case 0x0A:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                byte temp = getRegByte(mrr.reg);
                orByte(temp, getModRMByte(mrr));
                setRegByte(mrr.reg, temp);
                break;
            }
            
            case 0x0B:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                word temp = getRegWord(mrr.reg);
                orWord(temp, getModRMWord(mrr));
                setRegWord(mrr.reg, temp);
                break;
            }
            
            case 0x0C:
                orByte(al, memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                break;
                
            case 0x0D:
                orWord(ax, memory.readWord(NEXT_INSTRUCTION + 1));
                instructionLength = 3;
                break;
            
            // PUSH cs
            case 0x0E:
                push(cs);
                break;
            
            // PUSH ss
            case 0x16:
                push(ss);
                break;
            
            // POP ss
            case 0x17:
                ss = pop();
                break;
            
            // PUSH ds
            case 0x1E:
                push(ds);
                break;
                
            // POP ds
            case 0x1F:
                ds = pop();
                break;
            
            // AND
            case 0x20:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                byte temp = getModRMByte(mrr);
                andByte(temp, getRegByte(mrr.reg));
                setModRMByte(mrr, temp);
                break;
            }
                
            case 0x21:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                word temp = getModRMWord(mrr);
                andWord(temp, getRegWord(mrr.reg));
                setModRMWord(mrr, temp);
                break;
            }
                
            case 0x22:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                byte temp = getRegByte(mrr.reg);
                andByte(temp, getModRMByte(mrr));
                setRegByte(mrr.reg, temp);
                break;
            }
            
            case 0x23:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                word temp = getRegWord(mrr.reg);
                andWord(temp, getModRMWord(mrr));
                setRegWord(mrr.reg, temp);
                break;
            }
            
            case 0x24:
                andByte(al, memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                break;
                
            case 0x25:
                andWord(ax, memory.readWord(NEXT_INSTRUCTION + 1));
                instructionLength = 3;
                break;
            
            // SUB integer subtraction
            case 0x28:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                byte temp = getModRMByte(mrr);
                subByte(temp, getRegByte(mrr.reg));
                setModRMByte(mrr, temp);
                break;
            }
            
            case 0x29:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                word temp = getModRMWord(mrr);
                subWord(temp, getRegWord(mrr.reg));
                setModRMWord(mrr, temp);
                break;
            }
            
            case 0x2A:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                byte temp = getRegByte(mrr.reg);
                subByte(temp, getModRMByte(mrr));
                setRegByte(mrr.reg, temp);
                break;
            }
            
            case 0x2B:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                word temp = getRegWord(mrr.reg);
                subWord(temp, getModRMWord(mrr));
                setRegWord(mrr.reg, temp);
                break;
            }
            
            case 0x2C:
                subByte(al, memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                break;
                
            case 0x2D:
                subWord(ax, memory.readWord(NEXT_INSTRUCTION + 1));
                instructionLength = 3;
                break;
            
            // XOR exclusive or
            case 0x30:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                byte temp = getModRMByte(mrr);
                xorByte(temp, getRegByte(mrr.reg));
                setModRMByte(mrr, temp);
                break;
            }
                
            case 0x31:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                word temp = getModRMWord(mrr);
                xorWord(temp, getRegWord(mrr.reg));
                setModRMWord(mrr, temp);
                break;
            }
                
            case 0x32:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                byte temp = getRegByte(mrr.reg);
                xorByte(temp, getModRMByte(mrr));
                setRegByte(mrr.reg, temp);
                break;
            }
            
            case 0x33:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                word temp = getRegWord(mrr.reg);
                xorWord(temp, getModRMWord(mrr));
                setRegWord(mrr.reg, temp);
                break;
            }
            
            case 0x34:
                xorByte(al, memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                break;
            case 0x35:
                xorWord(ax, memory.readWord(NEXT_INSTRUCTION + 1));
                instructionLength = 3;
                break;
            
            // CMP Memory/Reg with Reg byte
            case 0x38:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                byte temp = getModRMByte(mrr);
                subByte(temp, getRegByte(mrr.reg));
                break;
            }
                
            // CMP Memory/Reg with Reg word
            case 0x39:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                word temp = getModRMWord(mrr);
                subWord(temp, getRegWord(mrr.reg));
                break;
            }
                
            // CMP Reg w/ Memory/Reg byte
            case 0x3A:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                byte temp = getRegByte(mrr.reg);
                subByte(temp, getModRMByte(mrr));
                break;
            }
            
            // CMP Reg w/ Memory/Reg word
            case 0x3B:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                word temp = getRegWord(mrr.reg);
                subWord(temp, getModRMWord(mrr));
                break;
            }
                
            // CMP Immed w/ AL
            case 0x3C:
            {
                instructionLength = 2;
                byte temp = al;
                subByte(temp, memory.readByte(NEXT_INSTRUCTION + 1));
                break;
            }
                
            // CMP Immed w/ AX
            case 0x3D:
            {
                instructionLength = 3;
                word temp = ax;
                subWord(temp, memory.readWord(NEXT_INSTRUCTION + 1));
                break;
            }
                
            // INC AX
            case 0x40:
                incWord(ax);
                break;
            
            // INC CX
            case 0x41:
                incWord(cx);
                break;
            
            // INC DX
            case 0x42:
                incWord(Dx);
                break;
            
            // INC BX
            case 0x43:
                incWord(bx);
                break;
            
            // INC SP
            case 0x44:
                incWord(sp);
                break;
            
            // INC BP
            case 0x45:
                incWord(bp);
                break;
            
            // INC SI
            case 0x46:
                incWord(si);
                break;
            
            // INC DI
            case 0x47:
                incWord(di);
                break;
            
            // DEC AX
            case 0x48:
                decWord(ax);
                break;
            
            // DEC CX
            case 0x49:
                decWord(cx);
                break;
            
            // DEC DX
            case 0x4A:
                decWord(Dx);
                break;
            
            // DEC BX
            case 0x4B:
                decWord(bx);
                break;
            
            // DEC SP
            case 0x4C:
                decWord(sp);
                break;
            
            // DEC BP
            case 0x4D:
                decWord(bp);
                break;
            
            // DEC SI
            case 0x4E:
                decWord(si);
                break;
            
            // DEC DI
            case 0x4F:
                decWord(di);
                break;
            
            // PUSH ax
            case 0x50:
                push(ax);
                break;
            
            // PUSH cx
            case 0x51:
                push(cx);
                break;
            
            // PUSH dx
            case 0x52:
                push(Dx);
                break;
            
            // PUSH bx
            case 0x53:
                push(bx);
                break;
                
            // PUSH sp
            case 0x54:
                push(sp);
                break;
                
            // PUSH bp
            case 0x55:
                push(bp);
                break;
                
            // PUSH si
            case 0x56:
                push(si);
                break;
                
            // PUSH di
            case 0x57:
                push(di);
                break;
            
            // POP ax
            case 0x58:
                ax = pop();
                break;
            
            // POP cx
            case 0x59:
                cx = pop();
                break;
            
            // POP dx
            case 0x5A:
                Dx = pop();
                break;
            
            // POP bx
            case 0x5B:
                bx = pop();
                break;
                
            // POP sp
            case 0x5C:
                sp = pop();
                break;
                
            // POP bp
            case 0x5D:
                bp = pop();
                break;
                
            // POP si
            case 0x5E:
                si = pop();
                break;
                
            // POP di
            case 0x5F:
                di = pop();
                break;
            
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
                
            case 0x80:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                switch (mrr.reg) {
                    case 0b000: // ADD
                    {
                        byte temp = getModRMByte(mrr);
                        addByte(temp, memory.readByte(NEXT_INSTRUCTION + instructionLength));
                        instructionLength += 1;
                        setModRMByte(mrr, temp);
                        break;
                    }
                    case 0b001: // OR
                    {
                        byte temp = getModRMByte(mrr);
                        orByte(temp, memory.readByte(NEXT_INSTRUCTION + instructionLength));
                        instructionLength += 1;
                        setModRMByte(mrr, temp);
                        break;
                    }
                    case 0b010:
                        cout << "unimplemented";
                        break;
                    case 0b011:
                        cout << "unimplemented";
                        break;
                    case 0b100: // AND
                    {
                        byte temp = getModRMByte(mrr);
                        andByte(temp, memory.readByte(NEXT_INSTRUCTION + instructionLength));
                        instructionLength += 1;
                        setModRMByte(mrr, temp);
                        break;
                    }
                    case 0b101: // SUB
                    {
                        byte temp = getModRMByte(mrr);
                        subByte(temp, memory.readByte(NEXT_INSTRUCTION + instructionLength));
                        instructionLength += 1;
                        setModRMByte(mrr, temp);
                        break;
                    }
                    case 0b110: // XOR
                    {
                        byte temp = getModRMByte(mrr);
                        xorByte(temp, memory.readByte(NEXT_INSTRUCTION + instructionLength));
                        instructionLength += 1;
                        setModRMByte(mrr, temp);
                        break;
                    }
                    case 0b111: // CMP
                        byte temp = getModRMByte(mrr);
                        subByte(temp, memory.readByte(NEXT_INSTRUCTION + instructionLength));
                        instructionLength += 1;
                        break;
                        
                }
                break;
            }
            
            case 0x81:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                switch (mrr.reg) {
                    case 0b000:  // ADD
                    {
                        word temp = getModRMWord(mrr);
                        addWord(temp, memory.readWord(NEXT_INSTRUCTION + instructionLength));
                        instructionLength += 2;
                        setModRMWord(mrr, temp);
                        break;
                    }
                    case 0b001: // OR
                    {
                        word temp = getModRMWord(mrr);
                        orWord(temp, memory.readWord(NEXT_INSTRUCTION + instructionLength));
                        instructionLength += 2;
                        setModRMWord(mrr, temp);
                        break;
                    }
                    case 0b010:
                        cout << "unimplemented";
                        break;
                    case 0b011:
                        cout << "unimplemented";
                        break;
                    case 0b100:  // AND
                        {
                            word temp = getModRMWord(mrr);
                            andWord(temp, memory.readWord(NEXT_INSTRUCTION + instructionLength));
                            instructionLength += 2;
                            setModRMWord(mrr, temp);
                            break;
                        }
                    case 0b101: // SUB
                    {
                        word temp = getModRMWord(mrr);
                        subWord(temp, memory.readWord(NEXT_INSTRUCTION + instructionLength));
                        instructionLength += 2;
                        setModRMWord(mrr, temp);
                        break;
                    }
                    case 0b110: // XOR
                    {
                        word temp = getModRMWord(mrr);
                        xorWord(temp, memory.readWord(NEXT_INSTRUCTION + instructionLength));
                        instructionLength += 2;
                        setModRMWord(mrr, temp);
                        break;
                    }
                    case 0b111: // CMP
                        word temp = getModRMWord(mrr);
                        subWord(temp, memory.readWord(NEXT_INSTRUCTION + instructionLength));
                        instructionLength += 2;
                        break;
                        
                }
                break;
            }
            
            case 0x83:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                switch (mrr.reg) {
                    case 0b000: // ADD
                    {
                        word temp = getModRMWord(mrr);
                        addWord(temp, signExtend(memory.readByte(NEXT_INSTRUCTION + instructionLength)));
                        instructionLength += 1;
                        setModRMWord(mrr, temp);
                        break;
                    }
                    case 0b001:
                    {
                        cout << "unimplemented";
                        break;
                    }
                    case 0b010:
                        cout << "unimplemented";
                        break;
                    case 0b011:
                        cout << "unimplemented";
                        break;
                    case 0b100:
                        break;
                    case 0b101: // SUB
                    {
                        word temp = getModRMWord(mrr);
                        subWord(temp, signExtend(memory.readByte(NEXT_INSTRUCTION + instructionLength)));
                        instructionLength += 1;
                        setModRMWord(mrr, temp);
                        break;
                    }
                    case 0b110:
                    {
                        cout << "unimplemented";
                        break;
                    }
                    case 0b111: // CMP
                        word temp = getModRMWord(mrr);
                        subWord(temp, signExtend(memory.readByte(NEXT_INSTRUCTION + instructionLength)));
                        instructionLength += 1;
                        break;
                        
                }
                break;
            }
            
            // XCHG byte reg to modrm
            case 0x86:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                byte temp = getModRMByte(mrr);
                setModRMByte(mrr, getRegByte(mrr.reg));
                setRegByte(mrr.reg, temp);
                break;
            }
            
            // XCHG word reg to modrm
            case 0x87:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                word temp = getModRMWord(mrr);
                setModRMWord(mrr, getRegWord(mrr.reg));
                setRegWord(mrr.reg, temp);
                break;
            }
                
            // MOV byte reg to rm
            case 0x88:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                setModRMByte(mrr, getRegByte(mrr.reg));
                break;
            }
            
            // MOV word reg to rm
            case 0x89:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                setModRMWord(mrr, getRegWord(mrr.reg));
                break;
            }
            
            // MOV byte rm to reg
            case 0x8A:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                setRegByte(mrr.reg, getModRMByte(mrr));
                break;
            }
            
            // MOV word rm to reg
            case 0x8B:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                setRegWord(mrr.reg, getModRMWord(mrr));
                break;
            }
                
            // MOV segment register to rm
            case 0x8C:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                setModRMWord(mrr, getSegmentRegWord(mrr.reg));
                break;
            }
            
            // MOV rm to segment register
            case 0x8E:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                setSegmentRegWord(mrr.reg, getModRMWord(mrr));
                break;
            }
            
            // POP
            case 0x8F:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                setModRMWord(mrr, pop());
                break;
            }
                
            // NOP no op (technically XCHG AX with AX)
            case 0x90:
                break;
            
            // XCHG AX w/ CX
            case 0x91:
            {
                word temp = ax;
                ax = cx;
                cx = temp;
                break;
            }
            
            // XCHG AX w/ DX
            case 0x92:
            {
                word temp = ax;
                ax = Dx;
                Dx = temp;
                break;
            }
            
            // XCHG AX w/ BX
            case 0x93:
            {
                word temp = ax;
                ax = bx;
                bx = temp;
                break;
            }
            
            // XCHG AX w/ SP
            case 0x94:
            {
                word temp = ax;
                ax = sp;
                sp = temp;
                break;
            }
            
            // XCHG AX w/ BP
            case 0x95:
            {
                word temp = ax;
                ax = bp;
                bp = temp;
                break;
            }
            
            // XCHG AX w/ SI
            case 0x96:
            {
                word temp = ax;
                ax = si;
                si = temp;
                break;
            }
            
            // XCHG AX w/ DI
            case 0x97:
            {
                word temp = ax;
                ax = di;
                di = temp;
                break;
            }
            
            // CWD Convert Word to Doubleword
            case 0x99:
                if ((ax & 0x8000) == 0x8000) {
                    Dx = 0xFFFF;
                } else {
                    Dx = 0;
                }
                break;
            
            // CALL direct
            case 0x9A:
            { // scope for new variables
                instructionLength = 5;
                push(cs);
                push(ip + instructionLength);
                // next two instructions are new ip
                // can't change ip until after have read CS
                word nextIp = memory.readWord(NEXT_INSTRUCTION + 1);
                // next two after that are new cs
                word nextCs = memory.readWord(NEXT_INSTRUCTION + 3);
                ip = nextIp;
                cs = nextCs;
                jump = true;
                break;
            }
            
            // PUSHF
            case 0x9C:
                push(flags);
                break;
            
            // POPF
            case 0x9D:
                flags = pop();
                break;
            
            // SAHF store AH into lower byte of flags
            case 0x9E:
                flags = ((flags & 0xFF00) | ah);
                break;
            
            // LAHF copy low byte of flags word to AH
            case 0x9F:
                ah = ((byte) (flags & 0x00FF));
                break;
            
            // MOV mem byte to AL
            case 0xA0:
            {
                instructionLength = 3;
                word location = memory.readWord(ip + 1);
                address pa = ((*currentSegment << 4) + location); // physical address
                al = memory.readByte(pa);
                break;
            }
            
            // MOV mem word to AX
            case 0xA1:
            {
                instructionLength = 3;
                word location = memory.readWord(ip + 1);
                address pa = ((*currentSegment << 4) + location); // physical address
                ax = memory.readWord(pa);
                break;
            }
            
            // MOV AL to mem byte
            case 0xA2:
            {
                instructionLength = 3;
                word location = memory.readWord(ip + 1);
                address pa = ((*currentSegment << 4) + location); // physical address
                memory.setByte(pa, al);
                break;
            }
            
            // MOV AX to mem word
            case 0xA3:
            {
                instructionLength = 3;
                word location = memory.readWord(ip + 1);
                address pa = ((*currentSegment << 4) + location); // physical address
                memory.setWord(pa, ax);
                break;
            }
                
            // TEST AL & immediate
            case 0xA8:
            {
                byte temp = memory.readByte(NEXT_INSTRUCTION + 1);
                instructionLength = 2;
                byte result = al & temp;
                setSZPFlagsByte(result);
                carry = false;
                overflow = false;
                break;
            }
            
            // TEST AX & immediate
            case 0xA9:
            {
                word temp = memory.readWord(NEXT_INSTRUCTION + 1);
                instructionLength = 3;
                word result = al & temp;
                setSZPFlagsWord(result);
                carry = false;
                overflow = false;
                break;
            }
            
            // STOSB store string byte
            case 0xAA:
            {
                repAA:
                address place = (es << 4) + di;
                cout << place <<  " : ";
                cout << hex << uppercase << setfill('0') << setw(2) << al << endl;
                memory.setByte(place, al);
                if (direction == 0) {
                    di++;
                } else {
                    di--;
                }
                
                if (repeatCX) {
                    cx--;
                    if (cx > 0) {
                        goto repAA;
                    }
                }
                
                break;
            }
            
            // STOSW store string word
            case 0xAB:
            {
                repAB:
                address place = (es << 4) + di;
                memory.setWord(place, ax);
                if (direction == 0) {
                    di += 2;
                } else {
                    di -= 2;
                }
                
                if (repeatCX) {
                    cx--;
                    if (cx > 0) {
                        goto repAB;
                    }
                }
                
                break;
            }
            
            // LODSB load string byte
            case 0xAC:
            {
                repAC:
                address place = (*currentSegment << 4) + si;
                cout << place <<  " : ";
                cout << hex << uppercase << setfill('0') << setw(2) << memory.readByte(place) << endl;
                al = memory.readByte(place);
                if (direction == 0) {
                    si++;
                } else {
                    si--;
                }
                
                if (repeatCX) {
                    cx--;
                    if (cx > 0) {
                        goto repAC;
                    }
                }
                
                break;
            }
            
            // LODSW load string word
            case 0xAD:
            {
                repAD:
                address place = (ds << 4) + si;
                ax = memory.readWord(place);
                if (direction == 0) {
                    si += 2;
                } else {
                    si -= 2;
                }
                
                if (repeatCX) {
                    cx--;
                    if (cx > 0) {
                        goto repAD;
                    }
                }
                
                break;
            }
                
            // MOV Byte move data to register
            case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB7:
                setReg(lowNibble(opcode), memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                break;
            
            // MOV Word move data to register
            case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBE: case 0xBF:
                setReg(lowNibble(opcode), memory.readWord(NEXT_INSTRUCTION + 1));
                instructionLength = 3;
                break;
                
            // RET intrasegment and add displacement
            case 0xC2:
            {
                word displacement = memory.readWord(NEXT_INSTRUCTION + 1);
                jump = true;
                ip = pop();
                sp += displacement;
                break;
            }
            
            // RET intra-segment
            case 0xC3:
                jump = true;
                ip = pop();
                cout << ip << endl;
                break;
            
            // MOV immediate byte to rm
            case 0xC6:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                byte data = memory.readByte(NEXT_INSTRUCTION + instructionLength);
                instructionLength += 1;
                setModRMByte(mrr, data);
                break;
            }
            
            // MOV immediate word to rm
            case 0xC7:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                word data = memory.readWord(NEXT_INSTRUCTION + instructionLength);
                instructionLength += 2;
                setModRMWord(mrr, data);
                break;
            }
            
            // RET intersegment and add displacement
            case 0xCA:
            {
                word displacement = memory.readWord(NEXT_INSTRUCTION + 1);
                jump = true;
                ip = pop();
                cs = pop();
                sp += displacement;
                break;
            }
            
            // RET intra-segment
            case 0xCB:
                jump = true;
                ip = pop();
                cs = pop();
                break;
            
            // INT (always 3)
            case 0xCC:
                jump = true;
                performInterrupt(3);
                break;
            
            // INT type
            case 0xCD:
            {
                instructionLength = 2;
                byte type = memory.readByte(NEXT_INSTRUCTION + 1);
                jump = true;
                performInterrupt(type);
                break;
            }
                
            // INTO overflow interrupt
            case 0xCE:
                if (overflow) {
                    jump=true;
                    performInterrupt(4);
                }
                break;
                
                
            // IRET
            case 0xCF:
                jump = true;
                ip = pop();
                cs = pop();
                flags = pop();
                break;
            
            // ROL/ROR/RCL/RCR/SHL/SHR/SAR/ROL 8 bits 1
            case 0xD0:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                switch (mrr.reg) {
                    case 0b000:
                        rolByte(mrr, 1);
                        break;
                    case 0b001:
                        rorByte(mrr, 1);
                        break;
                    case 0b010:
                        rclByte(mrr, 1);
                        break;
                    case 0b011:
                        rcrByte(mrr, 1);
                        break;
                    case 0b100:
                        shlByte(mrr, 1);
                        break;
                    case 0b101:
                        shrByte(mrr, 1);
                        break;
                    case 0b110:
                        cout << "Unused opcode extension 110 for 0xD2" << endl;
                        break;
                    case 0b111:
                        sarByte(mrr, 1);
                        break;
                        
                }
                break;
            }
                
                // ROL/ROR/RCL/RCR/SHL/SHR/SAR/ROL 16 bits 1
            case 0xD1:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                switch (mrr.reg) {
                    case 0b000:
                        rolWord(mrr, 1);
                        break;
                    case 0b001:
                        rorWord(mrr, 1);
                        break;
                    case 0b010:
                        rclWord(mrr, 1);
                        break;
                    case 0b011:
                        rcrWord(mrr, 1);
                        break;
                    case 0b100:
                        shlWord(mrr, 1);
                        break;
                    case 0b101:
                        shrWord(mrr, 1);
                        break;
                    case 0b110:
                        cout << "Unused opcode extension 110 for 0xD3" << endl;
                        break;
                    case 0b111:
                        sarWord(mrr, 1);
                        break;
                        
                }
                break;
            }
                
            // ROL/ROR/RCL/RCR/SHL/SHR/SAR/ROL 8 bits CL
            case 0xD2:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                switch (mrr.reg) {
                    case 0b000:
                        rolByte(mrr, cl);
                        break;
                    case 0b001:
                        rorByte(mrr, cl);
                        break;
                    case 0b010:
                        rclByte(mrr, cl);
                        break;
                    case 0b011:
                        rcrByte(mrr, cl);
                        break;
                    case 0b100:
                        shlByte(mrr, cl);
                        break;
                    case 0b101:
                        shrByte(mrr, cl);
                        break;
                    case 0b110:
                        cout << "Unused opcode extension 110 for 0xD2" << endl;
                        break;
                    case 0b111:
                        sarByte(mrr, cl);
                        break;
                    
                }
                break;
            }
                
            // ROL/ROR/RCL/RCR/SHL/SHR/SAR/ROL 16 bits CL
            case 0xD3:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                switch (mrr.reg) {
                    case 0b000:
                        rolWord(mrr, cl);
                        break;
                    case 0b001:
                        rorWord(mrr, cl);
                        break;
                    case 0b010:
                        rclWord(mrr, cl);
                        break;
                    case 0b011:
                        rcrWord(mrr, cl);
                        break;
                    case 0b100:
                        shlWord(mrr, cl);
                        break;
                    case 0b101:
                        shrWord(mrr, cl);
                        break;
                    case 0b110:
                        cout << "Unused opcode extension 110 for 0xD3" << endl;
                        break;
                    case 0b111:
                        sarWord(mrr, cl);
                        break;
                        
                }
                break;
            }
                
            // LOOPNE, LOOPNZ branch if CX non-zero and ZF = 0
            case 0xE0:
                cx--;
                if (cx != 0 && zero == false) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // LOOPE, LOOPZ branch if CX non-zero and ZF = 1
            case 0xE1:
                cx--;
                if (cx != 0 && zero == true) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // LOOP branch if CX non-zero
            case 0xE2:
                cx--;
                if (cx != 0) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
                
            
            // JCXZ Jump if CX is zero
            case 0xE3:
                if (cx == 0) {
                    ip = (ip + 2) + signExtend(memory.readByte(NEXT_INSTRUCTION + 1));
                    jump = true;
                }
                instructionLength = 2;
                break;
            
            // IN fixed port to AL
            case 0xE4:
                instructionLength = 2;
                al = pc.readPort(memory.readByte(NEXT_INSTRUCTION + 1));
                break;
            
            // IN fixed port to AX
            case 0xE5:
                instructionLength = 2;
                ax = pc.readPort(memory.readByte(NEXT_INSTRUCTION + 1));
                break;
            
            // OUT from al
            case 0xE6:
                instructionLength = 2;
                pc.writePort(memory.readByte(NEXT_INSTRUCTION + 1), al);
                break;
            
            // OUT from ax
            case 0xE7:
                instructionLength = 2;
                pc.writePort(memory.readByte(NEXT_INSTRUCTION + 1), ax);
                break;
                
            // CALL within segment or group, ip relative; 16 bit displacement
            case 0xE8:
            {
                instructionLength = 3;
                push(ip + instructionLength);
                cout << (ip + instructionLength) << endl;
                word displacement = memory.readWord(NEXT_INSTRUCTION + 1);
                ip += (displacement + instructionLength);
                jump = true;
                break;
            }
                
            // JMP within segment or group, ip relative; 16 bit displacement
            case 0xE9:
            {
                instructionLength = 3;
                word displacement = memory.readWord(NEXT_INSTRUCTION + 1);
                ip += (displacement + instructionLength);
                jump = true;
                break;
            }
                
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
                
            // JMP within segment or group, ip relative; 8 bit displacement sign extend
            case 0xEB:
            {
                instructionLength = 2;
                byte displacement = memory.readByte(NEXT_INSTRUCTION + 1);
                ip += signExtend(displacement);
                break;
            }
            
            // IN variable port (DX) to al
            case 0xEC:
                al = pc.readPort(Dx);
                break;
            
            // IN variable port (DX) to ax
            case 0xED:
                ax = pc.readPort(Dx);
                break;
                
            // OUT to Dx from al
            case 0xEE:
                pc.writePort(Dx, al);
                break;
            
            // OUT to Dx from ax
            case 0xEF:
                pc.writePort(Dx, ax);
                break;
                
            // CMC Complement Carry Flag
            case 0xF5:
                carry = !carry;
                break;
            
            
            case 0xF6:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                switch (mrr.reg) {
                    case 0b000: // TEST
                    {
                        byte temp1 = getModRMByte(mrr);
                        byte temp2 = memory.readByte(NEXT_INSTRUCTION + instructionLength);
                        instructionLength += 1;
                        byte result = temp1 & temp2;
                        setSZPFlagsByte(result);
                        carry = false;
                        overflow = false;
                        break;
                    }
                    case 0b010: // // NOT one's complement (invert 1s and 0s) byte
                    {
                        setModRMByte(mrr, ~getModRMByte(mrr));
                        break;
                    }
                    default:
                    {
                        cout << "unimplemented F6 opcode" << endl;
                        break;
                    }
                }
                break;
                
                
            }
            
            case 0xF7:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                switch (mrr.reg) {
                    case 0b000: // TEST word
                    {
                        word temp1 = getModRMWord(mrr);
                        word temp2 = memory.readWord(NEXT_INSTRUCTION + instructionLength);
                        instructionLength += 2;
                        word result = temp1 & temp2;
                        setSZPFlagsWord(result);
                        carry = false;
                        overflow = false;
                        break;
                    }
                    case 0b010: // // NOT one's complement (invert 1s and 0s) word
                    {
                        setModRMWord(mrr, ~getModRMWord(mrr));
                        break;
                    }
                    default:
                    {
                        cout << "unimplemented F7 opcode" << endl;
                        break;
                    }
                }
                break;
                
                
            }
                
            // CLC clear carry flag
            case 0xF8:
                carry = false;
                break;
            
            // STC set carry flag
            case 0xF9:
                carry = true;
                break;
            
            //
            
            // CLI clear interrupt
            case 0xFA:
                interrupt = false;
                break;
            
            // STI set interrupt flag
            case 0xFB:
                interrupt = true;
                break;
            
            // CLD clear direction flag
            case 0xFC:
                direction = false;
                break;
            
            // STD set direction flag
            case 0xFD:
                direction = true;
                break;
                
            
            case 0xFE:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                switch (mrr.reg) {
                    case 0b000: // INC increment byte by 1
                    {
                        byte temp = getModRMByte(mrr);
                        incByte(temp);
                        setModRMByte(mrr, temp);
                        break;
                    }
                    case 0b001: // DEC decrement byte by 1
                    {
                        byte temp = getModRMByte(mrr);
                        decByte(temp);
                        setModRMByte(mrr, temp);
                        break;
                    }
                    default:
                    {
                        cout << "unimplemented FE opcode" << endl;
                        break;
                    }
                }
                break;
            }
            
            case 0xFF:
            {
                ModRegRM mrr = ModRegRM(memory.readByte(NEXT_INSTRUCTION + 1));
                instructionLength = 2;
                modInstructionLength(mrr, instructionLength);
                switch (mrr.reg) {
                    case 0b000: // INC increment word by 1
                    {
                        word temp = getModRMWord(mrr);
                        incWord(temp);
                        setModRMWord(mrr, temp);
                        break;
                    }
                    case 0b001: // DEC decrement word by 1
                    {
                        word temp = getModRMWord(mrr);
                        decWord(temp);
                        setModRMWord(mrr, temp);
                        break;
                    }
                    case 0b010: // CALL within segment indirect
                    {
                        push(ip + instructionLength);
                        word temp = getModRMWord(mrr);
                        ip = temp;
                        jump = true;
                        break;
                    }
                    case 0b011: // CALL inter-segment indirect
                    {
                        push(cs);
                        push(ip + instructionLength);
                        // next two instructions are new ip
                        // can't change ip until after have read CS
                        address ea = calcEffectiveAddress(mrr);
                        address pa = ((*currentSegment << 4) + ea); // physical address
                        ip = memory.readWord(pa);
                        cs = memory.readWord(pa + 2);
                        jump = true;
                        break;
                    }
                    case 0b110: // PUSH modrm
                        push(getModRMWord(mrr));
                        break;
                    default:
                    {
                        cout << "unimplemented FF opcode" << endl;
                        break;
                    }
                }
                break;
            }
                
                
            // Unknown Opcode
            default:
                cout << "Unknown opcode!" << endl;
        }
        
        // if we didn't jump, move the instruction pointer forward
        if (!jump) { ip += instructionLength; }
        
        // dummy for now until we have accurate cycle counts
        cycleCount += 1;
        
        // sanity check
        //cout << hex << uppercase << (int)memory.readByte(90095) << dec << endl;
    }
    
}
