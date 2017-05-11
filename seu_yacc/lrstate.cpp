/*
 *=============================================================================
 *    Filename:LRState.cpp
 *
 *     Version: 1.0
 *  Created on: May 04, 2017
 *
 *      Author: corvo
 *=============================================================================
 */

#include "lrstate.h"


LRState* LRState::lrStateStandard = NULL;
map<string, set<char>> LRState::first;
string LRState::nonTermHead;
