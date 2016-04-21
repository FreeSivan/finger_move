#ifndef _SERVICE_H_
#define _SERVICE_H_

/****************************************************
 *  
 *  running on the group server
 *
 *  startServer : start group server
 *  runMove     : start Move server
 *  runConf     ：start Conf server
 *
 ***************************************************/
class GrService {
public:
    static void  startServer();
    static void* runMove(void* arg);
    static void* runConf(void* arg);
};


/****************************************************
 *  
 *  running on the sub cache 
 *
 *  startServer : start sub cache server
 *  runSync     : start Sync server
 *
 ***************************************************/
class ScService {
public:
    static void  startServer();
    static void* runSync(void* arg);
};

/****************************************************
 *  
 *  running on the main cache
 *
 *  startServer : start main cache server
 *  runMove     : start Move server
 *  runMain     : start Main thread
 *
 ***************************************************/
class McService {
public:
    static void  startServer();
    static void* runMove(void* arg);
    static void* runLoop(void* arg);
};

#endif

