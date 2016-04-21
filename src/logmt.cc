#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include "logmt.h"
#include "autoconf.h"

using namespace std;

#define RUN_ALL_TIME 100
#define MAX_LOG_CUNT 50000 

const char* LOG_NAME = "../runlog/server.log";
const char* FORMAT = "../runlog/%Y-%m-%d_%H-%M-%S.log";

void logmt::logstart() {
    pthread_t tid;
    pqueue<logmt::LogNode>::instance().setsize(100);
    pqueue<logmt::LogNode>::instance().setflag(0);
    pthread_create(&tid, 0, logmt::mtlogthr, 0);
}

void logmt::logover() {
    pqueue<logmt::LogNode>::instance().setflag(1);
    pqueue<logmt::LogNode>::instance().releaseFree();
    pqueue<logmt::LogNode>::instance().resetqueue();    
}

void* logmt::mtlogthr(void *arg) {
    node<logmt::LogNode> *p = 0;
    int count = 0;
    FILE *fp = 0;
    time_t tim;
    struct tm *at;
    char fname[80];

    while (1) {
        p = pqueue<logmt::LogNode>::instance().get();
        if (!p) {
            if (fp) {
                fclose(fp);
                fp = 0;
            }
            break;
        }
        if (!count) {
            if (fp) {
                fclose(fp);
                fp = 0;
            }
            time(&tim);
            at=localtime(&tim);
            strftime(fname, 79, FORMAT ,at);
			rename(LOG_NAME, fname);
            fp = fopen(LOG_NAME, "wb");
        }
        fputs(p->m_data.m_buf, fp);
		fflush(fp);
        delete p;
        count++;
        if (count >= MAX_LOG_CUNT) {
            count = 0;
        }
    }
}

void logmt::logstring(char *str) {
    node<logmt::LogNode> *p = new node<logmt::LogNode>();
    sprintf (p->m_data.m_buf,"%s\n", str);
    pqueue<logmt::LogNode>::instance().put(p);
}

