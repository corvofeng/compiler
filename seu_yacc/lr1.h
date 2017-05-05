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

       // this->grammar->printFirst();

        //lrStateStandard =
        LRState::getStandardState(this->grammar);
    }


    void iterms() {
        LRState *start = new LRState();
        char term = ('$');
        string left = "#";
        string right = this->grammar->getNonTermHead();
        start->addCoreExpr(left, right, term);
        start->findAllExpr();
        lrStateVec.push_back(start);

       //start->printAllExpr();

        for (int i = 0; i < lrStateVec.size(); ++i) {
            getAllNextState(lrStateVec.at(i));
        }

    }

    LRState* alreadyHave(LRState *lsTest) {
        for(LRState * ls: this->lrStateVec) {
            if (lsTest->isEqual(ls)) {
                return ls;
            }
        }
        return NULL;
    }


    bool getAllNextState(LRState *start) {

        bool isAdd = false;
        for(char next : start->maybeNext) {
            LRState* nLRState  = new LRState();
            for (SingleExpress *sExpr : start->singleExprVec) {

                int pos = sExpr->pos;
                if (sExpr->right.at(pos) == next) {
                    pos += 1;
                    nLRState->addCoreExpr(sExpr->left, sExpr->right, pos, sExpr->term);
                }
            }
            LRState* lrState = this->alreadyHave(nLRState);
            if (lrState == NULL) { // 如果之前不存在这样的状态, 直接进行加入
                lrState = nLRState;
                isAdd = true;
                this->lrStateVec.push_back(lrState);

                /*
                cout << "From " << next << "->" << endl;
                cout << "---- before find all ------" << endl;
                */
                nLRState->findAllExpr();
                /*
                cout << "---- after find all -------" << endl;
                nLRState->printAllExpr();
                cout << "---------------------------" << endl << endl;
                */

            } else {
                delete nLRState;   // 否则加入该边
            }
            start->out.insert(std::make_pair(next, lrState));
        }

        return isAdd;
    }

    std::set<LRState*> haveTravel;
    std::map<LRState*, int> state2id;

    void printLR1() {
        haveTravel.clear();
        LRState* start = this->lrStateVec.at(0);
        this->showLR1(start);
    }

    void showLR1(LRState* start) {
        static int id = 0;
        if (haveTravel.find(start) != haveTravel.end()) {
            return;
        }
        haveTravel.insert(start);
        if (state2id.find(start) == state2id.end()) {
            state2id.insert(std::make_pair(start, id));
            id += 1;
        }

        std::map<char, LRState*>& lrVec = start->out;
        for(auto it: lrVec) {
            char ch = it.first;
            LRState *tmpLR = it.second;

            if (state2id.find(tmpLR) == state2id.end()) {
                state2id.insert(std::make_pair(tmpLR, id));
                id += 1;
            }

            cout << state2id.at(start) << "<- " << ch << " ->" << state2id.at(tmpLR) << endl;;
            this->showLR1(tmpLR);
        }
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
