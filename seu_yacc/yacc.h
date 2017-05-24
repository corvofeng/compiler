/*
 *=============================================================================
 *    Filename:yacc.h
 *
 *     Version: 1.0
 *  Created on: May 03, 2017
 *
 *      Author: corvo
 *=============================================================================
 */


#ifndef YACC_H_
#define YACC_H_

#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <stack>
#include "lr1.h"


using std::string;
using std::stringstream;
using std::ifstream;
using std::map;
using std::vector;
using std::stack;
using std::endl;
using std::cout;

void yacc_test();

class Yacc
{
public:
    static const int MAXID = 100000;
    string yaccFile;
    std::ostream *out = NULL;
    std::istream *in = NULL;
    LR1* lr1 = NULL;

    Yacc (string _yaccFile) {
        in = new std::ifstream(_yaccFile);
        this->yaccFile = _yaccFile;
        this->out = &std::cout;
    }

    Yacc (string _yaccFile, string _outCFile) {
        in = new std::ifstream(_yaccFile);
        this->yaccFile = _yaccFile;
        std::ofstream *fout = new std::ofstream(_outCFile);

        this->out = fout;
    }

    /*
     * delete生成的DFA以及NFA
     * 关闭打开的文件句柄
     * 释放stream对象
     */
    ~Yacc() {

        ((std::ifstream*)in)->close();
        delete this->in;

        if (this->out == &std::cout)
            return;
        ((std::ofstream*)out)->close();
        delete this->out;
    }

    void scanner() {
        printf("In Yacc Scanner\n");

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
            case 0:
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
                    getDeclare(line, str);
                }
                break;
            }
            case 3: {
                getline(input, str);
                if (str.compare(0, 2, "%%") == 0) {
                    state = 0;
                    getGrammar(line, outStr);
                    output << "//%% end" << endl;
                } else if (str.compare(0, 2, "%$") == 0) {
                    getGrammar(line, outStr);
                    outStr.clear();
                } else
                    outStr += str;
                break;
            }
            default:
                printError(line, "结构不完整");
                break;
            }
        }
    }

    void buildTable() {

        for (auto it: expr2func) {
            cout << it.first << ": " << it.second << endl;
        }
        lr1 = new LR1(expr2func, "A", prior, assoc);

        lr1->iterms();

        lr1->makeACTIONGOTO();
        lr1->printACTIONGOTO();

    }

    /*
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

    }

    void outCodeBottom() {

    }
    */

    map<string, int> num2num;

    void parse(string filename) {
        ifstream fin(filename);
        std::istream& input = fin;
        int line = 0;
        string str;

        map<string, int>& actionTerm = this->lr1->actionTerm;
        map<string, int>& gotoNonTerm = this->lr1->gotoNonTerm;
        vector<vector<string>>& res_action = this->lr1->res_action;
        vector<vector<int>>& res_goto = this->lr1->res_goto;
        LRState* standardState = this->lr1->standardState;
        int state = 0;

        vector<char> symbolStack;
        vector<int> stateStack;
        vector<std::pair<string, int>> inputLex;

        stateStack.push_back(state);
        while(!input.eof()) {
            line ++;
            getline(input, str);
            //cout << "In " << line << ": " << str << endl;
            //cout << "Current state is " << state << endl;
            if (!str.empty()) {
                std::pair<string, int> s = lexSplit(str);
                inputLex.push_back(s);
            }

        }
        inputLex.push_back(std::make_pair("$", 0));

        for (auto lexItem = inputLex.begin(); lexItem != inputLex.end(); lexItem++) {

            std::pair<string, int>& s = *lexItem;

            cout << "Current state is " << state << endl;
            string term;
            if (token2ch.find(s.first) != token2ch.end()) {
                term = token2ch[s.first];
            } else {
                term = s.first;
            }

            int i = actionTerm[term];
            string action = res_action[state][i];

            //cout << "--" << state << "-----" << term <<  "--The action is " << action << endl;
            if (action[0] == 's') {
                symbolStack.push_back(term[0]);
                state = action[1] - 48;
                stateStack.push_back(state);
                //cout << "The shift action is " << action << " " << state << endl;

            } else if (action[0] == 'r') { // reduce

                int reduceNum = action[1] - 48;
                SingleExpress* expr = standardState->singleExprVec.at(reduceNum);
                string right = expr->right;
                std::reverse(right.begin(), right.end());

                expr->printSigleExpr();
                //cout << "The reduce action is " << action << " " << state << endl;
                cout << "current is" << expr->func << endl;
                for(auto it : right) {
                    stackPrint(symbolStack);
                    stackPrint(stateStack);

                    auto symbolIt = symbolStack.end();
                    auto stateIt = stateStack.end();
                    symbolIt--;

                    char s = *symbolIt;
                    cout << "Stack top is " << s << endl;
                    if (it == s) {
                        cout << "ok" << endl;
                        symbolStack.erase(symbolIt);
                        stateIt --;
                        stateStack.erase(stateIt);
                        stateIt --;
                        state = *stateIt;

                    } else {
                        cout << "Error in stack" << endl;
                        exit(-1);
                    }
                }

                int i = gotoNonTerm[expr->left];
                //cout << "i = " << i << "old state is " << state << endl;
                state = res_goto[state][i];
                //cout << "refresh state to " << tmp << endl;
                stateStack.push_back(state);
                symbolStack.push_back(expr->left[0]);
                lexItem --;
            } else if (action == "acc") {
                cout << "Accept" << endl;
                break;
            }


        }


    }

    void stackPrint(vector<char> &stack) {
        cout << "-------stack-----" << endl;
        for (auto it: stack) {
            cout << it << " ";
        }
        cout << endl;
        cout << "-------stack-----" << endl;
    }

    void stackPrint(vector<int> &stack) {
        cout << "-------stack-----" << endl;
        for (auto it: stack) {
            cout << it << " ";
        }
        cout << endl;
        cout << "-------stack-----" << endl;
    }




    std::pair<string, int> lexSplit(string str) {

        int start = 1;
        int pos = 0;
        int len = str.length() - 1;

        string key;
        string value;
        while(pos != len) {
            if (str[pos] == ',') {
                key = str.substr(start, pos - start);
                break;
            }
            pos ++;
        }

        value = str.substr(pos + 1, len - pos - 1);

        int retValue;
        if (key == "$NUM") {
            key = key.substr(1);
            retValue = std::stoi( value );
        } else {
            retValue = 0;
        }
        cout << key << "---" << retValue << endl;

        return std::make_pair(key, retValue);
    }



    map<string, string> expr2func;   // 保存表达式所对应的函数操作
    map<string, char> token2ch;
    map<string, char> gram2ch;

    map<string, string> prior;   // 记录优先级
    map<string, int> assoc;      // 记录结合律

    void getDeclare(int line, string declare) {

        static char ch = 'a';
        if (declare.find("%token") != string::npos) {
            stringstream input(declare);
                //依次输出到result中，并存入res中
            string token;
            input >> token;
            while(input>>token) {
                cout << "The token is " << token << endl;
                token2ch.insert(std::make_pair(token, ch));
                ch ++;
            }
            cout << "ok" << endl;
        } else if (declare.find("%left") != string::npos) {
            stringstream input(declare);
            string tmp;
            input >> tmp;
            string r, l;
            input >> l;
            input >> r;
            cout << "l:" << l << " r:" << r << endl;
            assoc.insert(std::make_pair(l, 1));
            assoc.insert(std::make_pair(r, 1));

            prior.insert(std::make_pair(l, r));
        } else if (declare.find("%right") != string::npos) {
            stringstream input(declare);
            string tmp;
            input >> tmp;
            string r;
            input >> r;
            assoc.insert(std::make_pair(r, 2));
        }else if (declare.find("%head") != string::npos) {
            stringstream input(declare);
            string tmp;
            input >> tmp;
            string head;
            input >> head;
            if (gram2ch.find(head) == gram2ch.end()) {
                gram2ch.insert(std::make_pair(head, 'A'));
                ch ++;
            }
        }
    }


    void getGrammar(int line, string grammar) {
        static char ch = 'B';

        // 清除空格
        string::iterator end_pos = std::remove(grammar.begin(), grammar.end(), ' ');
        grammar.erase(end_pos, grammar.end());

        cout << grammar << endl;

        vector<string> exprVec = split(grammar, ":|");
        string head = exprVec[0];
        if (gram2ch.find(head) == gram2ch.end()) {
            gram2ch.insert(std::make_pair(head, ch));
            ch ++;
        }

        for(int i = 1; i < exprVec.size(); i++) {
            std::pair<string, string> exprfunc = makeSplitExpr(head, exprVec[i], ch);

            string s;
            s += gram2ch[head];
            s = s + "->" + exprfunc.first;
            expr2func.insert(std::make_pair(s, exprfunc.second));
            cout << "Expr is " << exprfunc.first << endl;
            cout << "Func is " << exprfunc.second << endl;
        }
    }

    std::pair<string, string> makeSplitExpr(string& head, string& expr, char& ch) {

        //cout << "origin" << expr << endl;
        std::pair<string, string> expr_func = getExprAndFunc(expr);

        //cout << "first: " << expr_func.first << endl;
        //cout << "second: " << expr_func.second << endl;

        string retExpr;
        string pureExpr = expr_func.first;
        int len = expr_func.first.length();
        int pos = 1;
        int start = 1;

        while(pos < len) {

            if (pureExpr[pos] == '\'') {
                if (pos == start) {
                    pos += 1;
                    retExpr.push_back(pureExpr[pos]);
                    pos += 2;
                    start = pos;
                } else {
                    string tmp = pureExpr.substr(start, pos - start);
//                    cout << "Tmp is " << tmp << endl;

                    if (gram2ch.find(tmp) == gram2ch.end()) {  // 如果不属于已知的非终结符
                        if (token2ch.find(tmp) != token2ch.end()) { // 属于已知的终结符
                            retExpr.push_back(token2ch[tmp]);
                        } else {            // 该符号并不已知, 将其作为新的非终结符
                            gram2ch.insert(std::make_pair(tmp, ch));
                            ch++;
                            retExpr.push_back(gram2ch[tmp]);
                        }
                    } else {    // 该非终结符已知
                        retExpr.push_back(gram2ch[tmp]);
                    }

                    pos += 1;
                    retExpr.push_back(pureExpr[pos]);
                    pos += 2;
                    start = pos;
                }
            }
            pos += 1;
        }

        if (start + 1 != pos) {
            //cout << "There is exists" << start << pos << pureExpr.substr(start, pos-start+1) << endl;
            string tmp = pureExpr.substr(start, pos -start +1);

            if (gram2ch.find(tmp) == gram2ch.end()) {  // 如果不属于已知的非终结符
                if (token2ch.find(tmp) != token2ch.end()) { // 属于已知的终结符
                    retExpr.push_back(token2ch[tmp]);
                } else {            // 该符号并不已知, 将其作为新的非终结符
                    gram2ch.insert(std::make_pair(tmp, ch));
                    ch++;
                    retExpr.push_back(gram2ch[tmp]);
                }
            } else {    // 该非终结符已知
                retExpr.push_back(gram2ch[tmp]);
            }
        }

        return std::make_pair(retExpr, expr_func.second);
    }

    std::pair<string, string> getExprAndFunc(string str) {
        string re;
        string func;

        string::iterator iter = str.end();
        int stack = 1;
        while(*iter != '}' && iter != str.begin()) {
            iter --;
        }

        if (iter == str.begin()) {
            return std::make_pair(str, "");
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

        return std::make_pair(re, func);
    }

    std::vector<std::string> split(const  std::string& s, const std::string& delim)
    {
        std::vector<std::string> elems;
        size_t pos = 0;
        size_t len = s.length();
        size_t delim_len = delim.length();
        int start = 0;
        if (delim_len == 0) return elems;
        while (pos < len)
        {
            if (delim.find(s[pos]) != string::npos) {
                elems.push_back(s.substr(start, pos - start));
                start = pos;
            }
            pos += 1;
        }
        return elems;
    }

    void printError(int line, string str) {
        cout << "[error]:  in line " << line <<  " " << str << endl;
        exit(1);
    }

};

#endif
