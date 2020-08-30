//
//  DummyPortInterface.hpp
//  CPUTests
//
//  Created by David Kopec on 8/29/20.
//  Copyright © 2020 David Kopec. All rights reserved.
//

#ifndef DummyPortInterface_h
#define DummyPortInterface_h

#include "PortInterface.hpp"

using namespace DK86PC;

class DummyPortInterface: public PortInterface {
    void writePort(word port, word value) override {
        return;
    };
    word readPort(word port) override {
        return 0;
    };
};


#endif /* DummyPortInterface_h */
