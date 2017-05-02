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
#include <iostream>
#include <stdio.h>
#include "nfa.h"
#include "dfa.h"
#include "state.h"
#include <fstream>
#include <sstream>
#include <algorithm>

using std::string;
using std::cout;
using std::endl;
using std::cerr;

class Lex
{

public:
   static const int MAXID = 100000;
   string lexFile;
   std::ostream *out;
   std::istream *in;

   std::list<Re2NFA*> re2NFAList;
   std::list<N2DFA*> n2DFAList;


   Lex (string _lexFile) {
       in = new std::ifstream(_lexFile);
       this->lexFile = _lexFile;
       this->out = &std::cout;
   }

   Lex (string _lexFile, string _outCFile) {
       in = new std::ifstream(_lexFile);
       this->lexFile = _lexFile;
       std::ofstream *fout = new std::ofstream(_outCFile);
       this->out = fout;
   }

   ~Lex() {

       for (auto iter = re2NFAList.begin(); iter != re2NFAList.end(); ++iter) {
           delete *iter;
       }
       re2NFAList.clear();

       for (auto iter = n2DFAList.begin(); iter != n2DFAList.end(); ++iter) {
           delete *iter;
       }
       n2DFAList.clear();

       ((std::ifstream*)in)->close();
       delete this->in;

       if (this->out == &std::cout)
           return;
       ((std::ofstream*)out)->close();
       delete this->out;
   }

    void init();

    void scaner() {
        int state = 0;
        int line = 0;
        string str;
        string outStr;
        std::istream& input = *in;
        std::ostream& output = *out;

        while(!input.eof())
        {
            line ++;
            switch (state) {
            case 0: {
                getline(input, str);
                if (str.compare(0, 2, "%{") == 0) {
                    state = 1;
                    output << "//%{ start" << endl;
                } else if (str.compare(0, 2, "%!") == 0) {
                    state = 2;
                    output << "//%! start" << endl;
                } else if (str.compare(0, 2, "%%") == 0) {
                    state = 3;
                    outStr.clear();
                    output << "//%% start" << endl;
                }else if (str.compare(0, 2, "//") == 0) {
                        state = 4;
                } else {
                    state = 0;
                    if (!str.empty()) {
                        printError(line, str + "error");
                    }
                }
                break;
            }
            case 1: {
                getline(input, str);
                if (str.compare(0, 2, "%}") == 0) {
                    state = 0;
                    output << "//%} end" << endl;
                } else {
                    output << str << endl;
                }
                break;
            }
            case 2: {
                getline(input, str);
                if (str.compare(0, 2, "%!") == 0) {
                    output << "//%! end" << endl;
                    state = 0;
                } else {
                    getFunc(str, line);
                }
                break;
            }
            case 3: {
                getline(input, str);
                if (str.compare(0, 2, "%%") == 0) {
                    state = 0;
                    getRegular(outStr, line);
                    output << "//%% end" << endl;
                } else if (str.compare(0, 2, "%$") == 0) {
                    getRegular(outStr, line);
                    outStr.clear();
                } else
                    outStr += str;
                break;
            }
            case 4: {
                // 此处获得的是注释
                state = 0;
                break;
            }
            default:
                printError(line, "结构不完整");
                break;
            }
        }
        if (state != 0) {
            printError(line, "结构不完整");
        }
    }

    // 记录自定义的类型的判断函数, digit->isDigit
    std::map<string, string> funcMap;

    // 记录自定义的类型的符号, 以便进行转换 digit->a
    std::map<string, char> type2ch;

    void getFunc(string str, int line) {

        /*
         * 清除等式中的空格,
         * 代码来源 :
         * http://stackoverflow.com/questions/83439/remove-spaces-from-stdstring-in-c
         */
        string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
        str.erase(end_pos, str.end());

        // TODO为了避免重复, 此处需要修改为不存在的char值
        static char ch = 'I';

        string::iterator iter = str.begin();
        string left, right;
        std::istringstream f(str);

        getline(f, left, '=');
        getline(f, right, '=');


        funcMap.insert(std::make_pair(left, right));
        type2ch.insert(std::make_pair(left, ch));

        cout << ch << " <-->" << left << "<-->"  << right  << endl;
        ch ++;
    }

    std::map<N2DFA*, string> dfa2func;

    void getRegular(string str, int line) {
        std::pair<string, string> record = getReAndFunc(str);
        string re = record.first;
        string func = record.second;
        //cout << re << endl;

        char re_s[1024] = {0};
        int it = 0;

        for (int i = 0; i < re.size(); ++i) {
            char p = re.at(i);
            switch (p) {
            case '%': {
                i++;
                re_s[it++] = '\\';
                re_s[it++] = re.at(i);
                break;
            }
            case '{': {
                i++;
                int start = i;
                while (re.at(i) != '}') {i++;}

                string atom = re.substr(start, i - start);
//                cout << start << " - " << i - 1 << "The atom is " << atom << endl;
                if (type2ch.find(atom) != type2ch.end()) {
                    re_s[it++] = type2ch.at(atom);
                }  else {
                    printError(line, "error find " + atom);
                }
                break;
            }
            default:
                re_s[it++] = re.at(i);
                break;
            }
        }
        //cout << re_s << endl;

        Re2NFA *pre2Nfa = new Re2NFA(re_s);
        pre2Nfa->strToNFA();

        N2DFA *pN2DFA = new N2DFA(pre2Nfa);
        pN2DFA->nfa2dfa();

        re2NFAList.push_back(pre2Nfa);
        n2DFAList.push_back(pN2DFA);
        this->dfa2func.insert(std::make_pair(pN2DFA, func));
    }

    std::pair<string, string> getReAndFunc(string str) {
        string re;
        string func;

        string::iterator iter = str.end();
        int stack = 1;
        while(*iter != '}') {
            iter --;
        }
        func.push_back('}');
        iter --;
        while (stack >= 1 && iter != str.begin()) {
            if (*iter == '}') {
                stack ++;
            } else if (*iter == '{') {
                stack --;
            }
            func.push_back(*iter);
            iter --;
        }

        while(*iter == ' ') {
            iter --;
        }

        re.push_back(*iter);
        while(iter != str.begin()) {
            iter --;
            re.push_back(*iter);
        }

        std::reverse(func.begin(), func.end());
        std::reverse(re.begin(), re.end());

    //    cout << re << "<---->" << func << endl << endl;

        return std::make_pair(re, func);
    }



    void printWarnning(int line, string str) {
        cout << "[warning]:  in line " << line << " " << str << endl;
    }

    void printError(int line, string str) {
        cout << "[error]:  in line " << line <<  " " << str << endl;
        exit(1);
    }

    void output() {
        std::ostream &o = (*out);
        o << "hello world\n";
        o << "hello world\n";
    }

};

void test();

#endif /* ifndef LEX */
