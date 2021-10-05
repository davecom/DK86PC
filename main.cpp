//
//  main.cpp
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
#define SDL_MAIN_HANDLED

#include <iostream>
#include "PC.hpp"

#ifdef _WIN32
#include <filesystem>
#include <SDL.h>
#endif

using namespace std;
using namespace DK86PC;

int main(int argc, const char * argv[]) {
    // insert code here...
    cout << "Hello, World!" << endl;

    // change to current directory for working path
#ifdef _WIN32
    filesystem::current_path(SDL_GetBasePath()); //setting pat
    auto path = filesystem::current_path(); //getting path
    cout << path << endl;
#endif
    PC pc = PC();
    //pc.loadBIOS("BIOS/Original5150/BIOS_5150_24APR81_U33.BIN");
    //pc.loadBIOS("BIOS/5150_2764_DIAG.bin");
    pc.loadBIOS("BIOS/pcxtbios.bin");
    pc.loadCasetteBASIC("CasetteBASIC/5150cb10_1.bin", "CasetteBASIC/5150cb10_2.bin", "CasetteBASIC/5150cb10_3.bin", "CasetteBASIC/5150cb10_4.bin");
    pc.run();
    
    return 0;
}
