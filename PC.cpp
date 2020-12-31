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

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include "PC.hpp"
#include <SDL.h>

using namespace std;



namespace DK86PC {
    
    // On the original PC BIOS exists from 0xE0000 to 0xFFFFF
    // so implicitly must be <= 128k
    void PC::loadBIOS(string filename) {
        memory.loadBIOS(filename);
    }
    
    static int cgaThreadHelper(void *cga) {
        CGA *cgaPtr = static_cast<CGA *>(cga);
        cgaPtr->renderLoop();
        return 0;
    }

    void PC::runLoop() {
        // keep going until the user quits
        while (!shouldQuit) {
            
            if (cpu.canInterrupt()) {
                byte interruptType = pic.getInterrupt();
                if (interruptType != NO_INTERRUPT) {
                    cpu.hardwareInterrupt(interruptType);
                }
            }
            
            cpu.step();
            pit.update(); // not quite right, but hopefully close enough
            
        }
        quit:
        cga.exitRender();
    }
    

    static int runLoopHelper(void *pc) {
        PC *pcPtr = static_cast<PC *>(pc);
        pcPtr->runLoop();
        return 0;
    }

    void PC::run() {
        // start run loop on a separate thread
        SDL_Thread *runLoopThread;

        runLoopThread = SDL_CreateThread(runLoopHelper, "PCRunLoopThread", (void *)this);

        if (NULL == runLoopThread) {
            printf("SDL_CreateThread failed: %s\n", SDL_GetError());
        }
        
        // SDL Rendering must be on the main thread
        cga.renderLoop();
        shouldQuit = true;
        // when we get out of here, we quit
        
        
        int threadReturnValue;
        SDL_WaitThread(runLoopThread, &threadReturnValue);
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
            
            case 0x08:
                dma.writeCommand((byte) value);
                break;
            case 0x0A:
                dma.singleChannelMask((byte) value);
                break;
            case 0x0B:
                dma.setMode((byte) value);
                break;
            case 0x0D:
                dma.masterReset();
                break;
            case 0x0F:
                dma.multiChannelMask((byte) value);
                break;
            case 0x20:
                pic.writeCommand(value);
                break;
            case 0x21:
                pic.writeData(value);
                break;
            case 0x40:
                pit.writeCounter(0, value);
                break;
            case 0x41:
                pit.writeCounter(1, value);
                break;
            case 0x42:
                pit.writeCounter(2, value);
                break;
            case 0x43:
                pit.writeControl((byte)value);
                break;
            case 0x61:
                ppi.setB(value);
                break;
            case 0x63:
                ppi.setControl(value);
                break;
            case 0x80:
                dma.setPage(0, value);
                break;
            case 0x81:
                dma.setPage(1, value);
                break;
            case 0x82:
                dma.setPage(2, value);
                break;
            case 0x83:
                dma.setPage(3, value);
                break;
            case 0xA0:
                cout << "Port A0 on 5150 disables NMI interrupts " << endl;
                break;
            case 0xC0:
                cout << "Ignoring port C0 TI SN746496 PC JR" << endl;
                break;
            case 0x213:
                // enable expansion unit (1) or disable (0)
                // ignored
                break;
            case 0x278:
                cout << "Ignoring port 278 Parallel Port Data" << endl;
                break;
            case 0x378:
                cout << "Ignoring port 378 Parallel Port Data" << endl;
                break;
            case 0x3B4:
                cout << "Ignoring port 3B4 MDA Controller" << endl;
                break;
            case 0x3B5:
                cout << "Ignoring port 3B5 MDA Controller" << endl;
                break;
            case 0x3B8:
                cout << "Ignoring port 3B8 MDA Controller" << endl;
                break;
            case 0x3B9:
                cout << "Ignoring port 3B9 MDA Controller" << endl;
                break;
            case 0x3BC:
                cout << "Ignoring port 3BC Parallel Port Data" << endl;
                break;
            case 0x3D4:
                cga.set6845RegisterIndex(value);
                break;
            case 0x3D5:
                cga.set6845RegisterValue(value);
                break;
            case 0x3D8:
                cga.setMode(value);
                break;
            case 0x3D9:
                cga.setColor(value);
                break;
            case 0x2FB:
                cout << "Ignoring port 2FB Serial Port Line Control Register" << endl;
                break;
            case 0x3F2:
                fdc.writeControl(value);
                break;
            case 0x3F5:
                fdc.writeCommand(value);
                break;
            case 0x3FB:
                cout << "Ignoring port 3FB Serial Port Line Control Register" << endl;
                break;
            default:
                cout << "Port 0x" << hex << uppercase << port << dec << " not implemented for writing!" << endl;
                break;
        }
    }

    word PC::readPort(word port) {
        switch (port) {
            case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
            {
                byte channel = (byte) port / 2;
                if (port % 2 == 0) {
                    return dma.readAddress(channel);
                } else {
                    return dma.readCounter(channel);
                }
                break;
            }
            case 0x20:
                return pic.readStatus();
                break;
            case 0x21:
                return pic.readData();
                break;
            case 0x41:
                return pit.readCounter(1);
                break;
            case 0x60:
                return ppi.readA();
                break;
            case 0x61:
                return ppi.readB();
                break;
            case 0x62:
                return ppi.readC();
                break;
            case 0x278:
                cout << "Ignoring reading port 278 Parallel Port Data" << endl;
                return 0;
                break;
            case 0x378:
                cout << "Ignoring reading port 378 Parallel Port Data" << endl;
                return 0;
                break;
            case 0x3BC:
                cout << "Ignoring reading port 3BC Parallel Port Data" << endl;
                return 0;
                break;
            case 0x3DA:
                return cga.getStatus();
                break;
            case 0x201:
                cout << "Reading game port 201, trying to say not there" << endl;
                return 0xFF; // hack for bios to not thingk game port is there
                break;
            case 0x2C1:
            case 0x241:
            case 0x341:
                cout << "Reading clock port, trying to say not there" << endl;
                return 0xFF;
                break;
            case 0x2FB:
                cout << "Ignoring reading port 2FB Serial Port Line Control Register" << endl;
                return 0;
                break;
            case 0x3F4: // FDC read status (MSR)
                return fdc.readStatus();
                break;
            case 0x3F5: // FDC read command (FIFO)
                return fdc.readCommand();
                break;
            case 0x3FB:
                cout << "Ignoring reading port 3FB Serial Port Line Control Register" << endl;
                return 0;
                break;
            default:
                cout << "Port 0x" << hex << uppercase << port << dec << " not implemented for reading!" << endl;
                return 0;
        }
    }
}
