/*
 * author : xiwen.yxw_104792 
 *
 */
#ifndef _DYNAMIC_CLIENT_H_
#define _DYNAMIC_CLIENT_H_

#include <iostream>
#include <memory>
#include <string>
#include <grpc++/grpc++.h>

#include "dynamicFingerMove.grpc.pb.h"

using namespace std;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using dynamicFingerMove::FingerMove;
using dynamicFingerMove::FingerConf;
using dynamicFingerMove::FingerSync;
using dynamicFingerMove::FingerValue;
using dynamicFingerMove::ConfigValue;
using dynamicFingerMove::AfpDelName;
using dynamicFingerMove::EmptyMes;

/*********************************************************************
 *  Move server wraper
 *
 *
 ********************************************************************/
class CMoveWrap : public FingerMove::Service {
public:
    CMoveWrap(){}
    virtual ~CMoveWrap(){}
private:
    CMoveWrap(const CMoveWrap&);
    CMoveWrap& operator=(const CMoveWrap&);
};

/*********************************************************************
 *  Conf server wraper
 *
 *
 ********************************************************************/
class CConfWrap : public FingerConf::Service {
public:
    CConfWrap(){}
    virtual ~CConfWrap(){}
private:
    CConfWrap(const CConfWrap&);
    CConfWrap& operator=(const CConfWrap&);
};

/*********************************************************************
 *  Sync server wraper
 *
 *
 ********************************************************************/
class CSyncWrap : public FingerSync::Service {
public:
    CSyncWrap(){}
    virtual ~CSyncWrap(){}
private:
    CSyncWrap(const CSyncWrap&);
    CSyncWrap& operator=(const CSyncWrap&);
};

/*********************************************************************
 *  Move client wraper
 *
 *
 ********************************************************************/
class CMoveCltWrap {
public:
    CMoveCltWrap(shared_ptr<Channel> channel);
public:
    Status Afpmove(const FingerValue& req, FingerValue* res);
private:
    unique_ptr<FingerMove::Stub> m_stub;
private:
    CMoveCltWrap(const CMoveCltWrap&);
    CMoveCltWrap& operator=(const CMoveCltWrap&);
};

/*********************************************************************
 *  Conf client wraper
 *
 *
 ********************************************************************/
class CConfCltWrap {
public:
    CConfCltWrap(shared_ptr<Channel> channel);
public:
    Status Preconf(const ConfigValue& req, ConfigValue* res);
private:
    unique_ptr<FingerConf::Stub> m_stub;
private:
    CConfCltWrap(const CConfCltWrap&);
    CConfCltWrap& operator=(const CConfCltWrap&);
};

/*********************************************************************
 *  Sync Client wraper
 *
 *
 ********************************************************************/
class CSyncCltWrap {
public:
    CSyncCltWrap(shared_ptr<Channel> channel);
public:
    Status Delafps(const AfpDelName&  req, EmptyMes* res);
    Status Addafps(const FingerValue& req, EmptyMes* res);
private:
    unique_ptr<FingerSync::Stub> m_stub;
private:
    CSyncCltWrap(const CSyncCltWrap&);
    CSyncCltWrap& operator=(const CSyncCltWrap&); 
};

#endif

