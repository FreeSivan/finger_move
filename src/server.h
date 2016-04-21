#ifndef _SERVER_H_
#define _SERVER_H_

#include "grpcwrap.h"

/************************************************************
 *  Group Move server, Override Afpmove fucntion
 *
 *
 ***********************************************************/
class GMServer : public CMoveWrap {
public:
    static GMServer& instance();
public:
    virtual Status Afpmove(ServerContext* cxt, 
            const FingerValue* req, 
            FingerValue* res);
private:
    GMServer(){}
};

/************************************************************
 *  Group Conf server, Override Preconf fucntion
 *
 *
 ***********************************************************/
class GCServer : public CConfWrap {
public:
    static GCServer& instance();
public:
    virtual Status Preconf(ServerContext* cxt, 
            const ConfigValue* req, 
            ConfigValue* res);
private:
    GCServer(){}
};

/************************************************************
 *  Main cache Move server, Override Afpmove fucntion
 *
 *
 ***********************************************************/
class McServer : public CMoveWrap {
public:
    static McServer& instance();
public:
    virtual Status Afpmove(ServerContext* cxt, 
            const FingerValue* req, 
            FingerValue* res);
private:
    McServer(){}
};

/************************************************************
 *  subcache Sync server, Override Delafps & Addafps fucntion
 *
 *
 ***********************************************************/
class ScServer : public CSyncWrap {
public:
    static ScServer& instance();
public:
    virtual Status Delafps(ServerContext* cxt, 
            const AfpDelName* req, 
            EmptyMes* res);
    virtual Status Addafps(ServerContext* cxt, 
            const FingerValue* req, 
            EmptyMes* res);
private:
    ScServer(){}
};

#endif
/**********************************************************/

