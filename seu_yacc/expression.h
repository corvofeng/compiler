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
//#include "grammar.h"

//class Grammar;

using std::string;
using std::set;
using std::cout;
using std::endl;

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

};

// 单个表达式
class SingleExpress
{
public:
    string left;
    string right;
    int pos = 0;
    char term;

    SingleExpress() {}

    SingleExpress(string left, string right) {

    }

    SingleExpress(string &left, string &right, char term) {
        this->left = left;
        this->right = right;
        this->term = term;
    }

    void printSigleExpr() {
        cout << left << " -> " << right.substr(0, pos) << '.' << right.substr(pos) << "    ";

        cout << "term char: " << term;

        /*
        for (auto ch : term) {
            cout << ch << " ";
        }
        */
        cout << endl;
    }
};

using std::vector;



#endif
