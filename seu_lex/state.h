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
 * if c < 256   , 此状态有一条实边, 实边为out
 */
class State
{
public:
    int c;
    State *out;
    State *out1;
    int lastlist;

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

#endif
