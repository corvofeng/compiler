/*
 *=============================================================================
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

using std::string;

class Re2NFA {
public:
    Re2NFA() {
        strExpress = "";
        statusId = 0;
    }
    
    Re2NFA(string str) {
        strExpress = str;
    }
    
    void printNFA();
    
    void strToNFA();
    
   
private:
    string strExpress;
    int statusId;
};

#endif
