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

Uint16 unicodeMappings[256] = {0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f, 0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x007f, 0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x00e0, 0x00e5, 0x00e7, 0x00ea, 0x00eb, 0x00e8, 0x00ef, 0x00ee, 0x00ec, 0x00c4, 0x00c5, 0x00c9, 0x00e6, 0x00c6, 0x00f4, 0x00f6, 0x00f2, 0x00fb, 0x00f9, 0x00ff, 0x00d6, 0x00dc, 0x00a2, 0x00a3, 0x00a5, 0x20a7, 0x0192, 0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x00f1, 0x00d1, 0x00aa, 0x00ba, 0x00bf, 0x2310, 0x00ac, 0x00bd, 0x00bc, 0x00a1, 0x00ab, 0x00bb, 0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556, 0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510, 0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f, 0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567, 0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b, 0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580, 0x03b1, 0x00df, 0x0393, 0x03c0, 0x03a3, 0x03c3, 0x00b5, 0x03c4, 0x03a6, 0x0398, 0x03a9, 0x03b4, 0x221e, 0x03c6, 0x03b5, 0x2229, 0x2261, 0x00b1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00f7, 0x2248, 0x00b0, 0x2219, 0x00b7, 0x221a, 0x207f, 0x00b2, 0x25a0, 0x00a0};

void CGA::createFontCache() {
    for (int color = 0; color < NUM_COLORS; color++) {
        for (int character = 1; character < NUM_CHARACTERS; character++) {
            Uint16 tcharacter = unicodeMappings[character];
            SDL_Surface *text_surface;
            if(!(text_surface = TTF_RenderGlyph_Solid(font, tcharacter, colorPalette[color]))) {
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
    // default
    pcWidth = 640;
    pcHeight = 200;
    window = SDL_CreateWindow("DK86PC", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, pcWidth, pcHeight, SDL_WINDOW_ALLOW_HIGHDPI);
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
    font = TTF_OpenFont("Fonts/Px437_IBM_BIOS.ttf", pcHeight / NUM_ROWS);
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
        // output frame timing if needed
        //cout << dec << difference << endl;
        if (difference > MILLI_PER_FRAME) { // roughly 60 fps
            lastTicks = nextTicks;
            verticalRetraceEnd();
            renderScreen(nextTicks);
            verticalRetraceStart();
            
            
            numFrames++;
            
            // pit is supposed to be 18.2 hz, doing ~20 hz here
    //                if (numFrames % 3 == 0) {
    //                    pit.update();
    //                }
            
//            if (numFrames %4 == 0) {
//                verticalRetraceStart();
//            } else if (numFrames %4 == 1) {
//                verticalRetraceEnd();
//            } else if (numFrames %4 == 2) {
//                horizontalRetraceStart();
//            } else if (numFrames %4 == 3) {
//                horizontalRetraceEnd();
//            }
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
        cellWidth = pcWidth / numColumns;
        cellHeight = pcHeight / NUM_ROWS;
        for (int row = 0; row < NUM_ROWS; row++) {
            horizontalRetraceStart();
            for (int column = 0; column < numColumns; column++) {
                address memLocation = CGA_BASE_MEMORY_LOCATION + (row * (numColumns * 2)) + column * 2;
                byte character = memory.readByte(memLocation);
                // cout << character;
                byte attribute = memory.readByte(memLocation + 1);
                // if text mode, draw cursor every half second
                if (cursorColumn == column && cursorRow == row && ((timing % 1000) > 500)) {
                    drawCharacter(row, column, 219, (attribute&0xF0) | 15);
                } else {
                    drawCharacter(row, column, character, attribute);
                }
            }
            horizontalRetraceEnd();
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
    
    if (!graphicsMode && numColumns == 40) {
        pcWidth = 320;
    } else if (!graphicsMode && numColumns == 80) {
        pcWidth = 640;
    }
    SDL_SetWindowSize(window, pcWidth, pcHeight);
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
