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
#include "Types.h"

namespace DK86PC {

#define PC_WIDTH 640
#define PC_HEIGHT 200
#define NUM_ROWS 25
#define CGA_BASE_MEMORY_LOCATION 0xB8000

SDL_Color colorPalette[NUM_COLORS] = {
    {0x00, 0x00, 0x00, 0xFF}, // black
    {0x00, 0x00, 0xAA, 0xFF}, // blue
    {0x00, 0xAA, 0x00, 0xFF}, // green
    {0x00, 0xAA, 0xAA, 0xFF}, // cyan
    {0xAA, 0x00, 0x00, 0xFF}, // red
    {0xAA, 0x00, 0xAA, 0xFF}, // magenta
    {0xAA, 0x55, 0x00, 0xFF}, // brown
    {0xAA, 0xAA, 0xAA, 0xFF}, // light gray
    {0x55, 0x55, 0x55, 0xFF}, // dark gray
    {0x55, 0x55, 0xFF, 0xFF}, // light blue
    {0x55, 0xFF, 0xFF, 0xFF}, // light green
    {0x55, 0xFF, 0xFF, 0xFF}, // light cyan
    {0xFF, 0x55, 0x55, 0xFF}, // light red
    {0xFF, 0x55, 0xFF, 0xFF}, // light magenta
    {0xFF, 0xFF, 0x55, 0xFF}, // yellow
    {0xFF, 0xFF, 0xFF, 0xFF}, // white
};

void CGA::createFontCache() {
    for (int color = 0; color < NUM_COLORS; color++) {
        for (int character = 1; character < NUM_CHARACTERS; character++) {
            char tcharacter = (char)character;
            SDL_Surface *text_surface;
            char show[2] = {tcharacter, '\0'};
            if(!(text_surface = TTF_RenderText_Solid(font, show, colorPalette[color]))) {
            //if(!(text_surface = TTF_RenderGlyph_Solid(font, tcharacter, colorPalette[color]))) {
                //handle error here, perhaps print TTF_GetError at least
                printf("Could not RenderText_Solid for color %d, character %d\n", color, character);
            } else {
                SDL_Texture* inbetween = SDL_CreateTextureFromSurface(renderer, text_surface);
                //perhaps we can reuse it, but I assume not for simplicity.
                fontCache[color][character] = inbetween;
                SDL_FreeSurface(text_surface);
            }
            
        }
        // Character 0 ignored
    }
}

void CGA::freeFontCache() {
    for (int color = 0; color < NUM_COLORS; color++) {
        // character 0 always ignored, never allocated
        for (int character = 1; character < NUM_CHARACTERS; character++) {
            SDL_DestroyTexture(fontCache[color][character]);
        }
    }
}

void CGA::initScreen() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
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
//    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, PC_WIDTH, PC_HEIGHT);
//    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 0x00, 255, 0x00, 0x00);
    SDL_RenderClear(renderer);
    // load font
    if (TTF_Init() != 0){
        printf("Couldn't run TTF_Init() successfully.");
        SDL_Quit();
    }
    font = TTF_OpenFont("Fonts/Px437_IBM_BIOS.ttf", PC_HEIGHT / NUM_ROWS);
    if (font == NULL) {
        printf("Could not load font file.");
        SDL_Quit();
    }
    
    createFontCache();
}

#define MILLI_PER_FRAME 16

void CGA::renderLoop() {
    static long long numFrames = 0;
    static uint32_t lastTicks = 0;
    
    
                
    while (!shouldExit) {
        uint32_t nextTicks = SDL_GetTicks();
        uint32_t difference = (nextTicks - lastTicks);
        cout << dec << difference << endl;
        if (difference > MILLI_PER_FRAME) { // roughly 60 fps
            lastTicks = nextTicks;
            renderScreen(nextTicks);
            
            
            
            numFrames++;
            
            // pit is supposed to be 18.2 hz, doing ~20 hz here
    //                if (numFrames % 3 == 0) {
    //                    pit.update();
    //                }
            
            if (numFrames %4 == 0) {
                verticalRetraceStart();
            } else if (numFrames %4 == 1) {
                verticalRetraceEnd();
            } else if (numFrames %4 == 2) {
                horizontalRetraceStart();
            } else if (numFrames %4 == 3) {
                horizontalRetraceEnd();
            }
        } else {
            SDL_Delay(MILLI_PER_FRAME - difference);
        }
    }
}

void CGA::renderScreen(uint32_t timing) {
    // clear renderer
    // used to have this in but took it out and it seems to not
    // have any negative effect
    SDL_Color bgColor;
    if (graphicsMode) {
        bgColor = colorPalette[backgroundColor];
    } else {
        bgColor = colorPalette[0];
    }
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, 255);
    SDL_RenderClear(renderer);
    if (graphicsMode) {
        return;
    } else { // text mode
        int cursorLocation = ((int)registers6845[0xE] << 8) | ((int)registers6845[0xF]);
        byte cursorRow = cursorLocation / numColumns;
        byte cursorColumn = cursorLocation - (cursorRow * numColumns);
        cellWidth = PC_WIDTH / numColumns;
        cellHeight = PC_HEIGHT / NUM_ROWS;
        for (int row = 0; row < NUM_ROWS; row++) {
            for (int column = 0; column < numColumns; column++) {
                address memLocation = CGA_BASE_MEMORY_LOCATION + (row * (numColumns * 2)) + column * 2;
                byte character = memory.readByte(memLocation);
                // cout << character;
                byte attribute = memory.readByte(memLocation + 1);
                // if text mode, draw cursor every half second
                if (cursorColumn == column && cursorRow == row && ((timing % 1000) > 500)) {
                    drawCharacter(row, column, 95, (attribute&0xF0) | 15);
                } else {
                    drawCharacter(row, column, character, attribute);
                }
            }
        }
    }
    SDL_RenderPresent(renderer);
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

void CGA::setColor(byte value) {
    backgroundColor = (value & 0x0F);
    intensePalette = (value & 16);
    alternatePalette = (value & 32);
}

void CGA::verticalRetraceStart() {
    status = status | 8;
}

void CGA::verticalRetraceEnd() {
    status = status & 0b11110111;
}

void CGA::horizontalRetraceStart() {
    status = status | 1;
}

void CGA::horizontalRetraceEnd() {
    status = status & 0b11111110;
}

inline void CGA::drawCharacter(byte row, byte column, byte character, byte attribute) {
    // Can't draw non-characters
    if (character == 0) {
        return;
    }
    // some monitors/bios treat color and black and white modes both as color, so we'll try that here
    SDL_Color bgColor = colorPalette[highNibble(attribute)];
    int fgColor = lowNibble(attribute);
    SDL_Rect rect;
    rect.x = column * cellWidth;
    rect.y = row * cellHeight;
    rect.w = cellWidth;
    rect.h = cellHeight;
    // draw background of text cell
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(renderer, &rect);
    // draw text in foreground
    
    char text = ((char)character);
//    text = ((char)row * numColumns + column) + 1;
//    if ((int)text > 100) {
//        text = 100;
//    }
//    if (text != 0) {
//        cout << byte(text) << endl;
//    }
//    if (attribute != 0) {
//        cout << hex << uppercase << lowNibble(attribute) << dec << endl;
//    }
    //text = 'A';
    // char show[2] = {text, '\0'};
//    if(!(text_surface = TTF_RenderGlyph_Solid(font, text, fgColor))) {
//        //handle error here, perhaps print TTF_GetError at least
//        printf("Could not RenderText_Solid");
//    } else {
//        SDL_Texture* inbetween = SDL_CreateTextureFromSurface(renderer, text_surface);
//        //perhaps we can reuse it, but I assume not for simplicity.
//        SDL_RenderCopy(renderer, inbetween, NULL, &rect);
//        SDL_FreeSurface(text_surface);
//        SDL_DestroyTexture(inbetween);
//    }
    SDL_Texture* inbetween = fontCache[fgColor][text];
    SDL_RenderCopy(renderer, inbetween, NULL, &rect);
}

void CGA::exitRender() {
   shouldExit = true;
}

void CGA::set6845RegisterIndex(byte index) {
    registerIndex6845 = index;
}

void CGA::set6845RegisterValue(byte value) {
    registers6845[registerIndex6845] = value;
}

}
