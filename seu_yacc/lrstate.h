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
#include "grammar.h"

/**
 *   该类记录了LR1状态转换图中的每一个状态, 对于各种状态信息来说, 我们需要有标准状态来与其进行
 * 对比, 此类拥有静态变量lrStateStandard, 在调用此类时, 首先进行getStandardState, 将标准
 * 状态进行记录
 * @brief The LRState class
 */
class LRState {
public:
    static map<string, set<char>> first;
    static LRState* lrStateStandard;
    static string nonTermHead;

    std::map<char, LRState*> out;
    std::set<char> maybeNext;

    string termAll;

    /**
     *
     * acc = -2; 表示该状态属于中间状态, 只能进行移入
     * acc = -1; 表明该状态为可以接受, 即为完成
     * acc >= 0; 表示使用第acc个数字的产生式可以将进行规约
     *
     */
    int acc = -2;

    LRState(){}

    ~LRState() {
        for (auto single : this->singleExprVec){
            delete single;
        }
    }

    vector<SingleExpress*> singleExprVec;   // 存储所有的产生式, 方便最后内存的释放

    /**
     *  对于普通的状态, 该变量表示核心的expr
     *
     *  对于静态变量lrStateStandard, 此时的coreExpr就表示所有的标准的产生式, 并且每个
     * 产生式位置也为固定值
     * @brief coreExpr
     */
    vector<SingleExpress*> coreExpr;

    /**
     * 获取标准的文法, 此处使用单例模式, 防止重复创建
     * @brief getStandardState
     * @param grammer
     * @return
     */
    static LRState* getStandardState(Grammar *grammer);


    /**
     *  最终进行清理标准状态被调用
     * @brief deleteStandardState
     */
    static void deleteStandardState() {
        if (lrStateStandard) {
            delete lrStateStandard;
        }
        lrStateStandard = NULL;
    }


    /**
     *  由核心状态扩展至全部状态, 其中, 当某个核心态达到了截止状态时, 需要将该状态进行记录
     * @brief findAllExpr
     */
    void findAllExpr();


    /**
     * 　通过表达式的左值右值确定表达式编号, (此函数中使用了标准状态)
     * @brief findExprByLeftRight
     * @param left
     * @param right
     * @return
     */
    int findExprByLeftRight(string left, string right) {
        //cout << "left is " << left << " right is " << right << endl;
        vector<SingleExpress*> sExprVec = lrStateStandard->coreExpr;
        for (int i = 0; i < sExprVec.size(); ++i) {
            SingleExpress* tmp = sExprVec.at(i);
            if (tmp->left == left && tmp->right == right) {
                return i;
            }
        }
        cout << "we have a mistake" << endl;
        exit(1);
    }


    /**
     * 函数主要在findAllExpr中被调用,  来增加当前状态中表达式的个数, 例如
     *
     * S-> .A, $ (输入为A$)
     * 直接将A -> .<产生式>, $      添加到向量组中
     *
     * S-> .ABC, $ (输入为ABC$)
     * 将A -> .<产生式>, FIRST{B}  添加到向量组中
     *
     * S-> .Abc, $ (输入为Abc$)
     * 将 A -> .<产生式>, b        添加到向量组中
     *
     * @brief increaseState
     * @param s
     * @param term
     */
    void increaseState(string &s);


    /**
     * 从标准的表达式中添加左值为left的表达式, 同时将其终止符置为term (此函数能够防止重复添加)
     *
     * @brief findAndPush
     * @param left
     * @param term
     */
    void findAndPush(string& left, char term) {
        for (auto state: lrStateStandard->coreExpr) {
            SingleExpress *standardState = state;
            if(standardState->left == left) {
                SingleExpress* newSExpr =
                        new SingleExpress(left, standardState->right, term);
                this->maybeNext.insert(standardState->right[0]);
                if(isExists(newSExpr)) {
                    delete newSExpr;
                    continue;
                }
                this->singleExprVec.push_back(newSExpr);
            }
        }
    }

    /**
     * 根据核心状态, 判断两个状态是否为同一状态
     * @brief isEqual
     * @param tmpLRState
     * @return
     */
    bool isEqual(LRState* tmpLRState);


    /*
     * 判断该表达式是否已经存在于向量中,
     * 如果向量存在, 则返回已经存在状态的原始地址,
     * 否则, 返回NULL
     */
    SingleExpress* isExists(SingleExpress* sExprTmp) {
        for (auto it : this->singleExprVec){
            SingleExpress* sExpr = it;
            if (sExpr->isEqual(sExprTmp)) {
                return sExpr;
            }
        }
        return NULL;
    }


    /**
     * 打印所有的表达式, 包括此个状态中的所有表达式以及终止符
     * @brief printAllExpr
     */
    void printAllExpr() {
        for (auto it : this->singleExprVec) {
            SingleExpress* sExpr = it;
            sExpr->printSigleExpr();
        }
    }


    void addCoreExpr(string &left, string &right, char term) {
        SingleExpress* singleExpress = new SingleExpress(left, right, term);
        this->coreExpr.push_back(singleExpress);
        this->singleExprVec.push_back(singleExpress);
        this->maybeNext.insert(right.at(0));
    }


    void addCoreExpr(string &left, string &right, int pos, char term) {
        SingleExpress* singleExpress = new SingleExpress(left, right, pos, term);
        this->coreExpr.push_back(singleExpress);
        this->singleExprVec.push_back(singleExpress);

        if (pos < right.size()) {
            this->maybeNext.insert(right.at(pos));
        }
    }


    void addExpr(Expression& expr) {
        string left = expr.left;
        set<string>& rht = expr.right;
        for (auto it: rht) {
            char tmpC = ('$');
            SingleExpress* singleExpress = new SingleExpress(left, it, tmpC);
            this->singleExprVec.push_back(singleExpress);
        }
        return;
    }

};


#endif
