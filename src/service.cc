#include <pthread.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include "autoconf.h"
#include "prodeal.h"
#include "client.h"
#include "server.h"
#include "service.h"
#include "logmt.h"

#define GROUP_MODE 0
#define MAINCACHE_MODE 1
#define SUBCACHE_MODE 2
#define LOOP_ALL_TIME_IN_MAIN_THREAD 1

logmt g_logmt;
vector<string> g_sublist;

/***********************************************************************
 * 
 *  Service running on the sub cache server
 *  
 *  startServer :
 *  runSync     :
 *
 **********************************************************************/
void ScService::startServer() {
    void *tret;
    pthread_t tid;
    // create new thread to run Sync server
    pthread_create(&tid, 0, ScService::runSync, 0);
    // main thread wait son thread
    pthread_join(tid, &tret); 
}

void* ScService::runSync(void *arg) {
    ServerBuilder builder;
    // the addr is the sync server addr
    string addr = Config::SER_SYNC_ADDR;
    builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&ScServer::instance());
    unique_ptr<Server> server(builder.BuildAndStart());
    cout<< "Server listening on " << addr << std::endl;
    server->Wait();
}

/***********************************************************************
 *
 *  Service running on the group server
 *
 *  startServer :
 *  runMove     :
 *  runConf     :
 *
 **********************************************************************/
void GrService::startServer() {
    void *tret;
    pthread_t tid1, tid2;
    // create new thread to run Move server
    pthread_create(&tid1, 0, GrService::runMove, 0);
    // create new thread to run Conf server
    pthread_create(&tid2, 0, GrService::runConf, 0);
    // main thread wait son thread
    pthread_join(tid1, &tret);
}

void* GrService::runMove(void* arg) {
    ServerBuilder builder;
    // addr is the move server's addr
    string addr = Config::SER_MOVE_ADDR;
    builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&GMServer::instance());
    unique_ptr<Server> server(builder.BuildAndStart());
    cout<< "Server listening on " << addr << std::endl;
    server->Wait();
}

void* GrService::runConf(void* arg) {
    ServerBuilder builder;
    // addr is the conf server's addr
    string addr = Config::SER_CONF_ADDR;
    builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&GCServer::instance());
    unique_ptr<Server> server(builder.BuildAndStart());
    cout<< "Server listening on " << addr << std::endl;
    server->Wait();
}

/***********************************************************************
 *
 *  Service runing on the main cache server
 *
 *  startServer :
 *  runMove     :
 *  runMain     :
 *
 **********************************************************************/
void McService::startServer() {
    void *tret;
    pthread_t tid1, tid2;
    // create new thread to run the main loop
    pthread_create(&tid1, 0, McService::runLoop, 0);
    // create new thread to run the move server
    pthread_create(&tid2, 0, McService::runMove, 0);
    pthread_join(tid1, &tret);
}

void* McService::runMove(void* arg) {
    ServerBuilder builder;
    // the addr is the move server's addr 
    string addr = Config::SER_MOVE_ADDR;
    builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&McServer::instance());
    unique_ptr<Server> server(builder.BuildAndStart());
    cout<< "Server listening on " << addr << std::endl;
    server->Wait();
}

void* McService::runLoop(void* arg) {
    while(LOOP_ALL_TIME_IN_MAIN_THREAD) {
        int  thr, lineCount = 0;
        char conf_addr[MAX_LINE_LEN];
        char move_addr[MAX_LINE_LEN];
		char outputbuf[MAX_LINE_LEN];
		cout<<Config::GROUP_LIST<<endl;
        FILE *fp = fopen(Config::GROUP_LIST.c_str(), "rb");
        if (!fp) {
			sprintf(outputbuf,"open the group list file error!");
            g_logmt.logstring(outputbuf);
            printf ("open the group list file error!\n");
            goto WaitOneDay;
        }

        CProDeal::getAfpName(Config::AFP_DIR.c_str(),
                             Config::WRK_DIR.c_str());
        CProDeal::movLogFile(Config::DAT_DIR.c_str(),
                             Config::LOG_DIR.c_str());
        CProDeal::cntSrhNums(Config::LOG_DIR.c_str(),
                             Config::WRK_DIR.c_str(),
                             1);
        CProDeal::fltLogFile(Config::WRK_DIR.c_str(),
                             Config::WRK_DIR.c_str());
        thr = CProDeal::botAfpFile(Config::WRK_DIR.c_str(),
                             Config::WRK_DIR.c_str());
        if (-1 == thr) {
			
            g_logmt.logstring((char*)"preconf Error!");
            printf ("preconf Error!\n");
            goto WaitOneDay;
        }
        while (-1!= fscanf(fp, "%s%s", conf_addr, move_addr)) {
            int result = 0;
            Status status;
            string addr(conf_addr);
            McConfClient conf(
                grpc::CreateChannel(addr, grpc::InsecureCredentials())
            );
            status = conf.preConf(thr, result);
            if (!status.ok()) {
				g_logmt.logstring((char*)"preconf Error!");
                printf ("%s preconf error!\n", conf_addr);
                continue;
            }
            CProDeal::movCntFile(Config::AFP_DIR.c_str(),
                             Config::ORG_DIR.c_str(),
                             Config::WRK_DIR.c_str(), 
                             lineCount, 
                             result);
            lineCount += result;

            string addr1(move_addr);
            McMoveClient move(
                grpc::CreateChannel(addr1, grpc::InsecureCredentials())
            );
            if(-1 == move.afpMove(Config::ORG_DIR.c_str(), move_addr)) {
				sprintf(outputbuf, "%s move finger file Error!", move_addr);
				g_logmt.logstring(outputbuf);
                printf ("%s move finger file error!\n", move_addr); 
            }
        }
    WaitOneDay:
        sleep(3600*24);
    }
}

/***********************************************************************
 *
 **********************************************************************/
int main(int argc, char **argv) {
    int flag = 0;
    int ch;
    FILE *fp = 0;
	char buf[MAX_LINE_LEN];

	while(-1!=(ch=getopt(argc, argv, "gms"))) {
        switch(ch) {
        case 'g': {
            flag = GROUP_MODE; 
            break;
        }
        case 'm': {
            flag = MAINCACHE_MODE;
            break;
        }
        case 's': {
            flag = SUBCACHE_MODE;
            break;
        }
        default : {
            break;
        }
        }
    }
    Inspect::instance().startInspect();
	cout<<Config::SUB_LIST<<endl;
	fp = fopen(Config::SUB_LIST.c_str(), "rb");
	if (!fp) {
		goto Error;
	}
	while (fgets(buf, MAX_LINE_LEN, fp)) {
		int len = strlen(buf);
		buf[len-1] = 0;
		g_sublist.push_back(string(buf));
	}
	for (int i = 0; i < g_sublist.size(); ++i) {
		cout<<g_sublist[i]<<endl;
	}
    switch (flag) {
        case GROUP_MODE: {
			g_logmt.logstart();
            GrService::startServer(); 
            break;
        }
        case MAINCACHE_MODE: {
			g_logmt.logstart();
            McService::startServer();
            break;
        }
        case SUBCACHE_MODE: {
			g_logmt.logstart();
            ScService::startServer();
            break;
        }
        default : {
            break;
        }
    }

	return 0;
Error:
	return -1;
}
/***********************************************************************/

