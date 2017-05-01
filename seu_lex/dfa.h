/*
 *=============================================================================
 *    Filename:dfa.h
 *
 *     Version: 1.0
 *  Created on: April 29, 2017
 *
 *      Author: corvo
 *=============================================================================
 */

#ifndef DFA_H_
#define DFA_H_

#include "state.h"
#include "nfa.h"

DState* pickupFromList(std::list<DState*> *pDsList);

DState* alreadyHave(std::list<DState *> *pDsList, DState* ds);


// NFA 转换为DFA
class N2DFA
{
public:
    DState *dstart;

    N2DFA() {}
    ~N2DFA() {
        this->haveTravel.clear();
        free(this->dstart);
    }

    void free(DState *ds);

    std::set<DState*> haveTravel;
    std::map<DState*, int> state2id;
    int id = 0;

    void showDFA(DState *ds);
    DState* nfa2dfa(Re2NFA *nfa);
};


#endif
