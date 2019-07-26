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
    
#define NEXT_INSTRUCTION ((((address)cs) << 4) + ip)
    
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
        byte instructionLength = 1;
        
        byte opcode = memory.readByte(NEXT_INSTRUCTION);
        cout << hex << uppercase << (int)opcode << dec << endl;
        
        switch (opcode) {
            case 0xEA: // JMP direct
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
                
            case 0xFA: // CLI clear interrupt
                interrupt = false;
                break;
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
