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
    prior.insert(std::make_pair("+", "-"));
    prior.insert(std::make_pair("*", "/"));

    map<string, int> assoc;
    assoc.insert(std::make_pair("*", 1));
    assoc.insert(std::make_pair("+", 1));

    string expr3[4] = {
        "E -> E + E",
        "E -> E * E",
//        "E -> (E)",
        "E -> i"
    };

    map<string, string> exprFunc4;
    exprFunc4.insert(std::make_pair("A->A+A", "{$$=$1+$3;}"));
    exprFunc4.insert(std::make_pair("A->A-A", "{$$=$1-$3;}"));
    exprFunc4.insert(std::make_pair("A->A*A", "{$$=$1*$3;}"));
    exprFunc4.insert(std::make_pair("A->A/A", "{$$=$1/$3;}"));
    exprFunc4.insert(std::make_pair("A->a", ""));


    //LR1 *lr1 = new LR1(expr3, 3, "E", prior, assoc);
    LR1 *lr1 = new LR1(expr2, 6, "S", prior, assoc);
    lr1->grammar->printGram();
    lr1->grammar->printFirst();
    lr1->iterms();


//    lr1->printLR1();

    lr1->makeACTIONGOTO();
    lr1->printACTIONGOTO();

    delete lr1;

    /*
    Yacc *yacc = new Yacc("../input/require.y", "../input/output_yacc.c");
    yacc->scanner();
    yacc->buildTable();
    yacc->parse("../input/lex.out");
    delete yacc;
    */

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


void Yacc::scanner() {

    // printf("In Yacc Scanner\n");

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

void Yacc::parse(string filename) {
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
    vector<int> numStack;
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

        string term;
        if (token2ch.find(s.first) != token2ch.end()) {
            term = token2ch[s.first];
        } else {
            term = s.first;
        }

        int i = actionTerm[term];
        string action = res_action[state][i];

        /*
        cout << "Current state is " << state
             << " with term " << term
             << " and i " << i
             << " and action " << action << endl;

        stackPrint(symbolStack);
        stackPrint(stateStack);
        stackPrint(numStack);
        */

        string actionPrint;
        if (action[0] == 's') {
            actionPrint = action;
        } else if (action[0] == 'r') {
            int reduceNum  = std::stoi(action.substr(1));
            SingleExpress* expr = standardState->singleExprVec.at(reduceNum);
            string right = expr->right;
            actionPrint = expr->left + "->" + expr->right;
        } else if (action == "acc") {
            actionPrint = "Accept";
        }

        stackPrint(symbolStack, stateStack, numStack, actionPrint);

        if (action.empty()) {
            cout << "Can't make next step" << endl;
            exit(-1);
        }

        if (action[0] == 's') {
            symbolStack.push_back(term[0]);
            state = std::stoi(action.substr(1));
            stateStack.push_back(state);
            numStack.push_back(s.second);
        } else if (action[0] == 'r') { // reduce

            int reduceNum  = std::stoi(action.substr(1));
            SingleExpress* expr = standardState->singleExprVec.at(reduceNum);
            string right = expr->right;
            std::reverse(right.begin(), right.end());

            //expr->printSigleExpr();
            //cout << "The reduce action is " << action << endl;
            //cout << "Current is" << right.size() << " " << expr->func << endl;
            vector<int> tmp_num_vec(right.size());
            auto numIt = numStack.end();
            int tmp_num_it = right.size() - 1;
            numIt --;

            for(auto it : right) {

                auto symbolIt = symbolStack.end();
                auto stateIt = stateStack.end();
                symbolIt--;

                if (!expr->func.empty()) {
             //       cout << "Erase " << *numIt << endl;
                    tmp_num_vec[tmp_num_it] = *numIt;
                    numStack.erase(numIt);
                    numIt --;
                    tmp_num_it -= 1;
                }

                char s = *symbolIt;
                //cout << "Stack top is " << s << endl;
                if (it == s) {
                    //cout << "ok" << endl;
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
            if (!expr->func.empty()) {
                int result =  parseFunc(expr->func, tmp_num_vec);
                numStack.push_back(result);
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

int Yacc::parseFunc(string func, vector<int>& data) {
    int ret = 0;

    if (func == "{$$=$1+$3;}") {
        ret = data[0] + data[2];
    } else if (func == "{$$=$1-$3;}") {
        ret = data[0] - data[2];
    } else if (func == "{$$=$1*$3;}") {
        ret = data[0] * data[2];
    } else if (func == "{$$=$1/$3;}") {
        ret = data[0] / data[2];
    } else if (func == "{$$=$2;}") {
        ret = data[1];
    } else {
        cout << "Error in " << func << endl;
        exit(-1);
    }

    return ret;
}

void Yacc::stackPrint(vector<char> &symbolStack, vector<int>& stateStack, vector<int> numStack, string action) {

    int max_column = 8;

    int i = 0;
    for(auto it : symbolStack) {
        cout << it << " ";
        i ++;
    }
    while(i++ < max_column)
        cout << "  ";

    i = 0;
    for (auto it: stateStack) {
        cout << std::setw(3) << it ;
        i ++;
    }
    while(i++ < max_column)
        cout << "   ";

    i = 0;
    for (auto it: numStack) {
        cout << std::setw(4) << it;
        i ++;
    }

    while(i++ < max_column)
        cout << "    ";

    cout << action;
    cout << endl;

}

void Yacc::getDeclare(int line, string declare) {

    static char ch = 'a';
    if (declare.find("%token") != string::npos) {
        stringstream input(declare);
            //依次输出到result中，并存入res中
        string token;
        input >> token;
        while(input>>token) {
            token2ch.insert(std::make_pair(token, ch));
            ch ++;
        }
        //cout << "ok" << endl;
    } else if (declare.find("%left") != string::npos) {
        stringstream input(declare);
        string tmp;
        input >> tmp;
        string r, l;
        input >> l;
        assoc.insert(std::make_pair(l, 1));
        while (input >> r) {
            //cout << "l:" << l << " r:" << r << endl;
            assoc.insert(std::make_pair(r, 1));
            prior.insert(std::make_pair(l, r));
        }
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


std::pair<string, int> Yacc::lexSplit(string str) {

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

    //cout << key << "---" << retValue << endl;

    return std::make_pair(key, retValue);
}

std::vector<std::string> Yacc::split(const  std::string& s, const std::string& delim)
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


std::pair<string, string> Yacc::makeSplitExpr(string& head, string& expr, char& ch) {

    std::pair<string, string> expr_func = getExprAndFunc(expr);

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
        string tmp = pureExpr.substr(start, pos - start +1);

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

std::pair<string, string> Yacc::getExprAndFunc(string str) {
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


void Yacc::getGrammar(int line, string grammar) {
    static char ch = 'B';

    // 清除空格
    string::iterator end_pos = std::remove(grammar.begin(), grammar.end(), ' ');
    grammar.erase(end_pos, grammar.end());

    //cout << grammar << endl;

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
        /*
        cout << "Expr is " << exprfunc.first << endl;
        cout << "Func is " << exprfunc.second << endl;
        */
    }
}




