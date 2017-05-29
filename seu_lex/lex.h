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
using std::ios;

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


        std::ifstream* ifile = new std::ifstream();
        ifile->open(_lexFile);
        if(!ifile->is_open()) {
            printError(0, "Please input valid file");

        }
        in = ifile;

        this->lexFile = _lexFile;
        this->out = &std::cout;
    }

    Lex (string _lexFile, string _outCFile) {
        std::ifstream* ifile = new std::ifstream();
        ifile->open(_lexFile);
        if(!ifile->is_open()) {
            printError(0, "Please input valid file");

        }
        in = ifile;
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

    /**
     * 处理输入文件, 通过逐行扫描, 将输入分块. 对于恰当的块调用getFunc与getRegular函数进行
     * 初步处理
     *
     * 处理时, 分别有如下对应:
     *    'noone = isNoOne'     ->  getFunc
     *
     *    '% |{\t}|{\n} {  }'   -> getRegular
     */
    void scaner();

    // 记录自定义的类型的判断函数, digit->isDigit
    std::map<string, string> funcMap;

    // 记录自定义的类型的符号, 以便进行转换 digit->a
    std::map<string, char> type2ch;

    // 此处记录符号与判断函数对应 a -> isDigit
    std::map<char, string> ch2func;

    /**
     * 辅助函数: 记录不同的判断函数对应的字符串,
     *
     * 例如:
     *      digit   -> isDigit
     *      letter  -> isLetter
     *
     *
     * ({letter}|_)({letter}|_|{digit})*  (需要处理的正则表达式);
     * 虽然我们将digit对应于isDigit, 但是作为正则表达式处理时, 处理多个字符是有难度的, 如果
     * 我们直接修改NFA与DFA的生成类, 难度很高, 代码需要做很大的修改.
     *
     * 这里, 我们退而求其次, 因为有一些ASCII码是永远不会出现在字符串中, 所以, 我们进行这样的
     * 映射, 例如:
     *      digit -> 20
     *   对于正则表达式中的长字符, 可以直接进行替换, 这样所有的长字符就会被替换为单个字符, 正
     * 则表达式的实现不需要做大量的修改即可实现功能
     *
     * 而后生成文件时通过如下的映射进行恢复:
     *      20 -> isDigit
     *
     *
     * 在本函数中定义的静态常量:
     *   static char ch = 20;
     *   此常量为单个字符的起始常量, 有关ASCII中0-31基本为不可见的字符, 我们选择起始值为20,
     * 主要考虑到不会与其他字符冲突才进行这样的选择
     *
     */
    void getFunc(string str, int line);


    /**
     *   将每一行中的正则表达式与对应的处理函数相分离开来. 而后将正则表达式的处理结果进行存放,
     * 这里将正则表达式中的特殊部分以及需要转义的字符进行初步处理, 而后将其转化为NFA, 并且存入
     * NFA列表中, 以便于之后进行NFA合并
     *
     * 函数输入:
     * ({letter}|_)({letter}|_|{digit})* {
     *      int id = getKeyId(SYLEX_TEXT);
     *      if(id != 0)
     *          printf("<%s,->\n", SYLEX_TEXT);
     *      else {
     *          printf("<$ID,%s>\n", SYLEX_TEXT);
     *      }
     * }
     *
     * 这里给出了一个匹配变量名的正则表达式(字母,数字,下划线组成, 只能由字母与下划线开头的字符串)
     * 以及表达式对应的操作
     *
     * 首先调用getReAndFunc将正则表达式与处理逻辑相分离, 我们可以得到re以及func分别对应于两
     * 部分,
     *   re = ({letter}|_)({letter}|_|{digit})*
     *   func = {...}
     *   而后的正则表达式进行处理, 因为表达式中有{letter},{digit}这样的长名称, 因此, 我们
     * 通过state2ch来进行 {letter} -> char 的替换.
     *   生成的正则表达式将会变为:
     *   re_s = (a|_)(a|_|b)*  这里的a, b仅为填充字符, 真实的替换后正则表达式中, a b均为
     * 不可见的字符
     *
     * 另外:
     *  对于(%*=?)这样的正则表达式, 因为表达式中有'*'存在, 直接匹配将会出现错误, 因此,
     * 我们使用%进行转义, 而正则表达式中的转义为\*, 我们也在此处进行修改, 它将会变为:
     * (\*=?)
     *
     * 基础的处理过正则表达式后, 我们将正则表达式与其对应的处理函数存储进入NFA类中
     *  Re2NFA *pre2Nfa = new Re2NFA(re_s, func);
     *
     *
     * @brief getRegular
     * @param str
     * @param line
     */
    void getRegular(string str, int line);

    /**
     * 辅助函数: 获取正则以及其对应的处理函数, 使用pair进行返回
     * @brief getReAndFunc
     * @param str
     * @return
     */
    std::pair<string, string> getReAndFunc(string str);

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
        for (int i = 0; i < re2NFAList.size(); ++i) {
            Re2NFA* nfa = re2NFAList.at(i);
            nfa2List.merge(nfa);
        }
    }

    //std::map<N2DFA*, string> dfa2func;
    NFA2LIST nfa2List;

    /**
     * 合并后的NFA进行DFA的转换
     * @brief nfa2DFA
     */
    void nfa2DFA() {
        pN2DFA = new N2DFA(&nfa2List);
        pN2DFA->nfa2dfa();
    }

    void printWarnning(int line, string str) {
        cout << "[warning]:  in line " << line << " " << str << endl;
    }

    void printError(int line, string str) {
        cout << "[error]:  in line " << line <<  " " << str << endl;
        exit(1);
    }

    void outCodeTop();

    void outCodeMid();

    void outCodeBottom();

    void output() {
        //std::ostream &o = (*out);
        //o << "hello world\n";
        //o << "hello world\n";
        outCodeTop();
        outCodeMid();
        outCodeBottom();
    }

};

void test();

#endif /* ifndef LEX */
