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

        LRState::first = grammer->first;
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
            cout << "Please create standard list!!!" << endl;
            exit(1);
        }
        cout << "find all expr" << endl;

        auto it = this->singleExprVec.begin();

        int i = 0;
        while (i < singleExprVec.size()) {
            SingleExpress* sExpr = singleExprVec[i];
            int pos = sExpr->pos;
            string &right = sExpr->right;
            char ch = sExpr->right[pos];
            char term = sExpr->term;



            string Beta = right;

            Beta += term;
            cout << "The Beta is " << Beta << endl;
            increaseState(Beta);
            i++;
        }

        printAllExpr();
    }

    /**
     * 通过Bterm来增加表达式向量的个数, 例如
     * S-> .ABC, $
     *
     * 将会输入 BC$
     *
     * 此时, 会进行查找
     *
     *
     *
     * @brief increaseState
     * @param s
     * @param term
     */
    void increaseState(string &s) {

        if(s.size() == 2) {
            string left = s.substr(0, 1);
            char term = s[1];
            cout << "The size is 2 left is " << left << " term is "<< term << endl;
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

            } else {        // ch为终结符
                cout << "The char is " << ch << endl;
                findAndPush(left, ch);
                break;
            }
        }
    }

    void findAndPush(string& left, char term) {
        for (auto state: lrStateStandard->coreExpr) {
            SingleExpress *standardState = state;
            if(standardState->left == left) {
                SingleExpress* newSExpr =
                        new SingleExpress(left, standardState->right, term);

                if(isExists(newSExpr)) {
                    delete newSExpr;
                    continue;
                }
                this->singleExprVec.push_back(newSExpr);
            }
        }
    }

    /*
     * 判断该表达式是否已经存在于向量中,
     * 如果向量存在, 则返回原始地址,
     * 否则, 返回NULL
     */
    SingleExpress* isExists(SingleExpress* sExpr) {
        for (auto it : this->singleExprVec){
            SingleExpress* sExprTmp = it;
            if (sExpr->left == sExprTmp->left
                    && sExpr->right == sExprTmp->right
                    && sExpr->term == sExprTmp->term) {
                return sExpr;
            }
        }
        return NULL;
    }

    /**
     * 获取First(B)的终结符号集合
     * @brief getFirst
     * @param s
     * @return
     */
    void getFirst(string& s, char oldTerm) {
        if (first.empty()) {
            cout << "Please create standard first set" << endl;
            exit(1);
        }

        cout << s << " " << oldTerm << endl;
        set<char> termSet;
        bool hasExp = true;
        for (auto ch : s) {
            hasExp = false;
            if (std::isupper(ch)) {

                string s;
                s += ch;
                for (auto state: lrStateStandard->coreExpr) {
                    SingleExpress *standardState = state;
                    if(standardState->left == s) {

                    }
                }
            /*
                set<char>& tmpSet = this->first.at(ch);
                termSet.insert(tmpSet.begin(), tmpSet.end());
                if (tmpSet.find('~') != tmpSet.end()) {
                    continue;
                    hasExp = true;
                } else {
                    break;
                }
            */
            } else {     // 非大写字母, 终结符直接插入
                termSet.insert(ch);
                break;
            }
        }

        if (hasExp) {
            //termSet.insert(oldTerm.begin(), oldTerm.end());
        }

        //return termSet;
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

    LRState* alreadyHave(LRState *lsTest) {

    }

    void addCoreExpr(string &left, string &right, char term) {
        SingleExpress* singleExpress = new SingleExpress(left, right, term);
        this->coreExpr.push_back(singleExpress);
        this->singleExprVec.push_back(singleExpress);
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

#endif
