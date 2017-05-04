/*
 *=============================================================================
 *    Filename:LRState.h
 *
 *     Version: 1.0
 *  Created on: May 04, 2017
 *
 *      Author: corvo
 *=============================================================================
 */


#ifndef LRSTATE_H_
#define LRSTATE_H_

#include "expression.h"

class LRState {
public:
    static LRState* lrStateStandard;
    LRState(){}

    ~LRState() {
        for (auto single : this->singleExprVec){
            delete single;
        }
    }

    vector<SingleExpress*> singleExprVec;

    vector<SingleExpress*> coreExpr;        // 核心的expr

    /**
     * 获取标准的文法, 此处使用单例模式, 防止重复创建
     */
    static LRState* getStandardState(Grammar *grammer) {
        if (lrStateStandard) {
            return lrStateStandard;
        }
        LRState *lrState = new LRState();

        for (auto it: grammer->pExprVec) {
            Expression *expr = it;
            string left = expr->left;
            for (auto right: expr->right) {
                //SingleExpress* sExpr = new SingleExpress(left, right);
                lrState->addCoreExpr(left, right, {});
            }
        }
        lrStateStandard = lrState;
        return lrState;
    }

    static void deleteStandardState() {
        if (lrStateStandard) {
            delete lrStateStandard;
        }
        lrStateStandard = NULL;
    }

    void findAllExpr() {
        if (!lrStateStandard) {
            cout << "Please create standard sirst!!!" << endl;
            exit(1);
        }
        cout << "find all expr" << endl;

        for (auto it: this->coreExpr) {
            SingleExpress* sExpr = it;
            int pos = sExpr->pos;
            char ch = sExpr->right[pos];
            set<char> term = sExpr->term;

            if (std::isupper(ch)) {
                string s;
                s += ch;
                for (auto state: lrStateStandard->coreExpr) {
                    SingleExpress *standardState = state;
                    if(standardState->left == s) {
                        SingleExpress* newSExpr = new SingleExpress(s, standardState->right, term);
                        this->singleExprVec.push_back(newSExpr);
                    }
                }
            }
        }


        printAllExpr();
    }

    void printAllExpr() {
        for (auto it : this->singleExprVec) {
            SingleExpress* sExpr = it;
            sExpr->printSigleExpr();
        }
    }

    LRState* alreadyHave(LRState *lsTest) {

    }

    void addCoreExpr(string &left, string &right, set<char> terms) {
        SingleExpress* singleExpress = new SingleExpress(left, right, terms);
        this->coreExpr.push_back(singleExpress);
        this->singleExprVec.push_back(singleExpress);
    }

    void addExpr(Expression& expr) {
        string left = expr.left;
        set<string>& rht = expr.right;
        for (auto it: rht) {
            set<char> tmpC;
            tmpC.insert('$');
            SingleExpress* singleExpress = new SingleExpress(left, it, tmpC);
            this->singleExprVec.push_back(singleExpress);
        }
        return;
    }

};

LRState* LRState::lrStateStandard = NULL;
#endif
