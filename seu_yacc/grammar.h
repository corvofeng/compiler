/*
 *=============================================================================
 *    Filename:grammar.h
 *
 *     Version: 1.0
 *  Created on: May 04, 2017
 *
 *      Author: corvo
 *=============================================================================
 */

#ifndef GRAMMAR_H_
#define GRAMMAR_H_

#include "expression.h"
#include <map>

using std::map;

/**
 * 该类保存语法, 例如
 * S -> aSb
 * S -> ~
 * @brief The Grammar class
 */
class Grammar
{
public:
//    set<Expression*> pExprSet;

    map<string, Expression*> left2Expr;

    map<string, set<char>> first;


    Grammar() {}

    ~Grammar() {
        for (auto it = left2Expr.begin(); it != left2Expr.end(); it++) {
            Expression *expr = (*it).second;
            delete expr;
        }
    }

    void printGram() {
        for (auto it = left2Expr.begin(); it != left2Expr.end(); it++) {
            Expression *expr = (*it).second;
            expr->printExpr();
        }
    }

    void dfs(Expression* expr, bool *isOver) {

    }

    void makeFirst() {
        const int MAX = 1024;
        int size = left2Expr.size();
        bool isOver[MAX];

        for (int i = 0; i < size; ++i) {
            isOver[i] = false;
        }

        /*
        for (int i = 0; i < size; ++i) {
            dfs(i, &isOver);
        }
        */
    }


    void insert(string left, string right) {

        if (left2Expr.find(left) != left2Expr.end()) {
            left2Expr.at(left)->insert(right);
            return;
        }

        Expression *pExpr = new Expression(left);
        pExpr->insert(right);
        left2Expr.insert(std::make_pair(left, pExpr));
    }

};

#endif
