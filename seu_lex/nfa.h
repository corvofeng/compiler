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
#include <algorithm>
#include "state.h"

using std::string;
using std::cout;
using std::endl;

/**
 * 抽象基类, 此类的产生完全是为了使子类覆盖这两个函数, 以便DFA转化时需要
 */
class NFA {
public:
    virtual State *getStart() = 0;
    virtual std::set<int> getCharSet() = 0;
};

/*
 * 每条正则语句对应于一条NFA
 *
 */
class Re2NFA: public NFA
{
public:
    Re2NFA(char *str)
    {
        strcpy(this->re, str);
    }

    Re2NFA(char *str, string func)
    {
        strcpy(this->re, str);
        this->func = func;
    }

    ~Re2NFA()
    {
        if (nfa_s == NULL) {
            return;
        }
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
        start->out = NULL;
        free(start->out1);
        start->out1 = NULL;

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

    /*
     * 由后缀表达式形式转化为NFA
     * 代码参考(仅参考) https://swtch.com/~rsc/regexp/
     *
     * 此函数中new了众多变量, 及时delete
     */
    State *post2nfa(char * postfix);
    void showNFA(State * start);

    /**
     * 此段代码来源:
     * https://swtch.com/~rsc/regexp/
     *
     *
     * Convert infix regexp re to postfix notation
     * Insert . as explicit concatenation operator.
     *
     * 中缀转化为后缀, 同时插入'.'作为连接符
     * 例如ab, 转化为后缀为'ab.'
     *
     * 字符具有高于运算符的优先级, "m|food"匹配"m"或是"food", 如果想要匹配
     * "mood"或是"food", 需要使用"(m|f)ood"进行匹配.
     *
     *   通过此函数的使用, 使我对与lex中的正则匹配的机制有了更深的理解, 希望大家也能通过此函数
     * 的使用将正则表达式转换为后缀形式,
     *
     *   原有的程序中未解决字符转义的问题, 例如, 当我们想要'+|-', 或是想要'(|)', 原有算法均会产
     * 生问题, 而后的转换中, 我做了一些修改, 将转义字符的解析加入了进去, 上面的两个正则表达式需要
     * 以这样的方式进行传递: '\\+|\\-', '\\(|\\)' ,  使用两个转义'\'是由于c语言自身的原因, 本
     * 身会进行一次转义, 因此, 我们需要自己来添加
     */
    char *re2post(char * re);

    char buf[1024] = {0};
    char re[1024] = {0};
    string func;            // 该NFA所对应的终止状态的处理函数, 该NFA所对应的结局
    State * nfa_s = NULL;   // 保存解析后的数据
    State * nfa_e = NULL;   // 最终符合条件的状态
    std::set<int> char_set; // 输入符号集合


    int id = 0;
    std::set<State *> haveTravel;
    std::map<State *, int> state2id;
};

/*
 *   该类主要用来合并NFA, 合并完成后的NFA仍然为NFA, 只是不同的NFA链所对应的最终处理函数不同,
 * 可以注意到, 该类也进行了new的操作. 我们遵循"谁创建, 谁释放"的原则, 由该类创建的对象, 最后
 * 由自己进行统一的回收
 *
 */
class NFA2LIST: public NFA {
public:

    State *nfa_s = NULL;
    std::set<int> char_set;

    std::set<State*> st_create; // 统计新建的资源, 为了最终释放

    NFA2LIST() {}

    ~NFA2LIST() {
        for (auto state: st_create) {
            delete state;
        }
        st_create.clear();
    }

    /**
     * 合并操作, 主要用来合并NFA, 合并时, 新建一个新的起始节点, 再将两个NFA的字符集进行合并,
     */
    NFA2LIST* merge(Re2NFA *re2NFA) {
        /*
        if (nfa_s == NULL) {
            nfa_s = re2NFA->getStart();
            return this;
        }
        */
        State *create = new State(Split, re2NFA->getStart(), nfa_s);
        std::set<int> char_set1 = re2NFA->getCharSet();

        char_set.insert(char_set1.begin(), char_set1.end());
        st_create.insert(create);
        nfa_s = create;
        return this;
    }

    State* getStart() {
        return this->nfa_s;
    }

    std::set<int> getCharSet() {
        return this->char_set;
    }
};

#endif
