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

    }

    DState* nfa2dfa(Re2NFA *nfa){

        State *start = nfa->nfa_s;
        printf("start pointer is %p\n", nfa->nfa_s);
        std::set<int> char_list = nfa->char_set;

        printf("\n");

        if (start == NULL) {
            return NULL;
        }
        std::list<DState*> dsList;

        DState *ds = new DState();
        ds->addCoreState(start);
        ds->getAllState();

        dsList.push_back(ds);

        bool isModify = true;

        DState *pDs;

        while((pDs = pickupFromList(&dsList)) != NULL) {        // 从链表中选择一个尚未遍历的状态

            pDs->hasTravel = true;

            for(auto pChar = char_list.begin(); pChar != char_list.end(); pChar++) {

                DState *tmpDs = new DState();

                for(auto stateIterator = pDs->allState.begin();
                    stateIterator != pDs->allState.end();
                    stateIterator ++) {

                    State* s = (*stateIterator);
                    if (s->c == *pChar) {
                        printf("The char is %c\n", s->c);
                        tmpDs->addCoreState(s->out);
                    }
                }

                if (tmpDs->coreState.size() == 0) {
                    printf("can't reach by %c\n\n\n", *pChar);
                    delete tmpDs;
                    continue;
                }

                // 比较现有的状态与先前状态是否重复, 如果重复, 那么采用另一种方式加入

                DState *oldDs = alreadyHave(&dsList, tmpDs);

                if (oldDs != NULL) {        // 说明已经存在该状态
                    delete tmpDs;
                    printf("already has this state\n");
                    pDs->addDState(oldDs, *pChar);
                } else {
                    tmpDs->getAllState();
                    pDs->addDState(tmpDs, *pChar);
                    dsList.push_back(tmpDs);
                }

                printf("\n\n");
            }
        }

        dstart = ds;
        return dstart;
    }

    void showNFA(DState *dStart) {

    }
};

DState* pickupFromList(std::list<DState*> *pDsList) {

    for(auto pList = pDsList->begin(); pList != pDsList->end(); pList++) {
        DState* tmpDs = (*pList);
        if (tmpDs->hasTravel == false) {
            return tmpDs;
        }
    }

    return NULL;
}


DState* alreadyHave(std::list<DState *> *pDsList, DState* ds) {

    for(auto pList = pDsList->begin(); pList != pDsList->end(); pList++) {
        DState* tmpDs = (*pList);
        if (ds->isSimilar(tmpDs)) {
            return tmpDs;
        }
    }
    return NULL;
}


#endif
