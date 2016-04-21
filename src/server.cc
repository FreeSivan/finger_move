#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include "server.h"
#include "autoconf.h"
#include "prodeal.h"
#include "client.h"
#include "logmt.h"

using namespace std;

#define MAX_NAME_LEN 256
#define MAX_LINE_LEN 1024

extern vector<string> g_sublist;
extern logmt g_logmt;

/*******************************************************************
 *  Group Move server
 *
 *  instance    : Singleton Access point
 *  Afpmove     : 
 *
 ******************************************************************/
GMServer& GMServer::instance() {
    static GMServer ins;
    return ins;
}

Status GMServer::Afpmove(ServerContext* cxt, const FingerValue* req, 
            FingerValue* res) {
    DIR *dp = 0;
    struct dirent *dirp = 0;
    char fname[MAX_NAME_LEN];
    FILE *fdp;
	char outputbuf[MAX_LINE_LEN];

    char *data = (char*)req->val().data();
    int len = req->len();
    string name = req->name();
    sprintf (fname, "%s/%s", Config::DST_DIR.c_str(), name.c_str());
    if(!(fdp = fopen(fname, "wb"))) {
		sprintf(outputbuf, "open %s error!", fname);
		g_logmt.logstring(outputbuf);
        printf ("open %s error!\n", fname);
        goto Error;
    }
	fwrite(data, 1, len, fdp);
    fclose(fdp);  
    return Status::OK;
Error:
    return Status::CANCELLED;
}

/******************************************************************
 *  Group Conf Server
 *
 *  instance    : Singleton Access point
 *  Preconf     :  
 *
 *****************************************************************/
GCServer& GCServer::instance() {
    static GCServer ins;
    return ins;
}

Status GCServer::Preconf(ServerContext* cxt, const ConfigValue* req,
            ConfigValue* res) {
    // 解析server端req
    int num = req->len();
    int count = 0;
    
    // extract the finger file's name
    // AFP_DIR : finger file dir
    // WRK_DIR : work dir, tmp file's home
    CProDeal::getAfpName(Config::AFP_DIR.c_str(), 
                         Config::WRK_DIR.c_str());

    // move run logs to LOG_DIR
    // DAT_DIR  : finger searcher log dir
    // LOG_DIR  : log work dir, tmp file's home
    CProDeal::movLogFile(Config::DAT_DIR.c_str(), 
                         Config::LOG_DIR.c_str());

    // count the finger's Query volume
    // LOG_DIR  : log work dir
    // WRK_DIR  : work dir, tmp file's home
    CProDeal::cntSrhNums(Config::LOG_DIR.c_str(), 
                         Config::WRK_DIR.c_str(),
                         0);

    // filter result
    // WRK_DIR  : work dir, tmp file's home
    // WRK_DIR  : work dir, tmp file's home
    CProDeal::fltLogFile(Config::WRK_DIR.c_str(), 
                         Config::WRK_DIR.c_str());

    // extract tops
    // WRK_DIR  : work dir, tmp file's home
    // WRK_DIR  : work dir, tmp file's home 
    // num      : Threshold
    count = CProDeal::topAfpFile(Config::WRK_DIR.c_str(), 
                         Config::WRK_DIR.c_str(), 
                         num);
    if (count <= 0) {
        // log here
		g_logmt.logstring((char*)"preconf error!");
        printf ("proconf error!\n");
        goto Error;
    }
    // move finger file to ORG_DIR
    // AFP_DIR : finger file dir
    // ORG_DIR : transfer org dir
    // WRK_DIR : work dir, tmp file's home
    CProDeal::movAfpFile(Config::AFP_DIR.c_str(), 
                         Config::ORG_DIR.c_str(), 
                         Config::WRK_DIR.c_str());

    // GrMoveClient
    GrMoveClient::startClient();

    // set return value
    res->set_len(count);
    return Status::OK;
Error:
    return Status::CANCELLED;
}



/********************************************************************
 *  Main Cahce Move Server
 *
 *  instance    : Singleton Access point
 *  Afpmove     :
 *
 *******************************************************************/
McServer& McServer::instance() {
    static McServer ins;
    return ins;
}

Status McServer::Afpmove(ServerContext* cxt, const FingerValue* req, 
            FingerValue* res) {
    DIR *dp = 0;
    struct dirent *dirp = 0;
    char fname[MAX_NAME_LEN];
    FILE *fdp;
    FingerValue request;
    string addr;    
    Status status;
	char outputbuf[MAX_LINE_LEN];
	string url = cxt->peer();

    char *data = (char*)req->val().data();
    int len = req->len();
    string name = req->name();
    
    request.set_name(name);
    request.set_val(data, len);
    request.set_len(len);

	for (int i = 0; i < g_sublist.size(); ++i) {
		//addr = Config::CLT_SYNC_ADDR;
		addr = g_sublist[i];
		McSyncClient afpAddrClient (
			grpc::CreateChannel(addr, grpc::InsecureCredentials())
		);
		status = afpAddrClient.appAdd(request);    
		if(!status.ok()) {
			int pos = Config::CLT_SYNC_ADDR.find(":");
			string tmp = Config::CLT_SYNC_ADDR.substr(0, pos);
			sprintf(outputbuf, "%s\t:\t\tmain\t\t++>\t\t"
			"%s\t\tError!", name.c_str(), tmp.c_str());
			g_logmt.logstring(outputbuf);
			printf ("add finger:sync to sub Error!\n");
			goto Error;
		}
		else {
			int pos = Config::CLT_SYNC_ADDR.find(":");
			string tmp = Config::CLT_SYNC_ADDR.substr(0, pos);
			sprintf(outputbuf, "%s\t:\t\tmain\t\t++>\t\t"
			"%s\t\tSuccess!", name.c_str(), tmp.c_str());
			g_logmt.logstring(outputbuf);
		}

		sprintf(fname, "%s/%s", Config::DST_DIR.c_str(), name.c_str());
		if(!(fdp = fopen(fname, "wb"))) {
			int pos1 = url.find(":");
			int pos2 = url.find(":", pos1+1);
			string tmp = url.substr(pos1+1, pos2-pos1-1);
			sprintf(outputbuf, "%s\t:\t\tmain\t\t<==\t\t"
			"%s\t\tError!", name.c_str(), tmp.c_str());
			g_logmt.logstring(outputbuf);
			printf ("open file : %s Error!\n", fname);
			goto Error;
		}
		else {
			fwrite(data, 1, len, fdp);
			fclose(fdp);
			int pos1 = url.find(":");
			int pos2 = url.find(":", pos1+1);
			string tmp = url.substr(pos1+1, pos2-pos1-1);
			sprintf(outputbuf, "%s\t:\t\tmain\t\t<==\t\t%s"
			"\t\tSuccess!", name.c_str(), tmp.c_str()); 
			g_logmt.logstring(outputbuf);
		}
	}
    return Status::OK;
Error:
    return Status::CANCELLED;
}

/*******************************************************************
 *  Subcache Sync Server
 *
 *  instance    : Singleton Access point
 *  Delafps     :
 *  Addafps     : 
 * 
 ******************************************************************/
ScServer& ScServer::instance() {
    static ScServer ins;
    return ins;
}

Status ScServer::Delafps(ServerContext* cxt, const AfpDelName* req,
            EmptyMes* res) {
    char name[MAX_NAME_LEN];
    string rname = req->name(); 
    sprintf(name, "%s/%s", Config::AFP_DIR.c_str(), rname.c_str());
    if (remove(name)) {
        // log here
        printf ("remove file : %s error!\n", name);
        goto Error;
    }
    
    return Status::OK;
Error:
    return Status::CANCELLED;
}

Status ScServer::Addafps(ServerContext* cxt, const FingerValue* req,
            EmptyMes* res) {
    DIR *dp = 0;
    struct dirent *dirp = 0;
    char fname[MAX_NAME_LEN];
    FILE *fdp;
    
    char *data = (char*)req->val().data();
    int len = req->len();
    string name = req->name();

    sprintf(fname, "%s/%s", Config::DST_DIR.c_str(), name.c_str());
    if(!(fdp = fopen(fname, "wb"))) {
        // log here
        printf ("open the file : %s Error!\n", fname);
        goto Error;
    }
    fwrite(data, 1, len, fdp);
    fclose(fdp);
    
    return Status::OK;
Error:
    return Status::CANCELLED;
}
/********************************************************************/

