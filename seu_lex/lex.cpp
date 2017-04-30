/*
 * =============================================================================
 *    Filename:lex.cpp
 *
 *     Version: 1.0
 *  Created on: April 25, 2017
 *
 *      Author: corvo
 *=============================================================================
 */


#include "lex.h"
#include "nfa.h"
#include "dfa.h"

int isDigit(char ch)
{
    if(ch >= '0' && ch <= '9')
    {
        return 1;
    }

    return 0;
}

void test()
{
    Re2NFA re2NFA;
    char * str = re2NFA.re2post("a*(b|(cd?))+");
    //char *str = re2NFA.re2post("a?");

    printf("%s\n", str);

    State * start = re2NFA.post2nfa(str);
    re2NFA.showNFA(start);

    N2DFA n2DFA;
    DState *ds = n2DFA.nfa2dfa(&re2NFA);
    n2DFA.showDFA(ds);



    /*
     *    str = Re2NFA::re2post("ab*(a*|(ab)*)");
     *    printf("%s\n", str);
     *
     *    str = Re2NFA::re2post("b|cd?");
     *    printf("%s\n", str);
     */
}

