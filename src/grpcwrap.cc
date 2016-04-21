/**
 * author : xiwen.yxw_104792
 */
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "grpcwrap.h"

/*************************************************************************
 *  Move client wraper
 *
 *  CMoveCltWrap    : Construct function
 *  Afpmove         : Finger move function
 *
 ************************************************************************/
CMoveCltWrap::CMoveCltWrap(shared_ptr<Channel> channel)
    :m_stub(FingerMove::NewStub(channel)) {
}

Status CMoveCltWrap::Afpmove(const FingerValue& req, FingerValue* res) {
    ClientContext context;
    return m_stub->Afpmove(&context, req, res);
}

/*************************************************************************
 *  Move client wraper
 *  
 *  CConfCltWrap    : Construct function
 *  Preconf         : Prepare before move finger
 *
 ************************************************************************/
CConfCltWrap::CConfCltWrap(shared_ptr<Channel> channel)
    :m_stub(FingerConf::NewStub(channel)) {
}

Status CConfCltWrap::Preconf(const ConfigValue& req, ConfigValue* res) {
    ClientContext context;
    return m_stub->Preconf(&context, req, res);
}

/*************************************************************************
 *  Move client wraper
 *
 *  CSyncCltWrap    : Construct function
 *  Delafps         : Sync sub cache to delete finger
 *  Addafps         : Sync sub cache to add finger
 *
 ************************************************************************/
CSyncCltWrap::CSyncCltWrap(shared_ptr<Channel> channel)
    :m_stub(FingerSync::NewStub(channel)) {
}

Status CSyncCltWrap::Delafps(const AfpDelName& req, EmptyMes* res) {
    ClientContext context;
    return m_stub->Delafps(&context, req, res);
}

Status CSyncCltWrap::Addafps(const FingerValue& req, EmptyMes* res) {
    ClientContext context;
    return m_stub->Addafps(&context, req, res);
}

