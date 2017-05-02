/*
 * =============================================================================
 *    Filename:nfa.h
 *
 *     Version: 1.0
 *  Created on: April 25, 2017
 *
 *      Author: corvo
 *=============================================================================
 */

#ifndef NFA_H_
#define NFA_H_

#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <stack>
#include <set>
#include <map>
#include "state.h"

using std::string;
using std::cout;
using std::endl;

/*
 * 每条正则语句对应于一条NFA
 *
 */
class Re2NFA
{
public:
    Re2NFA(char *str)
    {
        strExpress = "";
        statusId = 0;
        strcpy(this->re, str);
    }

    Re2NFA(string str)
    {
        strExpress = str;
    }

    ~Re2NFA()
    {
        haveTravel.clear();
        // TODO 删除s
        free(nfa_s);
        nfa_s = NULL;
        nfa_e = NULL;
    }

    /**
     * 析构对象时, 需要考虑链表的递归引用, 一个对象不可以重复删除
     */
    void free(State *start) {
        if (start == NULL ||
            haveTravel.find(start) != haveTravel.end() ) {
            return;
        }

        haveTravel.insert(start);

        free(start->out);
        free(start->out1);

        delete start;
    }

    void printNFA() {
        haveTravel.clear();
        this->showNFA(this->nfa_s);
    }

    void strToNFA() {
        /*
        char *p = re;
        for(; *p; *p++) {
            printf("%d\n", *p);
        }
        cout << this->re << endl;
        */
        re2post(this->re);

       // cout << this->buf << endl;
        post2nfa(this->buf);

    }


    State *getStart() {
        return this->nfa_s;
    }

    std::set<int> getCharSet() {
        return this->char_set;
    }

private:
    State *post2nfa(char * postfix);
    void showNFA(State * start);
    char *re2post(char * re);

    string strExpress;
    int statusId;
    State * nfa_s;      // 保存解析后的数据
    State * nfa_e;		// 最终符合条件的状态
    std::set<int> char_set; // 输入符号集合

    char buf[1024] = {0};
    char re[1024] = {0};

    int id = 0;
    std::set<State *> haveTravel;
    std::map<State *, int> state2id;
};

#endif
