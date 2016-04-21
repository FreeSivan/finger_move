#ifndef _PREDEAL_H_
#define _PREDEAL_H_

#include <string>

using namespace std;

class CProDeal {
public:
    static int getAfpName(
				const char *org, 
				const char *dst
				);
    static int movLogFile(
				const char *org, 
				const char *dst
				);
    static int cntSrhNums(
				const char *org, 
				const char *dst, 
				int flg
				);
	static int fltLogFile(
				const char *org, 
				const char *dst
				);
	static int topAfpFile(
				const char *org, 
				const char *dst, 
				int cnt
				);
	static int botAfpFile(
				const char *org, 
				const char *dst
				);
	static int movAfpFile(
				const char *org, 
				const char *dst, 
				const char *wrk
				);
	static int movCntFile(
				const char *org, 
				const char *dst, 
				const char* wrk, 
				int pos, 
				int cnt
				);
private:
    static const string afpNameFile;
    static const string logNumCount; 
	static const string fltLogsFile;
	static const string topAfpsFile;
	static const string botAfpsFile;
};

#endif

