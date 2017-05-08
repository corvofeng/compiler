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

class LRState {
public:
    static map<string, set<char>> first;
    static LRState* lrStateStandard;
    static string nonTermHead;

    std::map<char, LRState*> out;
    std::set<char> maybeNext;

    /**
     * acc = -1; 表示该状态属于中间状态, 只能进行移入
     * acc = 0; 表明该状态为可以接受, 即为完成
     * acc > 0 表示使用第acc个数字的产生式可以将进行规约
     *
     */
    int acc = -1;

    LRState(){}

    ~LRState() {
        for (auto single : this->singleExprVec){
            delete single;
        }
    }

    vector<SingleExpress*> singleExprVec;   // 存储所有的产生式

    /**
     * 对于普通的状态, 该变量表示核心的expr
     *
     *  对于静态变量lrStateStandard, 此时的coreExpr就表示所有的标准的产生式, 并且每个
     * 产生式位置也为固定值
     * @brief coreExpr
     */
    vector<SingleExpress*> coreExpr;

    /**
     * 获取标准的文法, 此处使用单例模式, 防止重复创建
     */
    static LRState* getStandardState(Grammar *grammer) {
        if (lrStateStandard) {
            return lrStateStandard;
        }
        LRState *lrState = new LRState();

        LRState::first = grammer->first;
        for (auto it: grammer->pExprVec) {
            Expression *expr = it;
            string left = expr->left;
            for (auto right: expr->right) {
                //SingleExpress* sExpr = new SingleExpress(left, right);
                lrState->addCoreExpr(left, right, {});
            }
        }
        nonTermHead = grammer->nonTermHead;
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

                if (right.at(pos-1) == nonTermHead.at(0)) {
                    this->acc = 0;
                } else {
                    this->acc = this->findExprByLeftRight(sExpr->left, right);
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
     * 通过Bterm来增加表达式向量的个数, 例如
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
    void increaseState(string &s) {

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

    /**
     * 从标准的表达式中添加左值为left的表达式, 同时将其终止符置为term
     *
     * 此函数能够防止重复添加
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

    bool isEqual(LRState* tmpLRState) {
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

LRState* LRState::lrStateStandard = NULL;
map<string, set<char>> LRState::first;
string LRState::nonTermHead;

#endif
