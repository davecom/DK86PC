//
//  PPI.cpp
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

// implement the intel 8255

#include "PPI.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

namespace DK86PC {

byte usbToPCScancode[100] = {0xFF, 0xFF, 0xFF, 0xFF, 0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24, 0x25, 0x26, 0x32, 0x31, 0x18, 0x19, 0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, 0x2d, 0x15, 0x2c, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0a, 0x0b, 0x1c, 0x1, 0x0e, 0x0f, 0x39, 0x0c, 0x0d, 0x1a, 0x1b, 0x2b, 0x0, 0x27, 0x28, 0x29, 0x33, 0x34, 0x35, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x57, 0x58, 0xFF, 0x46, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x45, 0xFF, 0x37, 0x4a, 0x4e, 0xFF, 0x4f, 0x50, 0x51, 0x4b, 0x4c, 0x4d, 0x47, 0x48, 0x49, 0x52, 0x53};

void PPI::setB(byte value) {
    b = value;
}

byte PPI::readA() {
    return a;
}

byte PPI::readB() {
    return b;
}

byte PPI::readC() {
    return c;
}

void PPI::setControl(byte value) {
    control = value;
}

void PPI::keyboardDown(SDL_Keysym s) {
    // SDL uses usb codes for scancodes
    SDL_Scancode usb_code = s.scancode;
    // convert it
    byte scancode = usbToPCScancode[usb_code];
    cout << "KEY PRESSED " << dec << int(scancode) << endl;
    if (scancode == 0xFF) { // this key didn't exist in IBM PC scancode days
        return;
    }
    a = scancode; // set it
    pic.requestInterrupt(1); // interrupt 9 when key happened
}

void PPI::keyboardUp(SDL_Keysym s) {
    // SDL uses usb codes for scancodes
    SDL_Scancode usb_code = s.scancode;
    // convert it
    byte scancode = usbToPCScancode[usb_code];
    if (scancode == 0xFF) { // this key didn't exist in IBM PC scancode days
        return;
    }
    a = (scancode | 0x80); // set it with bit 7 for key up
    pic.requestInterrupt(1); // interrupt 9 when key happened
}

}
