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

using namespace std;

namespace DK86PC {
    
#define NEXT_INSTRUCTION ((((address)cs) << 4) + ip)
    
    void CPU::reset() {
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
        ip = 0; // reset vector
        flags = 0xF000;
    }
    
    void CPU::step() {
        bool jump = false;
        byte instructionLength = 1;
        
        byte opcode = memory.readByte(NEXT_INSTRUCTION);
        cout << hex << opcode << endl;
        
        switch (opcode) {
            case 0xEA: // JMP direct
                // next two instructions are new ip
                ip = memory.readWord(NEXT_INSTRUCTION + 1);
                // next two after that are new cs
                cs = memory.readWord(NEXT_INSTRUCTION + 3);
                jump = true;
                instructionLength = 5;
                break;
            default:
                cout << "Unknown opcode!" << endl;
                break;
        }
        
        // if we didn't jump, move the instruction pointer forward
        if (!jump) { ip += instructionLength; }
    }
    
}
