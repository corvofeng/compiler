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


Grammar::Grammar(std::map<string, string>& exprFunc, string nonTermHead) {
    for(auto it: exprFunc) {
        string expr = it.first;
        string func = it.second;

        expr.erase(std::remove_if(expr.begin(), expr.end(),
                                [](char c){return (c == '\r' || c == '\t' || c == ' ');}),
                                expr.end());
        string& s = expr;
        int j, len = s.size();
        for (j = 0; j < len; ++j) {
            if (s[j] == '-') break;
        }
        string left = s.substr(0, j);
        this->nonTerm.insert(left);
        j += 2;
        string right = s.substr(j, len - j);
        for (auto it: right) {
            string s;
            s += it;
            if (isupper(it)) {
                this->nonTerm.insert(s);
            } else {
                this->term.insert(s);
            }
        }
        this->insert(left, right, func);

    }
    this->nonTermHead = nonTermHead;
}

Grammar::Grammar(string* expr, int size, string nonTermHead) {
    for (int i = 0; i < size; ++i) {
        // 消除空格
        expr[i].erase(std::remove_if(expr[i].begin(), expr[i].end(),
                                [](char c){return (c == '\r' || c == '\t' || c == ' ');}),
                                expr[i].end());
    }

    for (int i = 0; i < size; ++i) {
        std::string s = expr[i];
        int j, len = s.size();
        for (j = 0; j < len; ++j) {
            if (s[j] == '-') break;
        }
        string left = s.substr(0, j);
        this->nonTerm.insert(left);
        j += 2;
        string right = s.substr(j, len - j);
        for (auto it: right) {
            string s;
            s += it;
            if (isupper(it)) {
                this->nonTerm.insert(s);
            } else {
                this->term.insert(s);
            }
        }
        this->insert(left, right);
    }
    this->nonTermHead = nonTermHead;
}


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

