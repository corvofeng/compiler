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


void DState::findSimple(State *start)
{
    if (start == NULL) {
        return ;
    }

    if (this->allState.find(start) == this->allState.end()) {
        this->allState.insert(start);
    }

    if (start->c == Match) {
        this->isEnd = true;
        this->endFunc = start->endFunc; // 向该状态传递结束函数
    } else if (start->c == Split) {
        findSimple(start->out);
        findSimple(start->out1);
    } else if (start->c < 256) {
        findSimple(start->out1);
    } else {
        printf("error in dfa\n");
        exit(1);
    }
}

bool DState::isSimilar(DState *newDs)
{
    if (this->coreState.size() != newDs->coreState.size()) {
        return false;
    }

    for (auto pDs = coreState.begin(); pDs != coreState.end(); pDs++) {
        if(newDs->coreState.find(*pDs) == newDs->coreState.end())
            return false;
    }

    for (auto pDs = newDs->coreState.begin(); pDs != newDs->coreState.end(); ++pDs) {
        if(this->coreState.find(*pDs) == this->coreState.end())
            return false;
    }

    return true;
}
