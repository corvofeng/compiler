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
#include <string>
#include <string.h>
#include <stack>
#include <set>
#include <map>
#include "state.h"

using std::string;

/*
 * 每条正则语句对应于一条NFA
 *
 */
class Re2NFA {
public:
    Re2NFA() {
        strExpress = "";
        statusId = 0;
    }
    
    Re2NFA(string str) {
        strExpress = str;
    }

    ~Re2NFA() {
        // TODO 删除s
    }

    void printNFA();

    void strToNFA();

//private:
    string strExpress;
    int statusId;
    State *s;       // 保存解析后的数据

    /*
     * 由后缀表达式形式转化为NFA
     * 代码参考(仅参考) https://swtch.com/~rsc/regexp/
     *
     * 此函数中new了众多变量, 及时delete
     */
    State* post2nfa(char *postfix) {
        char *p;
        Frag stack[1000], *stackp, e1, e2, e;
        State *tmp, *start,*end;

#define push(s) *stackp++ = s
#define pop()   *--stackp

        stackp = stack;
        if (postfix == NULL) return NULL;

        for(p = postfix; *p; p++) {
            switch (*p) {
            case '.':
                e2 = pop();
                e1 = pop();

                if (e1.end->c == Split) {
                    e1.end->out1 = e2.start;
                } else {
                    e1.end->c = Split;
                    e1.end->out = e2.start;
                }
                push(Frag(e1.start, e2.end));
                break;
            case '?':
                e = pop();
                //start = new State(Split, e.start, NULL);
                e.start->out1 = e.end;
                push(Frag(e.start, e.end));
                break;
            case '|':
                e1 = pop();
                e2 = pop();

                start = new State(Split, e1.start, e2.start);
                end = new State(Match, NULL, NULL);
                e1.end->c = Split;
                e2.end->c = Split;

                e1.end->out = end;
                e2.end->out = end;
                push(Frag(start, end));
                break;
            case '*':
                e = pop();
                start = new State(Split, e.start, NULL);
                end = new State(Split, start, NULL);

                e.end->c = Split;
                e.end->out = e.start;
                e.end->out1 = end;

                push(Frag(start, end));

                break;
            default:
                end = new State(Match, NULL, NULL);
                start = new State(*p, end, NULL);

                push(Frag(start, end));
                break;
            }
        }
        e = pop();
        s = e.start;
        return s;
    }


    int id = 0;
    void showNFA(State *start) {
        //std::map<State*, int> state2id;
        //state2id.insert(std::make_pair(start, id));
        if (start == NULL) {
            return;
        }

        if (start->c < 256) {
            char p = start->c;
            printf("%d -> %c -> %d", id, p, ++id);
            showNFA(start->out);
            showNFA(start->out1);
        } else {
            char p = start->c;
            printf("%d -> [] -> %d", id, p, ++id);
            showNFA(start->out);
            showNFA(start->out1);
        }




    }

    /**
     * 此段代码来源:
     * https://swtch.com/~rsc/regexp/
     *
     *
     * Convert infix regexp re to postfix notation
     * Insert . as explicit concatenation operator.
     *
     * 中缀转化为后缀, 同时插入 . 作为连接符
     * 例如ab, 转化为后缀为ab.
     *
     * 字符具有高于运算符的优先级, "m|food"匹配"m"或是"food", 如果想要匹配
     * "mood"或是"food", 需要使用"(m|f)ood"进行匹配.
     *
     *   通过此函数的使用, 使我对与lex中的正则匹配的机制有了更深的理解, 希望大家也能通过此函数
     * 的使用将正则表达式转换为后缀形式
     *
     */
    char * re2post(char *re) {

        int nalt, natom; 
        static char buf[8000]; 
        char *dst; 
        struct { 
            int nalt; 
            int natom; 
        } paren[100], *p; 

        p = paren; 
        dst = buf; 
        nalt = 0; 
        natom = 0; 
        if(strlen(re) >= sizeof buf/2) 
            return NULL; 
        for(; *re; re++){ 
            switch(*re){ 
                case '(': 
                    if(natom > 1){ 
                        --natom; 
                        *dst++ = '.'; 
                    } 
                    if(p >= paren+100) 
                        return NULL; 
                    p->nalt = nalt; 
                    p->natom = natom; 
                    p++; 
                    nalt = 0; 
                    natom = 0; 
                    break; 
                case '|': 
                    if(natom == 0) 
                        return NULL; 
                    while(--natom > 0) 
                        *dst++ = '.'; 
                    nalt++; 
                    break; 
                case ')': 
                    if(p == paren) 
                        return NULL; 
                    if(natom == 0) 
                        return NULL; 
                    while(--natom > 0) 
                        *dst++ = '.'; 
                    for(; nalt > 0; nalt--) 
                        *dst++ = '|'; 
                    --p; 
                    nalt = p->nalt; 
                    natom = p->natom; 
                    natom++; 
                    break; 
                case '*': 
                case '+': 
                case '?': 
                    if(natom == 0) 
                        return NULL; 
                    *dst++ = *re; 
                    break; 
                default:
                    if(natom > 1){ 
                        --natom; 
                        *dst++ = '.'; 
                    } 
                    *dst++ = *re; 
                    natom++; 
                    break;
            } 
        } 
        if(p != paren) 
            return NULL; 
        while(--natom > 0) 
            *dst++ = '.'; 
        for(; nalt > 0; nalt--) 
            *dst++ = '|'; 
        *dst = 0; 
        return buf; 
    }
};

#endif
