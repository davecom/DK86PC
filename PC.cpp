//
//  PC.cpp
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

#include <string>
#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include "PC.hpp"
#include <SDL.h>

using namespace std;

#define MILLI_PER_FRAME 16

namespace DK86PC {
    
    // On the original PC BIOS exists from 0xE0000 to 0xFFFFF
    // so implicitly must be <= 128k
    void PC::loadBIOS(string filename) {
        
        // open input stream
        ifstream input(filename, ios::in | ios::binary);
        // check if the file opened successfully
        if (!input.is_open() || input.fail()) {
            cout << "BIOS File " << filename << " can't be opened!";
            return;
        }
        
        vector<byte> buffer;
        copy(istreambuf_iterator<char>(input),
             istreambuf_iterator<char>(),
             back_inserter(buffer));
        
        // in original IBM PC BIOS is right before end of 1 MB of memory
        address biosPlace = 0x100000 - (address) buffer.size();
        memory.loadData(buffer, biosPlace);
    }
    

    void PC::run() {
        static long long numFrames = 0;
        // keep going until we hit an illegal/unknown instruction
        while (cpu.step() != -2) {
            if (SDL_GetTicks() / MILLI_PER_FRAME > numFrames) { // roughly 60 fps
                cga.renderScren();
                numFrames++;
                
                if (numFrames %60 == 0) {
                    cga.verticalRetraceStart();
                } else if (numFrames %60 == 1) {
                    cga.verticalRetraceEnd();
                }
            }
            
            
            SDL_Event e;
            
            while (SDL_PollEvent(&e) != 0) {
                switch (e.type) {
                    case SDL_QUIT:
                        goto quit;
                        break;
                    case SDL_KEYDOWN:
                        switch (e.key.keysym.sym) {
                            case SDLK_x:
                                break;
                            default:
                                break;
                        }
                        break;
                    case SDL_KEYUP:
                        switch (e.key.keysym.sym) {
                            case SDLK_x:
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        quit:
        return;
    }

    void PC::writePort(word port, word value) {
        switch (port) {
            case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
            {
                byte channel = (byte) port / 2;
                if (port % 2 == 0) {
                    dma.setAddress(channel, value);
                } else {
                    dma.setCounter(channel, value);
                }
                break;
            }
            case 0x83:
                dma.setPage(1, value);
                break;
            case 0x61:
                ppi.setB(value);
                break;
            case 0x63:
                ppi.setControl(value);
                break;
            case 0xA0:
                pic2.setCommand(value);
                break;
            case 0xA1:
                pic2.setStatus(value);
                break;
            case 0x3D8:
                cga.setMode(value);
                break;
            default:
                cout << "Port " << port << " not implemented for writing!" << endl;
                break;
        }
    }

    word PC::readPort(word port) {
        switch (port) {
            case 0x3DA:
                return cga.getStatus();
            default:
                cout << "Port " << port << " not implemented for reading!" << endl;
                return 0;
        }
    }
}
