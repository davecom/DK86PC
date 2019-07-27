//
//  Types.h
//  DK86PC
//
//  Created by David Kopec on 7/23/19.
//  Copyright © 2019 David Kopec. All rights reserved.
//

#include <cstdint>
#include <iostream>

#ifndef Types_h
#define Types_h

using namespace std;

namespace DK86PC {
    
#define lowNibble(b) ((b) & 0x0F)
#define highNibble(b) (((b) & 0xF0) >> 4)
    
    
    
    typedef uint8_t byte;
    typedef uint16_t word;
    typedef uint32_t address;
    
    inline word signExtend(byte original) {
        if (original & 0x80) {
            return ((word) original | 0xFF00);
        }
        return (word) original;
    }
}

#endif /* Types_h */
