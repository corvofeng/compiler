/*
 *=============================================================================
 *    Filename:lr1.h
 *
 *     Version: 1.0
 *  Created on: May 04, 2017
 *
 *      Author: corvo
 *=============================================================================
 */


#ifndef LR1_H_
#define LR1_H_

#include "grammar.h"
#include "expression.h"
#include "lrstate.h"

using std::vector;

class LR1
{
public:
    Grammar *grammar = NULL;
    LRState* lrStateStandard = NULL;
    vector<LRState*> lrStateVec;

    LR1() {}
    LR1(string* expr, int size, string nonTermHead) {
        this->grammar = new Grammar(expr, size, nonTermHead);
        this->grammar->makeFirst();
        this->grammar->printFirst();

        lrStateStandard = LRState::getStandardState(this->grammar);
    }


    void iterms() {
        LRState *start = new LRState();
        char term = ('$');
        string left = "#";
        string right = this->grammar->getNonTermHead();
        start->addCoreExpr(left, right, term);
        start->findAllExpr();

        lrStateVec.push_back(start);
    }



    ~LR1() {
        for(auto it : lrStateVec) {
            delete it;
        }
        lrStateVec.clear();

        LRState::deleteStandardState();
        if (grammar) {
            delete grammar;
        }
    }



};


#endif
