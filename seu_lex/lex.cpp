/*
 *=============================================================================
 *    Filename:lex.cpp
 *
 *     Version: 1.0
 *  Created on: April 25, 2017
 *
 *      Author: corvo
 *=============================================================================
 */


#include "lex.h"

int isDigit(char ch) {
    if (ch >= '0' && ch <= '9') {
        return 1;
    }
    return 0;
}

void test()
{
    printf("In lex test\n");
}
