//
//  CGA.hpp
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

// implement the Color Graphics Adapter

#ifndef CGA_hpp
#define CGA_hpp

#include <stdio.h>
#include "Memory.hpp"
// Apple OSX and iOS (Darwin)
#include<SDL.h>
#include<vector>

namespace DK86PC {

class CGA {
public:
    CGA(Memory &mem) : memory(mem) {
        initScreen();
    };
    ~CGA() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_DestroyTexture(texture);
        SDL_Quit();
    };
    
    void initScreen();
    void renderScren();
    byte getStatus();
    void verticalRetraceStart();
    void verticalRetraceEnd();
private:
    Memory &memory;
    byte mode;
    byte status;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
};

}

#endif /* CGA_hpp */
