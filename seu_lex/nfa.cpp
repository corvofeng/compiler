/*
 *=============================================================================
 *    Filename:nfa.cpp
 *
 *     Version: 1.0
 *  Created on: April 25, 2017
 *
 *      Author: corvo
 *=============================================================================
 */


#include "nfa.h"

/*
 *   连接符, 对于两个Frag片段, 如果有连接符存在, 则进行连接操作
 * 对于正则表达式来说, 需要选择一个不会被用到的字符
 */
#define LINK 27

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
char *Re2NFA::re2post(char *re)
{
    int nalt, natom;
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
                    *dst++ = LINK;
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
                    *dst++ = LINK;
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
                    *dst++ = LINK;
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

            case '\\' : {
                if(natom > 1)
                {
                    --natom;
                    *dst++ = LINK;
                }
                *dst++ = *re;
                re ++;
                *dst++ = *re;
                natom++;
                break;
            }

            default:
                if(natom > 1)
                {
                    --natom;
                    *dst++ = LINK;
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
        *dst++ = LINK;
    }

    for(; nalt > 0; nalt--)
    {
        *dst++ = '|';
    }

    *dst = 0;
    return buf;
}

/*
 * 由后缀表达式形式转化为NFA
 * 代码参考(仅参考) https://swtch.com/~rsc/regexp/
 *
 * 此函数中new了众多变量, 及时delete
 */
State *Re2NFA::post2nfa(char *postfix)
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
        case LINK:
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

        case '\\':
            *p ++;
            end = new State(Match, NULL, NULL);
            start = new State(*p, end, NULL);
            char_set.insert(*p);
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
        this->showNFA(e.start);
        exit(1);
    }


    if(e.end->c == Match)
    {
        //e.end->out1 = end;
        //printf ( "already matched\n" );
        e.end->endFunc = this->func;
    }
    else
    {
        end = new State(Match, NULL, NULL);
        end->endFunc = this->func;
        e.end->out1 = end;
        e.end = end;
    }

    nfa_s = e.start;
    nfa_e = e.end;

    return nfa_s;

}

/**
 * 打印NFA
 */
void Re2NFA::showNFA(State *start)
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
