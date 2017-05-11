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

    LR1(string *expr, int size, string nonTermHead,
          map<string, string> prior, map<string, int> assoc) {

        this->grammar = new Grammar(expr, size, nonTermHead);
        this->grammar->makeFirst();     // 产生first集合

        //this->grammar->printFirst();

        LRState::getStandardState(this->grammar);

        this->prior = prior;
        this->assoc = assoc;

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


    // 保存优先级, 例如 * > +, 则保存为 <*, +>
    map<string, string> prior;

    // 保存结合性, 如果为*左结合, 则保存为 <*, 1>; 右结合保存为<*, 2>, 以此进行区分
    map<string, int> assoc;

    /**
     *   此函数构建LR1状态转换图, 首先插入(push_back)初始状态. 因此, lrStateVec[0]为初
     * 始状态, 并且开始调用getAllNextState(start), 如果此时状态增加了, 那么将会继续循环
     * 直至所有状态都获取了它的下一个状态族
     *
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
     *   获取该状态所有的后继状态, 如果该状态已经存在于之前的状态中, 那么将会将原状态添加到该状
     * 态的后继状态族中, 此时不向状态向量组中添加状态
     *
     *   如果此时获得的状态为新的状态, 该状态添加到状态向量组中, 同时为状态进行编号, 编号信息
     * 保存在state2id映射中
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
    vector<vector<int>> res_goto;
    map<string, int> actionTerm;
    map<string, int> gotoNonTerm;


    /**
     * 此函数制作ACTION与GOTO表
     * @brief makeACTIONGOTO
     */
    void makeACTIONGOTO();


    /**
     * 打印ACTION与GOTO表
     * @brief printACTIONGOTO
     */
    void printACTIONGOTO();


    /**
     * 打印文法
     * @brief printGrammar
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
    void actionHelp(LRState *start, vector<vector<string>> &res_action,
                    map<string, int>& term);


    /**
     * @brief gotoHelp
     * @param start
     * @param res_goto
     * @param nonTerm
     */
    void gotoHelp(LRState *start, vector<vector<int>> &res_goto,
                  map<string, int>& nonTerm);


    /**
     *  该函数递归调用自身, 打印从start开始的所有状态, 并且通过haveTravel确保一个状态不会被
     * 重复访问 (辅助函数)
     * @brief showLR1
     * @param start
     */
    void showLR1(LRState* start);


};


#endif
