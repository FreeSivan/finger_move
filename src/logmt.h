/********************************************
 *
 *  auther  : xiwen.yxw_104792
 *  time    : 2015_11_27
 *
 *******************************************/
#ifndef _LOGMT_H_
#define _LOGMT_H_

#include "tpqueue.h"

#define MAX_LINE_LEN 1024 

/*
 * A simple log class.
 * start a new thread to write the log
 **/
class logmt {
public:
	logmt(){}
    ~logmt(){}
    static logmt& instance() {
        static logmt ins;
        return ins;
    } 
public:
    void logstring(char *str);
    void logstart();
    void logover();
public:
    static void* mtlogthr(void *arg);
private:
    logmt(const logmt&);
    logmt& operator=(const logmt&);
public:
    typedef struct lognode {
        char m_buf[MAX_LINE_LEN];
    }LogNode;    
};

#endif

