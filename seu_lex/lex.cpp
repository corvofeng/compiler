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
    char *s1 = "abb";
    char *s2 = "a";
    char *s3 = "a*b+";

    Re2NFA re2NFA1(s1, "first");
    Re2NFA re2NFA2(s2, "second");
    Re2NFA re2NFA3(s3, "third");

    re2NFA1.strToNFA();
    re2NFA2.strToNFA();
    re2NFA3.strToNFA();

    NFA2LIST nfaList;
    nfaList.merge(&re2NFA1);
    nfaList.merge(&re2NFA2);
    nfaList.merge(&re2NFA3);


    N2DFA n2Dfa(&nfaList);

    n2Dfa.nfa2dfa();
    n2Dfa.printDFA();

    std::cout << "print over" << std::endl;
    */



    /*
     * (K|_)(K|_|L)*
     * (+|-|$)LL*(.LL*|$)((E|e)LL*|$)
     * "(\\+|-|$)LL*(\\.LL*|$)((E|e)LL*|$)";
     * |M|N
     * (*(=|$))|(/(=|$))|(+(+|=|$))|(-(-|=|$))|(<(<|=|$))|(>(>|=|$))|(=(=|$))|(&(&|=|$))|(|(||=|$))|(^(=|$))|(~(=|$))
     * (\*(=|\$))|(/(=|\$))|(\+(\+|=|\$))|(-(-|=|\$))|(<(<|=|\$))|(>(>|=|\$))|(=(=|\$))|(&(&|=|\$))|(\|(\||=|\$))|(^(=|\$))|(~(=|\$))
     * (OI*O)|(PJ*P)
     * #include( )*((<(K|_)(K|_|L)*.h>)|("(K|_)(K|_|L)*.h"))

     */

    /*
    //char *str =
    char *str = "\\+(\\+|=)?";
    //char *str = "(\\+|=)?";
    //char *str = "\\+|=";
    //char *str = "\\(|\\)";
    Re2NFA *pRe2NFA = new Re2NFA(str);
    cout << pRe2NFA->re << endl;
    pRe2NFA->strToNFA();
    pRe2NFA->printNFA();
    cout << pRe2NFA->buf << endl;

    N2DFA n2DFA(pRe2NFA);
    n2DFA.nfa2dfa();
    n2DFA.printDFA();

    delete pRe2NFA;
    */

    Lex lex("../input/require.l", "../input/out.c");
    lex.scaner();
    lex.dfaMerge();
    lex.nfa2DFA();
    lex.output();
}

