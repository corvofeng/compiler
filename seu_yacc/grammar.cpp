/*
 *=============================================================================
 *    Filename:grammar.cpp
 *
 *     Version: 1.0
 *  Created on: May 04, 2017
 *
 *      Author: corvo
 *=============================================================================
 */


#include "grammar.h"


void Grammar::dfs(Expression* pExpr) {
    if (isOver.at(pExpr) == true) {
        //cout << pExpr->left << "  ok" << endl;
        return;
    }
    isOver[pExpr] = true;

    string left = pExpr->left;
    for (auto rh : pExpr->right) {
        string& rth = rh;

        for (int i = 0; i < rth.length(); ++i) {
            if (! isupper(rth.at(i)) && rth.at(i) != '\'') {
                this->first[left].insert(rth.at(i));
                break;
            }
            if (isupper(rth.at(i))) {
                Expression *pExprTmp;
                if (i != rth.length()- 1 && rth.at(i+1) == '\'') {
                    pExprTmp = left2Expr[rth.substr(i, 2)];
                } else {
                    pExprTmp = left2Expr[rth.substr(i, 1)];
                }
                string& exprLeftTmp = pExprTmp->left;
                dfs(pExprTmp);
                set<char> & tmp = this->first[exprLeftTmp];
                set<char>::iterator it1 = tmp.begin();
                bool flag = true;
                for(; it1 != tmp.end(); it1++) {
                    if (*it1 == '~')  {
                        flag = false;
                    }
                    this->first[left].insert(*it1);
                }

                if (flag) {
                    break;
                }
            }


        }
    }
}

