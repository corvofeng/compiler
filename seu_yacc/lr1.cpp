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


void LR1::makeACTIONGOTO() {
    //cout << "max state " << id << endl;

    res_action.resize(id);
    res_goto.resize(id);

    int termId = 0;
    for (auto it: this->grammar->term) {
        actionTerm.insert(std::make_pair(it, termId));
        termId ++;
    }
    actionTerm.insert(std::make_pair("$", termId));
    termId++;

    int nonTermId = 0;
    for (auto it: this->grammar->nonTerm) {
        gotoNonTerm.insert(std::make_pair(it, nonTermId));
        nonTermId++;
    }

    // 初始化二维数组
    int nonTermSize = gotoNonTerm.size();
    for (int i = 0; i < id; ++i) {
        res_action[i].resize(termId);
    }

    // 初始化二维数组
    int termSize = actionTerm.size();
    for (int i = 0; i < id; ++i) {
        res_goto[i].resize(nonTermId);
    }

    this->haveTravel.clear();
    actionHelp(this->lrStateVec.at(0), res_action, actionTerm);

    this->haveTravel.clear();
    gotoHelp(this->lrStateVec.at(0), res_goto, gotoNonTerm);

}


void LR1::printACTIONGOTO() {

    cout << "Print action" << endl;
    for (auto it: actionTerm) {
        cout << it.first << ":  " << it.second << endl;
    }

    int termSize = actionTerm.size();
    for (int i = 0; i < id; ++i) {
        for (int j = 0; j < termSize; ++j) {
            cout << "\t*" << (res_action)[i][j] << "*\t";
        }
        cout << endl;
    }


    cout << "Print goto" << endl;
    int nonTermSize = gotoNonTerm.size();
    for (auto it: gotoNonTerm) {
        cout << it.first << ":  " << it.second << endl;
    }
    for (int i = 0; i < id; ++i) {
        for (int j = 0; j < nonTermSize; ++j) {
            cout << "\t*" << res_goto[i][j] << "*\t";
        }
        cout << endl;
    }
}


void LR1::actionHelp(LRState *start, vector<vector<string>> &res_action,
                map<string, int>& term) {
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

                // 由于该过程总是最先进行, 不可能会在此处产生冲突
                if (!res_action[col][row].empty()) {
                    cout << "we have some thing wrong in "
                         << col << " " << row << "r" + std::to_string(reduceR) <<  endl;
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

            if (!res_action[col][row].empty()) { // 先前已有规约, 需要考虑后再插入
                /*
                cout << "The t is " << t << " " << "termAll " << start->termAll << " ";
                cout << "we have some thing wrong in " << col
                        << " " << row << res_action[col][row] << endl;
                */
                string old_reduce = res_action[col][row];

                // 当前项优先级较高, 需要进行移入操作
                if(this->prior.find(t) != this->prior.end()) {
                    string be_lose = this->prior.at(t);
                    for (int i = 0; i < start->coreExpr.size(); i++) {
                        SingleExpress* sExprTmp = start->coreExpr.at(i);
                        if (sExprTmp->right.size() <= sExprTmp->pos
                                && sExprTmp->right.find(be_lose) != string::npos) {
                            res_action[col][row] = "s" + std::to_string(shiftR);
                        }
                    }

                } else {
                    // 当前项优先级较低, 先进行规约操作, 例如当前项为+, 式子终结符为*,
                    // 则先对前面一项进行规约
                }

                // 处理结合性
                for (auto it_term: start->termAll) {
                    if (ch == it_term) {

                        /*
                        cout << "----------" << endl;
                        cout << "The ch is " << ch << endl;
                        cout << "----------" << endl;
                        */

                        if (this->assoc.at(t) == 1) {
                            // 左结合默认为规约操作,
                        } else {    //  右结合则继续移入
                            res_action[col][row] = "s" + std::to_string(shiftR);
                        }
                    }
                }

            } else {
                res_action[col][row] = "s" + std::to_string(shiftR);
            }

        }


//      cout << state2id.at(start)
//          << "<- " << ch << " ->" << state2id.at(tmpLR) << endl;;
        this->actionHelp(tmpLR, res_action, term);
    }
}

void LR1::gotoHelp(LRState *start, vector<vector<int>> &res_goto,
                   map<string, int>& nonTerm) {

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


void LR1::showLR1(LRState* start) {
    if (haveTravel.find(start) != haveTravel.end()) {
        return;
    }
    haveTravel.insert(start);

    std::map<char, LRState*>& lrVec = start->out;
    for(auto it: lrVec) {
        char ch = it.first;
        LRState *tmpLR = it.second;

        cout << state2id.at(start) <<
                "<- " << ch << " ->" << state2id.at(tmpLR) << endl;;
        this->showLR1(tmpLR);
    }
}

