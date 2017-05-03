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
    NFA *nfa;
    int dsCnt = 0;
    std::map<DState*, int> dState2id;
    std::map<int, DState*> id2dState;

    N2DFA(NFA *nfa) {
        this->nfa = nfa;
    }

    ~N2DFA() {
        this->haveTravel.clear();
        free(this->dstart);
    }

    std::set<DState*> haveTravel;
    std::map<DState*, int> state2id;
    int id = 0;

    void nfa2dfa() {
        this->nfa2dfa(nfa);
    }

    void printDFA() {
        this->haveTravel.clear();
        showDFA(this->dstart);
    }

private:
    DState* nfa2dfa(NFA *nfa);
    void showDFA(DState *ds);
    void free(DState *ds);

};
#endif
