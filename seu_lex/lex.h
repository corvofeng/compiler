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
#include <vector>
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
    std::ostream *out = NULL;
    std::istream *in = NULL;

    std::vector<Re2NFA*> re2NFAList;
    N2DFA* pN2DFA = NULL;

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

    /*
    * delete生成的DFA以及NFA
    * 关闭打开的文件句柄
    * 释放stream对象
    */
    ~Lex() {
        for (auto iter = re2NFAList.begin(); iter != re2NFAList.end(); ++iter) {
            delete *iter;
        }
        re2NFAList.clear();
        if (pN2DFA != NULL) {
            delete pN2DFA;
        }

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

        Re2NFA *pre2Nfa = new Re2NFA(re_s, func);
        pre2Nfa->strToNFA();
        re2NFAList.push_back(pre2Nfa);
        /*
        N2DFA *pN2DFA = new N2DFA(pre2Nfa);
        pN2DFA->nfa2dfa();
        n2DFAList.push_back(pN2DFA);
        this->dfa2func.insert(std::make_pair(pN2DFA, func));
        */
    }

    /**
     * 程序当前状态:
     * 之前的程序中其实已经完成了由NFA向DFA的转换, 但是, 多个NFA有多种结束与初始状态, 如果分
     * 别将NFA转换为DFA, 依旧无法解决分析的问题, 因此, 此处需要现将NFA进行合并操作, 合并之后
     * 再转换为DFA.
     *
     * 问题:
     * 想法总是很好, 但随之而来的问题也并不简单, 多个NFA有多种结束状态, 每种结束状态对应的操
     * 作函数不尽相同, 因此, 我们虽然将NFA进行了合并, 但是, 每个NFA所携带的结束状态的信息却
     * 无法合并. 此时, 才是真正的尴尬, 如果无法解决这个问题, 程序是无法继续进行的.
     *
     * 解决方案:
     *   此时, 我向State与DState类中, 添加了endFunc变量, 总体来看, 变量的添加造成了多余空
     * 间的使用, 也使得程序并不优雅. 但此时, 主体框架已经定义, 不可能过多的修改NFA与DFA的创
     * 建类, 当状态过多时, 将会导致程序的内存占用率上升.
     *
     *   由于我追求了DFA与NFA尽可能的分离, 并且分别定义了State类以及DState类, 由此带来的问
     * 题只能通过使用多余空间进行解决. 此种方法有利有弊, 但不是权宜之计, 是解决之道.
     */
    void dfaMerge() {
        for (int i = 1; i < re2NFAList.size(); ++i) {
            Re2NFA* nfa = re2NFAList.at(i);
            nfa2List.merge(nfa);
        }
    }

    void nfa2DFA() {
        pN2DFA = new N2DFA(&nfa2List);
        pN2DFA->nfa2dfa();
        pN2DFA->printDFA();
    }

    NFA2LIST nfa2List;

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

    void outCodeTop() {
        std::ostream &o = (*out);
        o<<"#include <stdio.h>"<<endl;
        o<<"#include <stdlib.h>"<<endl;
        o<<"#include <string.h>"<<endl;
        o<<endl;
        o<<"#define SYLEX_MAXSIZE_TEXT 120"<<endl;
        o<<"#define SYLEX_MAXSIZE_BUFF 1024"<<endl;
        o<<endl;
        o<<"char SYLEX_FILE_NAME[100];"<<endl;
        o<<"char SYLEX_OUT_FILE_NAME[100];"<<endl;
        o<<"int SYLEX_LINE = 0;"<<endl;
        o<<"int SYLEX_STATE = 0;"<<endl;
        o<<"int SYLEX_TEXT_LEN = 0;"<<endl;
        o<<"char SYLEX_TEXT[SYLEX_MAXSIZE_TEXT];"<<endl;
        o<<"char SYLEX_BUFF[SYLEX_MAXSIZE_BUFF];"<<endl;
        o<<endl;
    }

    void outCodeMid() {
        std::ostream &o = (*out);

        cout<<"//扫描函数"<<endl;
        cout<<"void SYLEX_scanner(char *str)"<<endl;
        cout<<"{"<<endl;
        cout<<"    char ch = ' ';"<<endl;
        cout<<"    while(ch != '\\0')"<<endl;
        cout<<"    {"<<endl;
        cout<<"        //printf(\"%c %d\\n\", ch, SYLEX_STATE);"<<endl;
        cout<<"        switch(SYLEX_STATE) {"<<endl;

        int state = 0;
        for(auto dfaIter = dfa2func.begin(); dfaIter != dfa2func.end(); dfaIter++) {
            cout << " case " << state << ":" << endl;
            N2DFA *n2DFA = (*dfaIter).first;



        }
    }

    void outCodeBottom() {
        std::ostream &o = (*out);
        o<<"int main(int argc, char **args)"<<endl;
        o<<"{"<<endl;
        o<<"    if(argc == 1)"<<endl;
        o<<"    {"<<endl;
        o<<"        printf(\"没有输入源文件名\");"<<endl;
        o<<"        return 0;"<<endl;
        o<<"    }"<<endl;
        o<<"    else if(argc == 2)"<<endl;
        o<<"    {"<<endl;
        o<<"        strcpy(SYLEX_FILE_NAME, args[1]);"<<endl;
        o<<"        sprintf(SYLEX_OUT_FILE_NAME, \"%s.out\", SYLEX_FILE_NAME);"<<endl;
        o<<"    }"<<endl;
        o<<"    else"<<endl;
        o<<"    {"<<endl;
        o<<"        strcpy(SYLEX_FILE_NAME, args[1]);"<<endl;
        o<<"        strcpy(SYLEX_OUT_FILE_NAME, args[2]);"<<endl;
        o<<"    }"<<endl;
        o<<"    FILE* file = fopen(SYLEX_FILE_NAME, \"r\");"<<endl;
        o<<"    while(NULL != fgets(SYLEX_BUFF, SYLEX_MAXSIZE_BUFF, file))"<<endl;
        o<<"    {"<<endl;
        o<<"        ++SYLEX_LINE;"<<endl;
        o<<"        SYLEX_scanner(SYLEX_BUFF);"<<endl;
        o<<"    }"<<endl;
        o<<"    return 0;"<<endl;
        o<<"}"<<endl;
    }

    void output() {
        std::ostream &o = (*out);
        outCodeTop();
        outCodeMid();
        outCodeBottom();
        //o << "hello world\n";
        //o << "hello world\n";
    }

};

void test();

#endif /* ifndef LEX */
