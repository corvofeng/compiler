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

    LR1(std::string *expr, int size, std::string nonTermHead, map<string, string> prior, map<string, int> assoc) {
        this->grammar = new Grammar(expr, size, nonTermHead);
        this->grammar->makeFirst();     // 产生first集合

        //this->grammar->printFirst();

        LRState::getStandardState(this->grammar);

        this->prior = prior;
        this->assoc = assoc;

    }

    // 保存优先级, 例如 * > +, 则保存为 <*, +>
    map<string, string> prior;

    // 保存结合性, 如果为*左结合, 则保存为 <*, 1>; 右结合保存为<*, 2>, 以此进行区分
    map<string, int> assoc;

    /**
     * 此函数构建LR1状态转换图, 到达一个状态后, 将会与之前的状态进行比较
     * @brief iterms
     */
    void iterms();

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
    bool getAllNextState(LRState *start);

    std::set<LRState*> haveTravel;
    std::map<LRState*, int> state2id;

    /**
     * 打印当前LR1的所有状态, 以及到达路径
     *   0<- E ->1
     * @brief printLR1
     */
    void printLR1() {
        haveTravel.clear();
        LRState* start = this->lrStateVec.at(0);
        this->showLR1(start);
    }

    /*
     * 存放ACTION表与GOTO表
     */
    vector<vector<string>> res_action;
    vector<vector<int>> res_goto;       //
    map<string, int> actionTerm;
    map<string, int> gotoNonTerm;


    void makeACTIONGOTO() {
        cout << "max state " << id << endl;

        //res_action = new vector<vector<string>>(id);  // 创建长度为id的数组
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

    void printACTIONGOTO() {

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

    /*
     * 打印文法
     */
    void printGrammar() {
        LRState* standard = LRState::lrStateStandard;
        int size = standard->coreExpr.size();
        for (int i = 0; i < size; ++i) {
            cout << i << ": " ;
            SingleExpress *sTmp = standard->coreExpr.at(i);
            sTmp->printSigleExpr();
        }
    }


    /**
     * 构造Action表, 事实表明, 这是一个糟糕的函数, 即使使用了递归也无法解救这个函数
     *
     *   我们平时所建的LR1表需要考虑到二义性文法的问题, 一旦出现二义性文法, 我们需要做出说明.
     *  如果某个文法是有二义性的, 而且文法中还未定义优先级和结合性, 那么在LR1阶段是被认为错误
     * 的选择
     *  同样, 如果某个文法定义了优先级和结合性, 那么, 我们需要在有冲突的位置做出反应, 对于
     * 有二义性的位置, 需要做出抉择, 确保我们的预测分析表具有唯一性
     *
     * @brief actionHelp
     * @param start
     * @param res_action
     * @param term
     */
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

                    if (!res_action[col][row].empty()) {  // 由于该过程总是最先进行, 不可能会在此处产生冲突
                        cout << "we have some thing wrong in " << col << " " << row << "r" + std::to_string(reduceR) <<  endl;
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
                    cout << "we have some thing wrong in " << col << " " << row << res_action[col][row] << endl;
                    */
                    string old_reduce = res_action[col][row];


                    if(this->prior.find(t) != this->prior.end()) { // 当前项优先级较高, 需要进行移入操作
                        string be_lose = this->prior.at(t);
                        for (int i = 0; i < start->coreExpr.size(); i++) {
                            SingleExpress* sExprTmp = start->coreExpr.at(i);
                            if (sExprTmp->right.size() <= sExprTmp->pos && sExprTmp->right.find(be_lose) != string::npos) {
                                res_action[col][row] = "s" + std::to_string(shiftR);
                            }
                        }

                    } else {
                        // 当前项优先级较低, 先进行规约操作, 例如当前项为+, 式子终结符为*, 则先对前面一项进行规约
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
