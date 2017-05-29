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
#define LINK 01


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

    if(strlen(re) >= sizeof(buf) / 2)
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
            end = new State(Match, NULL, NULL);
            start = new State(Split, e.start, end);
            if (e.end->c == Split) {
                e.end->out1 = end;
            } else {
                e.end->c = Split;
                e.end->out = end;
            }

            push(Frag(start, end));
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

        case '\\': {
            p++;
            end = new State(Match, NULL, NULL);
            start = new State(*p, end, NULL);
            char_set.insert(*p);
            push(Frag(start, end));
            break;
        }

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
            if (start->out->c == Match) {
                printf("%d -> [] -> [%d]\n", state2id.at(start), state2id.at(start->out));
            } else {
                printf("%d -> [] -> %d\n", state2id.at(start), state2id.at(start->out));
            }
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
