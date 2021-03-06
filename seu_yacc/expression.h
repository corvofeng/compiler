/*
 *=============================================================================
 *    Filename:expression.h
 *
 *     Version: 1.0
 *  Created on: May 04, 2017
 *
 *      Author: corvo
 *=============================================================================
 */

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include <set>
#include <string>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <map>

using std::string;
using std::set;
using std::cout;
using std::endl;

/**
 * 保存单个表达式, 例如
 * S -> a | b
 *
 *  该表达式的形式主要用于语法定义, 且适合于求解FIRST集合
 * @brief The Expression class
 */
class Expression
{
public:
    string left;
    set<string> right;
   // string func;
    std::map<string, string> right2func;

    Expression(string left) {
        this->left = left;
    }

    void printExpr() {
        printf("%s -> ", left.c_str());
        auto it = right.begin();
        if (right.begin() != right.end()) {
            printf("%s", (*it).c_str());
            it ++;
        }
        for (; it != right.end(); it++) {
            printf("|%s", (*it).c_str());
        }

        printf("\n");
    }

    void insert(string s) {
        right.insert(s);
    }

    void insert(string& s, string& func) {
        right.insert(s);
//        this->func = func;
        right2func.insert(std::make_pair(s, func));
    }

};

/**
 *   单个表达式的表示, 在LR1状态转换图中, 单个的表达式需要有各种信息记录, 其中包括:
 * 当前表达式解析到的位置, 当前表达式的终止符, 此表达式主要用于转换图中状态的记录
 *
 * 例如:
 *   S -> aB.c,  $
 * 其中.表示此次解析到达c字母之前, 而且此表达式的终止符为$
 *
 * @brief The SingleExpress class
 */
class SingleExpress
{
public:
    string left;
    string right;
    string func;
    int pos = 0;
    char term;

    SingleExpress() {}


    SingleExpress(string left, string right, int pos, char term) {
        this->left = left;
        this->right = right;
        this->term = term;
        this->pos = pos;
    }


    SingleExpress(string left, string right, int pos, char term, string func) {
        this->left = left;
        this->right = right;
        this->term = term;
        this->pos = pos;
        this->func = func;
    }


    SingleExpress(string &left, string &right, char term) {
        this->left = left;
        this->right = right;
        this->term = term;
    }


    /**
     * 判断两个表达式是否相同, 如果各项状态都相同方可
     * @brief isEqual
     * @param tmpExpr
     * @return
     */
    bool isEqual(SingleExpress *tmpExpr) {

        if (this->left == tmpExpr->left &&
                this->right == tmpExpr->right &&
                this->term == tmpExpr->term &&
                this->pos == tmpExpr->pos) {

            return true;
        }
        return false;
    }


    void printSigleExpr() {
        cout << left << " -> " <<
                right.substr(0, pos) << '.' << right.substr(pos) << "    ";

        cout << "term char: " << term << " ---- " << func;

        cout << endl;
    }
};

#endif
