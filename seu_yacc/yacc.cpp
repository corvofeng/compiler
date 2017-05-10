/*
 *=============================================================================
 *    Filename:yacc.c
 *
 *     Version: 1.0
 *  Created on: May 03, 2017
 *
 *      Author: corvo
 *=============================================================================
 */


#include <stdio.h>
#include <string>
#include <string.h>
#include <algorithm>
#include <iostream>
#include "yacc.h"
#include "expression.h"
#include "grammar.h"
#include "lr1.h"

using std::cout;
using std::endl;
using std::string;

void yacc_test()
{
    /*
     *  "E  -> TE'",
     *  "E' -> +TE'",
     *  "E' -> ~",
     *  "T  -> FT'",
     *  "T' -> *FT'",
     *  "T' -> ~",
     *  "F  -> (E)",
     *  "F  -> i"
     *
     *  为了方便起见表达式中不处理含'的状态
     *
     * E' -> K
     * T' -> J
     */
    string expr[8] = {
        "E  -> TK",
        "K  -> +TK",
        "K  -> ~",
        "T  -> FJ",
        "J  -> *FJ",
        "J  -> ~",
        "F  -> (E)",
        "F  -> i"
    };



    string expr1[3] = {
        "S -> CC",
        "C -> cC",
        "C -> d"
    };

    string expr2[6] = {
        "S->S;A",
        "S->A",
        "A -> E",
        "A->i=E",
        "E->E+i",
        "E->i"
    };

    map<string, string> prior;
    prior.insert(std::make_pair("*", "+"));

    map<string, int> assoc;
    assoc.insert(std::make_pair("*", 1));
    assoc.insert(std::make_pair("+", 1));

    string expr3[4] = {
        "E -> E + E",
        "E -> E * E",
//        "E -> (E)",
        "E -> i"
    };

    LR1 lr1(expr3, 3, "E", prior, assoc);
    lr1.iterms();

    //lr1.printLR1();

    lr1.makeACTION();
    //lr1.makeGOTO();


    //Grammar grammar(expr, 8, "E");
    /*
    Grammar grammar(expr1, 3, "S");

    grammar.printGram();
    grammar.getNonTermHead();
    grammar.makeFirst();
    */

//    printf("%d\n", sizeof(expr));

    return;
}
