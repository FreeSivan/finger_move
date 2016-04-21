#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "grpcwrap.h"
/*************************************************************
 *  Group Move Client
 *  
 *  GrMoveClient:
 *  afpMove     :
 *  runClient   :
 *  startClient :
 ************************************************************/
class GrMoveClient {
public:
    GrMoveClient(shared_ptr<Channel> channel);
public:
    int afpMove(const char *org);
public:
    static void  startClient();
    static void* runClient(void *arg);
private:
    CMoveCltWrap stub;
};

/*************************************************************
 *  Main Cache Move Client
 *
 *  McMoveClient:
 *  afpMove     :
 *
 ************************************************************/
class McMoveClient {
public:
    McMoveClient(shared_ptr<Channel> channel);
public:
    int afpMove(const char *org, char *addr);
private:
    CMoveCltWrap stub;
};

/*************************************************************
 *  Main Cache Sync Client
 *
 *  McSyncClient:
 *  appAdd      :
 *  appDel      :
 *
 ************************************************************/
class McSyncClient {
public:
    McSyncClient(shared_ptr<Channel> channel);
public:
    Status appAdd(const FingerValue& data);
    Status appDel(const AfpDelName&  name);
private:
    CSyncCltWrap stub;
};

/*************************************************************
 *  Main Cache Conf Client
 *
 *  McConfClient:
 *  preConf     :
 *
 ************************************************************/ 
class McConfClient {
public:
    McConfClient(shared_ptr<Channel> channel);
public:
    Status preConf(int threShold, int& output);
private:
    CConfCltWrap stub;
};

#endif
/************************************************************/

