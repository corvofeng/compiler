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
    Grammar *grammar = NULL;            // 该LR1解析中对应的文法
    //LRState* lrStateStandard = NULL;
    vector<LRState*> lrStateVec;        // 保存创建的所有状态
    int id = 0;

    LR1() {}
    LR1(string* expr, int size, string nonTermHead) {
        this->grammar = new Grammar(expr, size, nonTermHead);
        this->grammar->makeFirst();     // 产生first集合

       // this->grammar->printFirst();

        //lrStateStandard =
        LRState::getStandardState(this->grammar);
    }


    void iterms() {
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

       //start->printAllExpr();

        // 遍历所有状态, 直到无法添加新的状态
        for (int i = 0; i < lrStateVec.size(); ++i) {
            getAllNextState(lrStateVec.at(i));
        }

    }

    /**
     * 检测该状态是否已经在状态向量中,
     *  如果存在,
     *    则返回已经存在的状态的地址,
     * 如果之前不存在:
     *    返回NULL
     * @brief alreadyHave
     * @param lsTest
     * @return
     */
    LRState* alreadyHave(LRState *lsTest) {
        for(LRState * ls: this->lrStateVec) {
            if (lsTest->isEqual(ls)) {
                return ls;
            }
        }
        return NULL;
    }

    /**
     * 获取该状态所有的后继状态, 并将这些状态添加到状态向量组中, 同时为状态进行编号
     *
     * @brief getAllNextState
     * @param start
     * @return
     */
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
                state2id.insert(std::make_pair(nLRState, id));
                id ++;

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

    void makeACTION() {
        cout << "max state" << id << endl;
        vector<string> term;
        vector<string> nonTerm;

        for (auto it: this->grammar->term) {
            term.push_back(it);
        }
        for (auto it: this->grammar->nonTerm) {
            nonTerm.push_back(it);
        }
        term.push_back("$");


    }

    void makeGOTO() {

    }

    /**
     *  该函数递归调用自身, 打印从start开始的所有状态, 并且通过haveTravel确保一个状态不会被
     * 重复访问
     * @brief showLR1
     * @param start
     */
    void showLR1(LRState* start) {
        if (haveTravel.find(start) != haveTravel.end()) {
            return;
        }
        haveTravel.insert(start);

        std::map<char, LRState*>& lrVec = start->out;
        for(auto it: lrVec) {
            char ch = it.first;
            LRState *tmpLR = it.second;

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
