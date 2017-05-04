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

using std::cout;
using std::endl;
using std::string;

void yacc_test()
{
    //printf("hello yacc\n");

    string expr[8] = {
        "E  -> TE'",
        "E' -> +TE'",
        "E' -> ~",
        "T  -> FT'",
        "T' -> *FT'",
        "T' -> ~",
        "F  -> (E)",
        "F  -> i"
    };

    for (int i = 0; i < 8; ++i) {

        //std::string s = expr[i];
        expr[i].erase(std::remove_if(expr[i].begin(), expr[i].end(),
                                [](char c){return (c == '\r' || c == '\t' || c == ' ');}),
                                expr[i].end());
        std::cout << expr[i] << std::endl;
    }


    Grammar grammar;
    for (int i = 0; i < 8; ++i) {
      //  printf("%s\n", expr[i]);
        std::string s = expr[i];
        int j, len = s.size();
        for (j = 0; j < len; ++j) {
            if (s[j] == '-') break;
        }
        string left = s.substr(0, j);
        j += 2;
        string right = s.substr(j, len - j);

        /*
        Expression express(left);
        express.insert(right);
        express.insert(right);
        express.printExpr();
        exit(1);
        */
        grammar.insert(left, right);
        //cout << left << " -> " << right << endl;
    }
    grammar.printGram();
    grammar.makeFirst();

//    printf("%d\n", sizeof(expr));

    return;
}
