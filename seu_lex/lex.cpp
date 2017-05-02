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


#include "fstream"
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

    /*
    Re2NFA re2NFA;
    //char * str = re2NFA.re2post("a*(b|(cd?))+");
    //char *str = re2NFA.re2post("*");

    printf("%s\n", str);

    State * start = re2NFA.post2nfa(str);
    re2NFA.showNFA(start);

    N2DFA n2DFA;
    DState *ds = n2DFA.nfa2dfa(&re2NFA);
    n2DFA.showDFA(ds);
    */

    /*
     * (K|_)(K|_|L)*
     * (+|-|$)LL*(.LL*|$)((E|e)LL*|$)
     * "(\\+|-|$)LL*(\\.LL*|$)((E|e)LL*|$)";
     * |M|N
     * (*(=|$))|(/(=|$))|(+(+|=|$))|(-(-|=|$))|(<(<|=|$))|(>(>|=|$))|(=(=|$))|(&(&|=|$))|(|(||=|$))|(^(=|$))|(~(=|$))
     * (OI*O)|(PJ*P)
     * #include( )*((<(K|_)(K|_|L)*.h>)|("(K|_)(K|_|L)*.h"))


    //char *str =
    char *str = "\\(|\\)|\\{|\\}|\\[|\\]|;|,|.";
    //char *str = "\\(|\\)";
    Re2NFA *pRe2NFA = new Re2NFA(str);
    pRe2NFA->strToNFA();

    N2DFA n2DFA(pRe2NFA);
    n2DFA.nfa2dfa();
    n2DFA.printDFA();

    delete pRe2NFA;
     */

    Lex lex("../input/require.l", "../input/out.c");
    lex.scaner();
    lex.output();
}

