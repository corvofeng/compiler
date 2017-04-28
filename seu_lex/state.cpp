/*
 *=============================================================================
 *    Filename:state.cpp
 *
 *     Version: 1.0
 *  Created on: April 27, 2017
 *
 *      Author: corvo
 *=============================================================================
 */

#include "state.h"

State::State(int c, State *out, State *out1) {
    this->c = c;
    this->out = out;
    this->out1 = out1;
}
