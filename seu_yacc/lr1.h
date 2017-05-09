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

        this->grammar->printFirst();

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
        cout << "max state " << id << endl;
        map<string, int> term;
        map<string, int> nonTerm;

        vector<vector<string>> res_action(id);  // 创建长度为id的数组
        vector<vector<int>> res_goto(id);       //


        int termId = 0;
        for (auto it: this->grammar->term) {
            term.insert(std::make_pair(it, termId));
            termId ++;
        }
        term.insert(std::make_pair("$", termId));
        termId++;

        int nonTermId = 0;
        for (auto it: this->grammar->nonTerm) {
            nonTerm.insert(std::make_pair(it, nonTermId));
            nonTermId++;
        }

        // 初始化二维数组
        int nonTermSize = nonTerm.size();
        for (int i = 0; i < id; ++i) {
            res_action[i].resize(termId);
        }

        // 初始化二维数组
        int termSize = term.size();
        for (int i = 0; i < id; ++i) {
            res_goto[i].resize(nonTermId);
        }

        //string test = "$";
        //cout << term.at(0) << endl;
        this->haveTravel.clear();
        actionHelp(this->lrStateVec.at(0), res_action, term);

        //this->printLR1();
        //this->printGrammar();

        cout << "Print action" << endl;

        for (auto it: term) {
            cout << it.first << ":  " << it.second << endl;
        }

        for (int i = 0; i < id; ++i) {
            for (int j = 0; j < termId; ++j) {
                cout << "\t*" << res_action[i][j] << "*\t";
            }
            cout << endl;
        }

        this->haveTravel.clear();
        gotoHelp(this->lrStateVec.at(0), res_goto, nonTerm);

        cout << "Print goto" << endl;
        for (auto it: nonTerm) {
            cout << it.first << ":  " << it.second << endl;
        }
        for (int i = 0; i < id; ++i) {
            for (int j = 0; j < nonTermId; ++j) {
                cout << "\t*" << res_goto[i][j] << "*\t";
            }
            cout << endl;
        }
    }

    void printGrammar() {
        LRState* standard = LRState::lrStateStandard;
        int size = standard->coreExpr.size();
        for (int i = 0; i < size; ++i) {
            cout << i << ": " ;
            SingleExpress *sTmp = standard->coreExpr.at(i);
            sTmp->printSigleExpr();
        }
    }


    void actionHelp(LRState *start, vector<vector<string>> &res_action, map<string, int>& term) {
        if (haveTravel.find(start) != haveTravel.end()) {
            return;
        }
        haveTravel.insert(start);
        std::map<char, LRState*>& lrVec = start->out;

        /*
         * 当前状态如果为终止状态
         */
        if (start->acc == -1) {
            int col = state2id.at(start);
            int row = term.at("$");
            res_action[col][row] = "acc";
        } else if (start->acc >= 0) {
            int col = state2id.at(start);
            int reduceR = start->acc;
            for (auto it: start->coreExpr) {
                SingleExpress *sExpr = it;
                if (sExpr->pos >= sExpr->right.size()) {
                    string t;
                    t += sExpr->term;
                    int row = term.at(t);

                    if (!res_action[col][row].empty()) {
                        cout << "we have some thing wrong in " << col << " " << row << endl;
                        exit(1);
                    }

                    res_action[col][row] = "r" + std::to_string(reduceR);
                }
            }
        }

        int col = state2id.at(start);
        for(auto it: lrVec) {
            char ch = it.first;
            LRState *tmpLR = it.second;
            int tmpId = state2id.at(tmpLR);

            if (!isupper(ch)) {
                string t;
                t += ch;
                int row = term.at(t);
                int shiftR = state2id.at(tmpLR);
                if (!res_action[col][row].empty())
                    cout << "we have some thing wrong in " << col << " " << row << endl;
                res_action[col][row] = "s" + std::to_string(shiftR);
            }


//            cout << state2id.at(start) << "<- " << ch << " ->" << state2id.at(tmpLR) << endl;;
            this->actionHelp(tmpLR, res_action, term);
        }
    }

    void makeGOTO() {

    }

    void gotoHelp(LRState *start, vector<vector<int>> &res_goto, map<string, int>& nonTerm) {
        if (haveTravel.find(start) != haveTravel.end()) {
            return;
        }
        haveTravel.insert(start);
        std::map<char, LRState*>& lrVec = start->out;

        int col = state2id.at(start);
        for(auto it: lrVec) {
            char ch = it.first;
            LRState *tmpLR = it.second;
            int tmpId = state2id.at(tmpLR);

            if (isupper(ch)) {
                string t;
                t += ch;
                int row = nonTerm.at(t);
                int shiftR = state2id.at(tmpLR);
                res_goto[col][row] =  shiftR;
            }


//            cout << state2id.at(start) << "<- " << ch << " ->" << state2id.at(tmpLR) << endl;;
            this->gotoHelp(tmpLR, res_goto, nonTerm);
        }
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
