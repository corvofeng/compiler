/*
 *=============================================================================
 *    Filename:state.h
 *
 *     Version: 1.0
 *  Created on: April 27, 2017
 *
 *      Author: corvo
 *=============================================================================
 */

#ifndef STATE_H_
#define STATE_H_

#include <stdio.h>
#include <list>
#include <map>
#include <set>


enum {
    Match = 256,
    Split = 257
};


/*
 * NFA节点
 *
 * Represents an NFA state plus zero or one or two arrows exiting.
 * if c == Match, no arrows out; matching state.
 * If c == Split, unlabeled arrows to out and out1 (if != NULL).
 * If c < 256, labeled arrow with character c to out.
 *
 * 这里做一下解释: 枚举体中表示了一个节点的状态, 他可能没有出度(例如尾节点)或是一个节点,
 * 或是两个节点. 当然, 我们可以确保所有的节点均的出度不会超过两个,并且能够保证如果为两个出度
 * 其中一个出度一定为sigma边, 也就是可以直达的.
 *
 * if c == Match, 没有出度边, 意味着该状态为匹配状态
 * if c == Split, 此状态表示有一条或两条sigma边时
 * if c < 256   , 此状态有一条实边, 实边为out,非实边为out1
 */
class State
{
public:
    int c;
    State *out;
    State *out1;
    std::string endFunc;        // 结束状态所对应的函数, 只有该点为Match时, 值可以视为有效

    State() {}
    State(int c, State *out, State *out1);
};

/*
 * NFA片段, 取fragment之说
 */
class Frag
{
public:
    State *start;   // 起始节点
    State *end;     // 尾节点

    Frag() {}
    Frag(State *start, State *end){
        this->start = start;
        this->end = end;
    }
};

/**
 * DFA 节点状态
 */
class DState {
public:

    std::map<DState*, int> out;		// 记录DFA节点能够到达的状态以及路径
    std::set<State*> coreState;		// 当前状态所代表的NFA状态中的核心状态, 用于之后进行状态比较
    std::set<State*> allState;		// 记录所有可能状态, 在getAllState调用后被填充
    bool isEnd = false;             // 是否为接受节点
    bool hasTravel = false;         // 构建DFA时, 表示该状态是否已经被遍历

    std::string endFunc;            // 结束状态中所对应的执行函数, 只有该节点为结束状态才可以操纵该对象

    /*
     * 此函数中的参数为DState类型, 是添加DFA的路径
     */
    DState& addDState(DState* pDs, int path) {

//        printf("add DState %p with %c\n", pDs, path);
        this->out.insert(std::make_pair(pDs, path));
        return *this;
    }

    /*
     * 添加到核心状态
     */
    DState& addCoreState(State * state) {
 //       printf("insert state is %p\n", state);
        this->coreState.insert(state);
        return *this;
    }

    /*
     * 由所有的核心状态进行扩展, 最终表示出所有状态
     */
    void getAllState() {
        for(auto pCore = coreState.begin(); pCore != coreState.end(); pCore++) {
            findSimple(*pCore);
        }
    }

    /*
     * 从核心点衍生到所有点
     */
    void findSimple(State *start) {

        if (start == NULL) {
            return ;
        }

        if (this->allState.find(start) == this->allState.end()) {
//          printf("add state %p\n", start);
            this->allState.insert(start);
        }

        if (start->c == Match) {
//          printf("add end state\n");
            this->isEnd = true;
            this->endFunc = start->endFunc; // 向该状态传递结束函数
        } else if (start->c == Split) {
            findSimple(start->out);
            findSimple(start->out1);
        } else if (start->c < 256) {
            findSimple(start->out1);
        } else {
            printf("error in dfa\n");
            exit(1);
        }
    }

    /*
     * 通过核心状态的比对, 判断两个DState是否为相同的状态
     */
    bool isSimilar(DState *newDs) {

        if (this->coreState.size() != newDs->coreState.size()) {
            return false;
        }

        for (auto pDs = coreState.begin(); pDs != coreState.end(); pDs++) {
            if(newDs->coreState.find(*pDs) == newDs->coreState.end())
                return false;
        }

        for (auto pDs = newDs->coreState.begin(); pDs != newDs->coreState.end(); ++pDs) {
            if(this->coreState.find(*pDs) == this->coreState.end())
                return false;
        }

        return true;
    }

};

#endif
