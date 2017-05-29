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
#include <iomanip>


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

        std::ifstream* ifile = new std::ifstream();
        ifile->open(_yaccFile);
        if(!ifile->is_open()) {
            printError(0, "Please input valid file");

        }
        in = ifile;
        this->yaccFile = _yaccFile;
        this->out = &std::cout;
    }

    Yacc (string _yaccFile, string _outCFile) {
        std::ifstream* ifile = new std::ifstream();
        ifile->open(_yaccFile);
        if(!ifile->is_open()) {
            printError(0, "Please input valid file");

        }
        in = ifile;
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

        if (lr1) {
            delete lr1;
            lr1 = NULL;
        }

        if (this->out == &std::cout)
            return;
        ((std::ofstream*)out)->close();
        delete this->out;
    }


    /**
     * 进行Yacc文件解析
     * @brief scanner
     */
    void scanner();

    /**
     * 构建预测分析表
     * @brief buildTable
     */
    void buildTable() {
        /*
        for (auto it: expr2func) {
            cout << it.first << ": " << it.second << endl;
        }
        */
        lr1 = new LR1(expr2func, "A", prior, assoc);
        lr1->iterms();
        lr1->makeACTIONGOTO();
        //lr1->printACTIONGOTO();
    }

    /**
     * 记录终结符所对应的值, 由于我们只针对计算式例如a+b*c进行了分析, 所以只记录了int型数据
     */
    map<string, int> num2num;

    /**
     * 使用预测分析表进行分析指定文件
     *
     * @brief parse
     * @param filename
     */
    void parse(string filename);

    /**
     * 规约完成时将会调用该函数, 当我们输入func={$$=$1+$3;}, data=[23, 0, 16]
     * 将会计算 23 + 16
     * 返回 39
     *
     * @brief parseFunc
     */
    int parseFunc(string func, vector<int>& data);

    /**
     * 进行结果展示, 输出类似如下形式
     * 符号栈             状态栈             实际计算栈         当前动作
     * (                 0  1               0                s6
     * ( a               0  1  6            0  23            A->a
     * @brief stackPrint
     */
    void stackPrint(vector<char> &symbolStack,
                    vector<int>& stateStack,
                    vector<int> numStack,
                    string action);

    /**
     * lex 输出结果的解析
     * 解析例如
     *      <$NUM, 9>  --> NUM 9
     * @brief lexSplit
     * @param str
     * @return
     */
    std::pair<string, int> lexSplit(string str);

    map<string, string> expr2func;   // 保存表达式所对应的函数操作
    map<string, char> token2ch;
    map<string, char> gram2ch;

    map<string, string> prior;   // 记录优先级
    map<string, int> assoc;      // 记录结合性

    /**
     * 解析每一行, 记录token(也就是可能出现的终结符), 以及优先级与结合性
     * @brief getDeclare
     */
    void getDeclare(int line, string declare);

    /**
     * 获取文法, 这里一组文法被直接以一整行输入, 例如
     * expr:expr'+'expr{$$=$1+$3;}|expr'-'expr{$$=$1-$3;}|expr'*'expr{$$=$1*$3;}|expr'/'expr{$$=$1/$3;}|'('expr')'{$$=$2;}|NUM|;
     *
     *   由于需要解析成我们的LR1支持的形式, 所以, 此处进行了单个产生式以及其规约函数的分离
     *  1.分离出开始符号expr
     *  2.对于每一个表达式的产生式, 例如expr'+'expr{$$=$1+$3;}, 进行makeSplitExpr操作.
     *   产生类似 <A+A, {$$=$1+$3;}>的键值对, 其中A代表非终结符expr, 转换细节请查看函数实现
     *  3. 对于每一个产生式的类似如上的键值对, 转换为LR1可识别的输入形式, 最终保存在
     *    expr2func这个变量中, 保存键值对形式如 <A->A+A, {$$=$1+$3;}>
     *
     * 函数主要调用了下方的split族函数, 有兴趣可自行查看
     *
     * @brief getGrammar
     */
    void getGrammar(int line, string grammar);

    /**
     * 详细过程已在上面进行描述, 仅在这里列出输入输出
     * 输入:
     * :expr'+'expr{$$=$1+$3;}
     * 输出:
     *  <A+A, {$$=$1+$3;}>
     * @brief makeSplitExpr
     */
    std::pair<string, string> makeSplitExpr(string& head, string& expr, char& ch);

    /**
     * 输入:
     *      :expr'+'expr{$$=$1+$3;}
     * 输出:
     *      <:expr'+'expr, {$$=$1+$3;}>
     *
     * @brief getExprAndFunc
     */
    std::pair<string, string> getExprAndFunc(string str);

    /**
     * 输入:
     *      :expr'+'expr{$$=$1+$3;}|expr'-'expr{$$=$1-$3;}|expr'*'expr{$$=$1*$3;}
     * 输出(数组):
     *      :expr'+'expr{$$=$1+$3;}
     *      |expr'-'expr{$$=$1-$3;}
     *      |expr'*'expr{$$=$1*$3;}
     *
     * @brief split
     */
    std::vector<std::string> split(const  std::string& s, const std::string& delim);

private:
    void printError(int line, string str) {
        cout << "[error]:  in line " << line <<  " " << str << endl;
        exit(1);
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

};

#endif
