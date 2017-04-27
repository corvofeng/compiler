/*
 *=============================================================================
 *    Filename:state.h
 *
 *     Version: 1.0
 *  Created on: April 27, 2017
 *
 *      Author: corvo
 *=============================================================================
 */

#ifndef STATE_H_
#define STATE_H_

enum {
    Match = 256,
    Split = 257
};



class State
{
    int c;
    State *out;
    State *out1;
    int lastlist;

public:
    State() {}
    State(int c, State *out, State *out1);
};

#endif
