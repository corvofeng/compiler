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
class Re2NFA
{
public:
    Re2NFA()
    {
        strExpress = "";
        statusId = 0;
    }

    Re2NFA(string str)
    {
        strExpress = str;
    }

    ~Re2NFA()
    {
        // TODO 删除s
        free(nfa_s);
        nfa_s = NULL;
        nfa_e = NULL;
    }

    std::set<State*> hasDel; // 析构时的辅助集合, 保存已经删除的节点, 阻止重复删除

    /**
     * 析构对象时, 需要考虑链表的递归引用, 一个对象不可以重复删除
     */
    void free(State *start) {
        if (start == NULL ||
            hasDel.find(start) == hasDel.end() ) {
            return;
        }

        hasDel.insert(start);
        free(start->out);
        free(start->out1);

        delete start;
    }

    void printNFA();

    void strToNFA();

    //private:
    string strExpress;
    int statusId;
    State * nfa_s;      // 保存解析后的数据
    State * nfa_e;		// 最终符合条件的状态
    std::set<int> char_set; // 输入符号集合

    /*
     * 由后缀表达式形式转化为NFA
     * 代码参考(仅参考) https://swtch.com/~rsc/regexp/
     *
     * 此函数中new了众多变量, 及时delete
     */
    State * post2nfa(char * postfix)
    {
        char * p;
        Frag stack[1000], *stackp, e1, e2, e;
        State * start, *end;

#define push(s) *stackp++ = s
#define pop()   *--stackp

        stackp = stack;

        if(postfix == NULL)
        {
            return NULL;
        }

        for(p = postfix; *p; p++)
        {
            switch(*p)
            {
                case '.':
                    e2 = pop();
                    e1 = pop();

                    if(e1.end->c == Split)
                    {
                        e1.end->out1 = e2.start;
                    }
                    else
                    {
                        e1.end->c = Split;
                        e1.end->out = e2.start;
                    }

                    push(Frag(e1.start, e2.end));
                    break;

                case '?':
                    e = pop();
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
                    end = new State(Match, NULL, NULL);
                    start = new State(Split, e.start, end);
                    e.end->c = Split;
                    e.end->out = e.start;
                    e.end->out1 = end;
                    push(Frag(start, end));
                    break;

                case '+':
                    e = pop();
                    start = new State(Split, e.start, NULL);
                    end = new State(Match, NULL, NULL);
                    e.end->c = Split;
                    e.end->out = e.start;
                    e.end->out1 = end;
                    push(Frag(start, end));
                    break;

                default:
                    end = new State(Match, NULL, NULL);
                    start = new State(*p, end, NULL);
                    char_set.insert(*p);
                    push(Frag(start, end));
                    break;
            }
        }

        e = pop();

        if(stackp != stack)      // 如果栈中仍然存在其余元素, 则不匹配
        {
            printf("error occuerd\n");
        }


        if(e.end->c == Match)
        {
            //e.end->out1 = end;
            //printf ( "already matched\n" );
        }
        else
        {
            end = new State(Match, NULL, NULL);
            e.end->out1 = end;
            e.end = end;
        }

        nfa_s = e.start;
        nfa_e = e.end;

        return nfa_s;
    }

    int id = 0;
    std::set<State *> haveTravel;
    std::map<State *, int> state2id;

    /**
     * 打印NFA
     */
    void showNFA(State * start)
    {
        if(start == NULL ||
                start->c == Match ||
                haveTravel.find(start) != haveTravel.end())
        {
            return;
        }

        if(state2id.find(start) == state2id.end())
        {
            state2id.insert(std::make_pair(start, id));
            id ++;
        }

        haveTravel.insert(start);

        if(start->out != NULL)
        {
            if (haveTravel.find(start->out) == haveTravel.end()) {
                state2id.insert(std::make_pair(start->out, id));
                id++;
            }
            if (start->c < 256) {
                printf("%d -> %c -> %d\n",
                   state2id.at(start), start->c, state2id.at(start->out));
            } else {
                printf("%d -> [] -> %d\n", state2id.at(start), state2id.at(start->out));
            }
            showNFA(start->out);
        }

        if(start->out1 != NULL)
        {
            if (haveTravel.find(start->out1) == haveTravel.end()) {
                state2id.insert(std::make_pair(start->out1, id));
                id++;
            }
            //if (start->c < 256) {
            //    printf("%d -> %c -> %d\n",
            //       state2id.at(start), start->c, state2id.at(start->out1));
            //} else {
                printf("%d -> [] -> %d\n", state2id.at(start), state2id.at(start->out1));
            //}
            showNFA(start->out1);
        }
        // char p;

        /*
        if(start->c < 256)
        {
            printf("start->c is %d\n", start->c);
            p = start->c;
            printf("%d -> %c -> %d\n", id, p, id + 1);
            id ++;
            showNFA(start->out);
            showNFA(start->out1);
        }
        else
        {
            printf("%d -> [] -> %d\n", id, id + 1);
            id ++;
            showNFA(start->out);
            showNFA(start->out1);
        }
        */
    }

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
     * 的使用将正则表达式转换为后缀形式
     *
     */
    char * re2post(char * re)
    {

        int nalt, natom;
        static char buf[8000];
        char * dst;
        struct
        {
            int nalt;
            int natom;
        } paren[100], *p;

        p = paren;
        dst = buf;
        nalt = 0;
        natom = 0;

        if(strlen(re) >= sizeof buf / 2)
        {
            return NULL;
        }

        for(; *re; re++)
        {
            switch(*re)
            {
                case '(':
                    if(natom > 1)
                    {
                        --natom;
                        *dst++ = '.';
                    }

                    if(p >= paren + 100)
                    {
                        return NULL;
                    }

                    p->nalt = nalt;
                    p->natom = natom;
                    p++;
                    nalt = 0;
                    natom = 0;
                    break;

                case '|':
                    if(natom == 0)
                    {
                        return NULL;
                    }

                    while(--natom > 0)
                    {
                        *dst++ = '.';
                    }

                    nalt++;
                    break;

                case ')':
                    if(p == paren)
                    {
                        return NULL;
                    }

                    if(natom == 0)
                    {
                        return NULL;
                    }

                    while(--natom > 0)
                    {
                        *dst++ = '.';
                    }

                    for(; nalt > 0; nalt--)
                    {
                        *dst++ = '|';
                    }

                    --p;
                    nalt = p->nalt;
                    natom = p->natom;
                    natom++;
                    break;

                case '*':
                case '+':
                case '?':
                    if(natom == 0)
                    {
                        return NULL;
                    }

                    *dst++ = *re;
                    break;

                default:
                    if(natom > 1)
                    {
                        --natom;
                        *dst++ = '.';
                    }

                    *dst++ = *re;
                    natom++;
                    break;
            }
        }

        if(p != paren)
        {
            return NULL;
        }

        while(--natom > 0)
        {
            *dst++ = '.';
        }

        for(; nalt > 0; nalt--)
        {
            *dst++ = '|';
        }

        *dst = 0;
        return buf;
    }
};

#endif
