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
#include <vector>
#include <algorithm>

using std::map;
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::set;

/**
 * 该类保存语法, 例如
 * S -> aSb
 * S -> ~
 * @brief The Grammar class
 */
class Grammar
{
public:
    vector<Expression*> pExprVec;
    map<string, Expression*> left2Expr;
    map<string, set<char>> first;
    string nonTermHead;        // 获得非终结符的首结点
    set<string> term;            // 保存终结符的集合a, b, +, i ...
    set<string> nonTerm;         // 保存非终结符的集合(一定为大写字母) S, A, B ..


    Grammar() {}
    Grammar(string* expr, int size, string nonTermHead) {
        for (int i = 0; i < size; ++i) {
            // 消除空格
            expr[i].erase(std::remove_if(expr[i].begin(), expr[i].end(),
                                    [](char c){return (c == '\r' || c == '\t' || c == ' ');}),
                                    expr[i].end());
        //    std::cout << expr[i] << std::endl;
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

    string getNonTermHead() {
        return this->nonTermHead;
    }

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

    /**
     * 此代码并非本人书写, 其中主要参考了如下代码:
     *     http://www.kancloud.cn/digest/compile-principle/143016
     *
     * 只能简单做下解析
     * 对于FIRST集合的求解, 主要有4条规则
     *  1. if X是终结符, 那么 FIRST(X) = {X}, 终结符的FIRST集就是它本身
     *  2. if X不是非终结符,  且有产生式X→a…，a∈VT,  则 a∈FIRST(X),
     *     X→~,则~∈FIRST(X)
     *  3. if X -> Y1 Y2 Y3 .. YK, 如果存在Y1 Y2 .. YK-1能够推导出 ~,
     *    那么FIRST(YK)∈FIRST(X)
     *
     * 而在程序中采用了一种不太相同的思路:
     *
     * if 该项(X)已经完成解析 return
     * else 标记为解析状态
     *
     * 遍历该项的所有产生式(例如 F->(E)|i 将会依次遍历F->(E), 与F->i):
     *
     *     遍历每个产生式中的单元 为x:
     *         if x为终结符, 直接加入  结束内层循环
     *
     *         if x为非终结符:
     *               取出该终结符对应的产生式
     *               dfs(产生式) 递归调用
     *               将该项的的FIRST集合归并到FIRST(X)集合中
     *               if 产生式的存在~
     *                   说明规则三得到了满足, 需要继续看下一个非终结符的状态
     *               else
     *                  结束内层循环
     *
     * @brief dfs
     * @param pExpr
     */
    void dfs(Expression* pExpr);

    void printFirst() {
        cout << "\nprint first : " << endl;
        for(auto it: this->first) {
            const string& left = it.first;
            set<char> &chSet = it.second;

            cout << left << ": ";
            for (auto ch : chSet){
                cout << ch << " ";
            }
            cout << endl;
        }
    }

    map<Expression*, bool> isOver;

    /**
     * 获取该文法的First集合, 通过dfs函就数进行计算
     * @brief makeFirst
     */
    void makeFirst() {
        // 此处虽为循环调用, 但实际上执行时, 很多情况会直接返回, 复杂度并不很高
        for (auto it : pExprVec) {
            Expression *pExpr = it;
            dfs(pExpr);
        }
    }


    void insert(string left, string right) {

        if (left2Expr.find(left) != left2Expr.end()) {
            left2Expr.at(left)->insert(right);
            return;
        }

        Expression *pExpr = new Expression(left);
        pExpr->insert(right);
        left2Expr.insert(std::make_pair(left, pExpr));
        pExprVec.push_back(pExpr);
        isOver[pExpr] = false;
    }

};

#endif
