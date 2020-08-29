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
#include <SDL.h>
#include <vector>
#include "SDL_ttf.h"

namespace DK86PC {

#define NUM_CHARACTERS 256
#define NUM_COLORS 16

class CGA {
public:
    CGA(Memory &mem) : memory(mem) {
        initScreen();
    };
    ~CGA() {
        freeFontCache();
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_DestroyTexture(texture);
        SDL_Quit();
    };
    
    void initScreen();
    void renderLoop();
    void renderScreen();
    byte getStatus();
    void setMode(byte value);
    void setColor(byte value);
    void verticalRetraceStart();
    void verticalRetraceEnd();
    void horizontalRetraceStart();
    void horizontalRetraceEnd();
    void exitRender();
private:
    inline void drawCharacter(byte row, byte column, byte character, byte attribute);
    void createFontCache();
    void freeFontCache();
    Memory &memory;
    byte status;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    TTF_Font* font;
    byte numColumns = 40;
    bool graphicsMode = false;
    bool greyscaleMode = false;
    bool highResolutionMode = false;
    byte backgroundColor = 0;
    bool intensePalette = false;
    bool alternatePalette = false;
    int cellWidth = 8;  // text cell width
    int cellHeight = 8; // text cell height
    bool shouldExit = false;
    SDL_Texture *fontCache[NUM_COLORS][NUM_CHARACTERS];
};

}

#endif /* CGA_hpp */
