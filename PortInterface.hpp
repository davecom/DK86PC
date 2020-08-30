//
//  PortInterface.hpp
//  DK86PC
//
//  Created by David Kopec on 8/29/20.
//  Copyright © 2020 David Kopec. All rights reserved.
//

#ifndef PortInterface_h
#define PortInterface_h

#include "Types.h"

namespace DK86PC {

class PortInterface {
    public:
    virtual void writePort(word port, word value) = 0;
    virtual word readPort(word port) = 0;
};

}

#endif /* PortInterface_h */
