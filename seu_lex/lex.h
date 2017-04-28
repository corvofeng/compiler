/*
 *=============================================================================
 *    Filename:lex.h
 *
 *     Version: 1.0
 *  Created on: April 25, 2017
 *
 *      Author: corvo
 *=============================================================================
 */

#ifndef LEX_H_
#define LEX_H_

#include <string>
#include <stdio.h>
#include "nfa.h"
#include "state.h"

using std::string;

class Lex
{

public:
   static const int MAXID = 100000;

   Lex ( string _lexFile );
   Lex ( string _lexFile, string _outCFile );


   void init();


};

void test();

#endif /* ifndef LEX */
