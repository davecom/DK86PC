//
//  CGA.cpp
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
//  MERCHANTABILITY or FITPCS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

// implement the Color Graphics Adapter

#include "CGA.hpp"


namespace DK86PC {

#define PC_WIDTH 640
#define PC_HEIGHT 200
#define CGA_BASE_MEMORY_LOCATION 0xB8000

void CGA::initScreen() {
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return;
    }
    window = SDL_CreateWindow("DK86PC", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, PC_WIDTH, PC_HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window: %s", SDL_GetError());
        return;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create renderer: %s", SDL_GetError());
        return;
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, PC_WIDTH, PC_HEIGHT);
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 0x00, 255, 0x00, 0x00);
    SDL_RenderClear(renderer);
}

void CGA::renderScren() {
    // clear renderer
    // used to have this in but took it out and it seems to not
    // have any negative effect
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black
    SDL_RenderClear(renderer);
    if (graphicsMode) {
        return;
    } else { // text mode
        for (int row = 0; row < 25; row++) {
            for (int column = 0; column < numColumns; column++) {
                address memLocation = CGA_BASE_MEMORY_LOCATION + (row * (numColumns * 2)) + column * 2;
                byte character = memory.readByte(memLocation);
                byte attribute = memory.readByte(memLocation + 1);
                drawCharacter(row, column, character, attribute);
            }
        }
    }
}

byte CGA::getStatus() {
    return status;
}

void CGA::setMode(byte value) {
    if (value & 1) {
        numColumns = 80;
    } else {
        numColumns = 40;
    }
    graphicsMode = (value & 2);
    greyscaleMode = (value & 4);
    highResolutionMode = (value & 16);
}

void CGA::verticalRetraceStart() {
    status = status | 8;
}

void CGA::verticalRetraceEnd() {
    status = status & 0b11110111;
}

inline void CGA::drawCharacter(byte row, byte column, byte character, byte attribute) {
    
}

}
