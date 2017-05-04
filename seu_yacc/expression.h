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

using std::string;
using std::set;

/**
 * 保存单个表达式, 例如
 * S -> a | b
 * @brief The Expression class
 */
class Expression
{
public:
    string left;
    set<string> right;

    Expression(string s) {
        left = s;
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

};

#endif
