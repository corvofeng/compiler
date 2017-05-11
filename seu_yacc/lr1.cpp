/*
 *=============================================================================
 *    Filename:lr1.cpp
 *
 *     Version: 1.0
 *  Created on: May 04, 2017
 *
 *      Author: corvo
 *=============================================================================
 */


#include "lr1.h"
#include "lrstate.h"

void LR1::iterms() {
    // 构建初始状态
    LRState *start = new LRState();
    char term = ('$');
    string left = "#";
    string right = this->grammar->getNonTermHead();
    start->addCoreExpr(left, right, term);

    start->findAllExpr();
    lrStateVec.push_back(start);
    state2id.insert(std::make_pair(start, id));
    id ++;

  //  start->printAllExpr();

    // 遍历所有状态, 直到无法添加新的状态
    for (int i = 0; i < lrStateVec.size(); ++i) {
        getAllNextState(lrStateVec.at(i));
    }

}

bool LR1::getAllNextState(LRState *start) {

    bool isAdd = false;

    for(char next : start->maybeNext) {
        LRState* nLRState  = new LRState();
        for (SingleExpress *sExpr : start->singleExprVec) {

            int pos = sExpr->pos;

            // 需要考虑right的长度是否比pos短, 如果短, 则继续匹配下一项, 否则进行匹配
            if (sExpr->right.size() <= pos) {
                continue;
            }

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
            nLRState->printAllExpr();
            */

            nLRState->findAllExpr();


            /*
            cout << "---- after find all -------" << endl;
            nLRState->printAllExpr();
            cout << "---------------------------" << endl << endl;
            */


            state2id.insert(std::make_pair(nLRState, id));
            id ++;

        } else {
            delete nLRState;   // 否则加入该边
        }
        start->out.insert(std::make_pair(next, lrState));
    }

    return isAdd;
}
