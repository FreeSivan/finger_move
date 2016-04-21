#include <string>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include "autoconf.h"
#include "prodeal.h"
#include "client.h"
#include "logmt.h"

#define MAX_NAME_LEN 256

extern logmt g_logmt;
extern vector<string> g_sublist;
/*************************************************************************
 *  Group Move Client
 *
 *  GrMoveClient:
 *  startClient :
 *  runClient   :
 *
 ************************************************************************/
GrMoveClient::GrMoveClient(shared_ptr<Channel> channel)
    : stub(channel) {
}

void GrMoveClient::startClient() {
    pthread_t tid;
    pthread_create(&tid, 0, GrMoveClient::runClient, 0); 
}

void* GrMoveClient::runClient(void *arg) {
    string addr = Config::CLT_MOVE_ADDR;
    GrMoveClient transfer(
        grpc::CreateChannel(addr, grpc::InsecureCredentials())
    );
    transfer.afpMove(Config::ORG_DIR.c_str());
}

int GrMoveClient::afpMove(const char *org) {
    FingerValue request;
    FingerValue responst;
    Status status;
    DIR *dp;
    struct dirent *dirp;
    char fname[MAX_NAME_LEN];
    FILE *fop;
    char *filebuf = 0;
    int flen;
 
    if (!(dp = opendir(org))) {
        // log here
        printf ("open dir : %s Error\n", org);
        goto Error;
    }

    while (dirp = readdir(dp)) {
        if (!strcmp(".", dirp->d_name) ||
            !strcmp("..", dirp->d_name)) {
            continue;
        }

        sprintf (fname, "%s/%s", org, dirp->d_name);
        if (!(fop = fopen(fname, "rb"))) {
            // log here
            printf ("open file : %s Error\n", fname);
            continue;
        }

        fseek(fop, 0, SEEK_END);
        flen = ftell(fop);
        fseek(fop, 0, SEEK_SET);
        filebuf = new char[flen];
        fread(filebuf, 1, flen, fop);
        fclose(fop);

        request.set_name(string(dirp->d_name)); 
        request.set_val(filebuf, flen);
        delete[] filebuf; 
        request.set_len(flen);

        status = stub.Afpmove(request, &responst);
        
        if (status.ok()) {
            // log here
            printf ("transfer %s success!\n", dirp->d_name);
        }
        else {
            printf ("transfer %s Error!\n", dirp->d_name);
            // log here
        }
    }

    return 0;
Error:
    return -1;
}

/*************************************************************************
 *  Main Cache Move Client
 *
 *  McMoveClient:
 *  afpMove     : 
 *
 ************************************************************************/
McMoveClient::McMoveClient(shared_ptr<Channel> channel)
    :stub(channel) {
}

int McMoveClient::afpMove(const char *org, char *hostaddr) {
    FingerValue request;
    FingerValue responst;
    AfpDelName  delreq;
    Status status;
    DIR *dp;
    struct dirent *dirp;
    char fname[MAX_NAME_LEN];
    FILE *fop;
    char *filebuf = 0;
    int flen;
	char outputbuf[MAX_LINE_LEN];

    //string addr = Config::CLT_SYNC_ADDR;
    //McSyncClient syncClient(
    //    grpc::CreateChannel(addr, grpc::InsecureCredentials())
    //); 
    
    if (!(dp = opendir(org))) {
		sprintf(outputbuf, "open dir : %s Error!", org);
		g_logmt.logstring(outputbuf);
        printf ("open dir : %s Error!\n", org);
        goto Error;
    }    
    
    while (dirp = readdir(dp)) {
        if (!strcmp(".", dirp->d_name) ||
            !strcmp("..", dirp->d_name)) {
            continue;
        }
        
        sprintf (fname, "%s/%s", org, dirp->d_name);
        if (!(fop = fopen(fname, "rb"))) {
			sprintf(outputbuf, "open file : %s Error", fname);
			g_logmt.logstring(outputbuf);
            printf ("open file : %s Error!\n", fname);
            continue;
        }
        
        fseek(fop, 0, SEEK_END);
        flen = ftell(fop);
        fseek(fop, 0, SEEK_SET);
        filebuf = new char[flen];
        fread(filebuf, 1, flen, fop);
        fclose(fop);
        
        request.set_name(string(dirp->d_name)); 
        request.set_val(filebuf, flen);
        delete[] filebuf; 
        request.set_len(flen);
         
        status = stub.Afpmove(request, &responst);
        if (status.ok()) {
			char *hostname = strtok(hostaddr, ":");
			sprintf(outputbuf, "%s\t:\t\tmain\t\t==>\t\t"
			"%s\t\tSuccess!", dirp->d_name, hostname);
			g_logmt.logstring(outputbuf);
            printf ("move %s success!\n", dirp->d_name);
        }
        else {
			char *hostname = strtok(hostaddr, ":");
			sprintf(outputbuf, "%s\t:\t\tmain\t\t==>\t\t"
			"%s\t\tError!", dirp->d_name, hostname);
			g_logmt.logstring(outputbuf);
            printf ("move %s error!\n", dirp->d_name);
        }
        
        delreq.set_name(string(dirp->d_name));
		for (int i = 0; i < g_sublist.size(); ++i) {
			string addr = g_sublist[i];
			McSyncClient syncClient(
				grpc::CreateChannel(addr, grpc::InsecureCredentials())
			); 
			status = syncClient.appDel(delreq);
			if (status.ok()) {
				int pos = addr.find(":");
				string tmp = addr.substr(0, pos);
				sprintf(outputbuf, "%s\t:\t\tmain\t\t-->\t\t"
				"%s\t\tSuccess!", dirp->d_name, tmp.c_str());
				g_logmt.logstring(outputbuf);
				printf ("sync %s success!\n", dirp->d_name);
			}
			else {
				int pos = addr.find(":");
				string tmp = addr.substr(0, pos);
				sprintf(outputbuf, "%s\t:\t\tmain\t\t-->\t\t"
				"%sError!", dirp->d_name, tmp.c_str());
				g_logmt.logstring(outputbuf);
				printf ("sync %s error!\n", dirp->d_name);
			}
		}
    }
    return 0;
Error:
    return -1;
}

/*************************************************************************
 *  Main Cache Sync Client
 *
 *  McSyncClient:
 *  appAdd      :
 *  appDel      :
 *
 ************************************************************************/
McSyncClient::McSyncClient(shared_ptr<Channel> channel)
    :stub(channel) {
}

Status McSyncClient::appAdd(const FingerValue& data) {
    EmptyMes res;
    return stub.Addafps(data, &res);
}

Status McSyncClient::appDel(const AfpDelName&  req) {
    EmptyMes res;
    return stub.Delafps(req, &res);
}

/*************************************************************************
 *  Main Cache Conf Client
 *
 *  McConfClient:
 *  preConf     :
 *
 ************************************************************************/
McConfClient::McConfClient(shared_ptr<Channel> channel)
    :stub(channel) {
}

Status McConfClient::preConf(int threShold, int& output) {
    ConfigValue req;
    ConfigValue res;
    Status status;
    req.set_len(threShold);
    status = stub.Preconf(req, &res);
    output = res.len();
    return status;
}
/************************************************************************/

