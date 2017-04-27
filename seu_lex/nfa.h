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
#include "state.h"

using std::string;

class Re2NFA {
public:
    Re2NFA() {
        strExpress = "";
        statusId = 0;
    }
    
    Re2NFA(string str) {
        strExpress = str;
    }

    void printNFA();

    void strToNFA();

//private:
    string strExpress;
    int statusId;

    State* post2nfa(char *postfix) {

    }

    /**
     * 主要代码来源:
     * https://swtch.com/~rsc/regexp/
     *
     *
     * Convert infix regexp re to postfix notation
     * Insert . as explicit concatenation operator.
     *
     * 中缀转化为后缀, 同时插入 . 作为连接符
     * 例如ab, 转化为后缀为ab.
     *
     * 另外, 原有算法是有问题的, 当我们输入为
     * b|cd?时, 应该转换为的结果是'bc|d?.' 也就是说, b|c的优先级比价高
     *
     * 但是原有算法中会转换为'bcd?.|' 曲解输入为b|(cd?), 这是不合适的. 故而有所改正
     *
     *
     */
    static char * re2post(char *re) {

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
                default:        // 此处与原文代码不同
                    if(natom > 1){ 
                        --natom; 
                        *dst++ = '.'; 
                    } 
                    *dst++ = *re; 
                    natom++; 
                    if(nalt >= 1) {
                        --nalt;
                        *dst ++ = '|';
                    }
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
