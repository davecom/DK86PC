//
//  PC.hpp
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

#ifndef PC_hpp
#define PC_hpp

#include <string>
#include "CPU.hpp"
#include "Memory.hpp"
#include "DMA.hpp"
#include "PIC.hpp"
#include "PPI.hpp"
#include "PIT.hpp"
#include "CGA.hpp"
#include "FDC.hpp"
#include "PortInterface.hpp"

using namespace std;

namespace DK86PC {
    class CPU;

    class PC: PortInterface {
    public:
        PC() : memory(), cpu(*this, memory), dma(), pic(), ppi(pic), pit(pic), cga(memory, ppi), fdc(pic) {
#ifdef DEBUG
            //memory.setWatch(90094);
            //memory.setWatch(00000);
            //memory.setWatch(0x3E4);
            //memory.setWatch(0x415);
#endif
            
        };
        void loadBIOS(string filename);
        void loadCasetteBASIC(string filename1, string filename2, string filename3, string filename4);
        void runLoop();
        void run();
        void writePort(word port, word value) override;
        word readPort(word port) override;
    private:
        bool shouldQuit = false;
        Memory memory;
        CPU cpu;
        DMA dma;
        PIC pic; // 0x20, 0x21 ports
        //PIC pic2; // 0xA0, 0xA1 ports // 5150 has only 1
        PPI ppi;
        PIT pit;
        CGA cga;
        FDC fdc;
    };
    
}

#endif /* PC_hpp */
