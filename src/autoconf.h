#ifndef _AUTOCONF_H_
#define _AUTOCONF_H_

#include <string>
#include <map>

using namespace std;

#define TAG_SERVER_ADDR 10
#define TAG_CLIENT_ADDR 11
#define TAG_AFP_DIR 12
#define TAG_DAT_DIR 13
#define TAG_WRK_DIR 14
#define TAG_ORG_DIR 15
#define TAG_DST_DIR 16
#define TAG_LOG_DIR 17
#define TAG_SER_MOVE_ADDR 18
#define TAG_SER_CONF_ADDR 19
#define TAG_SER_SYNC_ADDR 20
#define TAG_CLT_MOVE_ADDR 21
#define TAG_CLT_CONF_ADDR 22
#define TAG_CLT_SYNC_ADDR 23
#define TAG_GROUP_LIMIT 24
#define TAG_CACHE_LIMIT 25
#define TAG_GROUP_LIST  26
#define TAG_SUB_LIST	27

class Config {
public:
    ~Config(){};
    static Config& instance();
public:
    static string SERVER_ADDR;  // listen addr
    static string CLIENT_ADDR;  // connect addr
    static string AFP_DIR;      // finger print dir
    static string DAT_DIR;      // runlog dir
    static string WRK_DIR;      // work space
    static string ORG_DIR;      // transfer org data
    static string DST_DIR;      // transfer dst data
    static string LOG_DIR;      // self log data
    static string SER_MOVE_ADDR;// local move server addr
    static string SER_CONF_ADDR;// local conf server addr
    static string SER_SYNC_ADDR;// local sync server addr
    static string CLT_MOVE_ADDR;// remote move server addr
    static string CLT_CONF_ADDR;// remote conf server addr
    static string CLT_SYNC_ADDR;// remote sync server addr
    static string GROUP_LIST;   // the file save group list 
	static string SUB_LIST;
public:
    static unsigned int GROUP_LIMIT;
    static unsigned int CACHE_LIMIT;
private:
    Config();
    Config(Config&);
    Config& operator=(Config&);
public:
    map<string, int> m_ref;
};

class Inspect {
public:
    ~Inspect(){};
public:
    static Inspect& instance();
    static void* routing_fn(void *arg);
public:
    void startInspect();
    void closeInspect();
    void resetInspect();
    int  getOverflag();
private:
    Inspect();
    Inspect(const Inspect&);
    Inspect& operator=(const Inspect&);
private:
    static void dispath(char* name, char* val);
private:
    int overflag;
};

#endif

