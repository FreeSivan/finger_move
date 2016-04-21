#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include "autoconf.h"

using namespace std;

#define toStr(_VAL) #_VAL

#define MAX_NAME_LEN 256
#define MAX_VAL_LEN 256

const int INSPECT_INTERVAL = 60;
const char *CONF_NAME = "../conf";


string Config::SERVER_ADDR      = "0.0.0.0:50059";
string Config::CLIENT_ADDR      = "localhost:50059";
string Config::AFP_DIR          = "";
string Config::DAT_DIR          = "";
string Config::WRK_DIR          = "";
string Config::ORG_DIR          = "";
string Config::DST_DIR          = "";
string Config::LOG_DIR          = "";
string Config::SER_MOVE_ADDR    = "";
string Config::SER_CONF_ADDR    = "";
string Config::SER_SYNC_ADDR    = "";
string Config::CLT_MOVE_ADDR    = "";
string Config::CLT_CONF_ADDR    = "";
string Config::CLT_SYNC_ADDR    = "";
string Config::GROUP_LIST       = "group_list";
string Config::SUB_LIST			= "sub_list";

unsigned int Config::GROUP_LIMIT= 100;
unsigned int Config::CACHE_LIMIT= 1800;

Config::Config() {
    m_ref[string("SERVER_ADDR")] = TAG_SERVER_ADDR;
    m_ref[string("CLIENT_ADDR")] = TAG_CLIENT_ADDR;
    m_ref[string("AFP_DIR")] = TAG_AFP_DIR; 
    m_ref[string("DAT_DIR")] = TAG_DAT_DIR;
    m_ref[string("WRK_DIR")] = TAG_WRK_DIR;
    m_ref[string("ORG_DIR")] = TAG_ORG_DIR;
    m_ref[string("DST_DIR")] = TAG_DST_DIR;
    m_ref[string("LOG_DIR")] = TAG_LOG_DIR;
    m_ref[string("SER_MOVE_ADDR")] = TAG_SER_MOVE_ADDR;
    m_ref[string("SER_CONF_ADDR")] = TAG_SER_CONF_ADDR;
    m_ref[string("SER_SYNC_ADDR")] = TAG_SER_SYNC_ADDR;
    m_ref[string("CLT_MOVE_ADDR")] = TAG_CLT_MOVE_ADDR;
    m_ref[string("CLT_CONF_ADDR")] = TAG_CLT_CONF_ADDR;
    m_ref[string("CLT_SYNC_ADDR")] = TAG_CLT_SYNC_ADDR;
    m_ref[string("GROUP_LIMIT")]= TAG_GROUP_LIMIT;
    m_ref[string("CACHE_LIMIT")]= TAG_CACHE_LIMIT;
    m_ref[string("GROUP_LIST")]	= TAG_GROUP_LIST;
	m_ref[string("SUB_LIST")]	= TAG_SUB_LIST;
}

Config& Config::instance() {
    static Config ins;
    return ins;
}

Inspect::Inspect() {
    overflag = 0;
}

void Inspect::resetInspect() {
    sleep(INSPECT_INTERVAL);
    overflag = 0;
}

void Inspect::closeInspect() {
    overflag = 1;
}

void Inspect::startInspect() {
    pthread_t tid;
    FILE *fp = 0;
    char name[MAX_NAME_LEN];
    char value[MAX_NAME_LEN];
    
    if (!(fp = fopen(CONF_NAME, "rb"))) {
        return;
    }
    while (-1 != fscanf(fp, "%s%s", name, value)) {
        dispath(name, value);
    }    
    pthread_create(&tid, 0, Inspect::routing_fn, 0);
}

Inspect& Inspect::instance() {
    static Inspect ins;
    return ins;
}

int Inspect::getOverflag() {
    return overflag;
}

void* Inspect::routing_fn(void *arg) {
    FILE *fp = 0;
    char name[MAX_NAME_LEN];
    char value[MAX_VAL_LEN];

    while (!Inspect::instance().getOverflag()) {
        fp = fopen(CONF_NAME, "rb");
        if (!fp) {
            sleep(INSPECT_INTERVAL);
            continue;
        }
        while(-1!=fscanf(fp, "%s%s",name, value)) {
            dispath(name, value);
        }
        fclose(fp);
        sleep(INSPECT_INTERVAL);
    }
}

void Inspect::dispath(char *name, char* val) {
    string str(name);
    map<string, int>::iterator it;
    it = Config::instance().m_ref.find(str);
    if (it == Config::instance().m_ref.end()) {
        goto Over;
    }
    switch (it->second) {
    case TAG_SERVER_ADDR: {
        Config::SERVER_ADDR = val;
        break;
    }
    case TAG_CLIENT_ADDR: {
        Config::CLIENT_ADDR = val;
        break;
    }
    case TAG_AFP_DIR: {
        Config::AFP_DIR = val;
        break;
    }
    case TAG_DAT_DIR: {
        Config::DAT_DIR = val;
        break;
    }
    case TAG_WRK_DIR: {
        Config::WRK_DIR = val;
        break;
    }
    case TAG_ORG_DIR: {
        Config::ORG_DIR = val;
        break;
    }
    case TAG_DST_DIR: {
        Config::DST_DIR = val;
        break;
    }
    case TAG_LOG_DIR: {
        Config::LOG_DIR = val;
        break;
    }
    case TAG_SER_MOVE_ADDR: {
        Config::SER_MOVE_ADDR = val;
        break;
    }
    case TAG_SER_CONF_ADDR: {
        Config::SER_CONF_ADDR = val;
        break;
    }
    case TAG_SER_SYNC_ADDR: {
        Config::SER_SYNC_ADDR = val;
        break;
    }
    case TAG_CLT_MOVE_ADDR: {
        Config::CLT_MOVE_ADDR = val;
        break;
    }
    case TAG_CLT_CONF_ADDR: {
        Config::CLT_CONF_ADDR = val;
        break;
    }
    case TAG_CLT_SYNC_ADDR: {
        Config::CLT_SYNC_ADDR = val;
        break;
    }
    case TAG_GROUP_LIMIT: {
        Config::GROUP_LIMIT = atoi(val);
        break;
    }
    case TAG_CACHE_LIMIT: {
        Config::CACHE_LIMIT = atoi(val);
        break;
    }
    case TAG_GROUP_LIST: {
        Config::GROUP_LIST = val;
        break;
    }
	case TAG_SUB_LIST: {
		Config::SUB_LIST = val;	
		break;
	}
    default: {
        break;
    }
    }

Over:
    return;
}

