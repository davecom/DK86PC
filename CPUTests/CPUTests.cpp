//
//  CPUTests.cpp
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

#include "catch.hpp"
#include "Memory.hpp"
#include "CPU.hpp"
#include "DummyPortInterface.hpp"
#include "Types.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>

using namespace std;
using namespace DK86PC;

vector<uint8_t> loadBin(string filename) {
    // open input stream
    ifstream input(filename, ios::in | ios::binary);
    // check if the file opened successfully
    if (!input.is_open() || input.fail()) {
        cout << filename << " can't be opened!";
    }
    
    vector<uint8_t> buffer;
    copy(istreambuf_iterator<char>(input),
         istreambuf_iterator<char>(),
         back_inserter(buffer));
    
    return buffer;
}


TEST_CASE( "artlav CPU Tests" ) {
    
    
    // passing tests "rotate", "add", "sub", "jump1", "jump2", "bitwise", "control", "cmpneg", "rep", "shifts", "strings", "interrupt", "jmpmov", "datatrnf"
    // failing tests "mul", "div", "segpr", "bcdconv"
    auto name = GENERATE(as<std::string>{}, "rotate", "add", "sub", "jump1", "jump2", "bitwise", "control", "cmpneg", "rep", "shifts", "strings", "interrupt", "jmpmov", "datatrnf");
    
    DYNAMIC_SECTION( "Instructions: " << name ) {
        Memory memory = Memory();
        memory.loadBIOS("80186_tests/" + name + ".bin");
//        memory.setWatch(0x10000 + 0x2501);
//        memory.setWatch(0x10000 + 0x2600);
//        memory.setWatch(0x2A);
//        memory.setWatch(22);
//        memory.setWatch(23);
//        memory.setWatch(24);
//        memory.setWatch(25);
//        memory.setWatch(26);
        DummyPortInterface dpi = DummyPortInterface();
        CPU cpu = CPU(dpi, memory);
        // default flags for later x86 CPUs expected by tests
        cpu.setTestingFlags(0b0000000000000010);
        // tests otherwise go off the end of the 1 MB of memory with first reset vector jmp
        cpu.setCSIP(0xF000, 0xFFF0);
        while (!cpu.isHalted()) {
            cpu.step();
        }
        // special test case without result
        if (name == "jmpmov") {
            INFO("Testing byte 0 of Memory for jmpmov test");
            CHECK((uint8_t)memory.readByte(0) == (uint8_t)0x4001);
            return;
        }
        // normal test cases
        vector<uint8_t> expectedResult = loadBin("80186_tests/res_" + name + ".bin");
        for (int i = 0; i < expectedResult.size(); i++) {
            INFO("Testing byte #" << i);
            uint8_t expected = (uint8_t)expectedResult[i];
            uint8_t actual = (uint8_t)memory.readByte(i);
            CHECK(int(expected) == int(actual));
        }
    }
}
