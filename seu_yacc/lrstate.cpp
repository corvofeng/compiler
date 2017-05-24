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


LRState* LRState::getStandardState(Grammar *grammer) {
    if (lrStateStandard) {
        return lrStateStandard;
    }
    LRState *lrState = new LRState();

    LRState::first = grammer->first;
    for (auto it: grammer->pExprVec) {
        Expression *expr = it;
        string left = expr->left;
        for (auto right: expr->right) {
            lrState->addCoreExpr(left, right, 0, '?', expr->func);
        }
    }
    nonTermHead = grammer->nonTermHead;
    lrStateStandard = lrState;
    /*
    cout << "-------------------------" << endl;
    lrStateStandard->printAllExpr();
    cout << "-------------------------" << endl;
    */
    return lrState;
}

void LRState::findAllExpr() {
    if (!lrStateStandard) {
        cout << "Please create standard list!!!" << endl;
        exit(1);
    }
    //cout << "find all expr" << endl;

    auto it = this->singleExprVec.begin();

    int i = 0;
    while (i < singleExprVec.size()) {
        SingleExpress* sExpr = singleExprVec[i];
        int pos = sExpr->pos;
        string &right = sExpr->right;
        char ch = sExpr->right[pos];
        char term = sExpr->term;

        if (pos >= right.size()) { // 如果当前位置已经处于末尾, 则不进行处理
            i++;

            if (right.size() == 1 &&  right.at(0) == nonTermHead.at(0)) { // 当前状态为可接受
                //cout << right.at(pos - 1) << endl;
                this->acc = -1;
            } else {        // 不可接受状态, 但已经到达末尾
                //this->termAll += term;
                /*
                cout << "************" << endl;
                cout << this->termAll<< endl;
                cout << "************" << endl;
                */
                this->acc = this->findExprByLeftRight(sExpr->left, right); // 找寻该表达式在标准表达式中的位置
     //           cout << "the acc is " << this->acc << endl;
            }
            continue;
        }

        string Beta = right.substr(pos);
        Beta += term;
        //cout << "The Beta is " << Beta << "  pos is " << pos << endl;
        increaseState(Beta);
        i++;
    }
}


void LRState::increaseState(string &s) {

    //cout << "The s is " << s << endl;
    if (s.size() < 2) {
        return;
    }

    if(s.size() == 2) {
        string left = s.substr(0, 1);
        char term = s[1];
        findAndPush(left, term);
        return;
    }

    string left = s.substr(0, 1);
    string mayBeLeft = s.substr(1);
    char term = *s.end();

    for(auto it: mayBeLeft) {
        char ch = it;
        if (isupper(ch)) { // 非终结符
            string s;
            s = ch;
            for (auto term : first.at(s)) {
                findAndPush(left, term);
            }
        } else {        // ch为终结符
            //cout << "The char is " << ch << endl;
            findAndPush(left, ch);
        }
        break;
    }
}

bool LRState::isEqual(LRState* tmpLRState) {
    vector<SingleExpress*>& coreExprVecTmp = tmpLRState->coreExpr;
    if (this->coreExpr.size() != coreExprVecTmp.size()) {
        return false;
    }
    for (auto it : coreExprVecTmp){
        SingleExpress *tmpExpr = it;
        bool exist = false;
        for(SingleExpress *oriExpr : this->coreExpr) {
            if (oriExpr->isEqual(tmpExpr)) {
                exist = true;
            }
        }

        if (!exist) {
            return false;
        }
    }

    for (SingleExpress *oriExpr : this->coreExpr) {

        bool exist = false;
        for (SingleExpress* tmpExpr : coreExprVecTmp) {
            if (tmpExpr->isEqual(oriExpr)) {
                exist = true;
            }
        }

        if (!exist) {
            return false;
        }
    }

    return true;
}



