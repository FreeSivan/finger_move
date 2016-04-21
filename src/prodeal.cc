#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include "prodeal.h"
#include "autoconf.h"

using namespace std;

#define MAX_NAME_LEN 256
#define MAX_LINE_LEN 1024

const string CProDeal::afpNameFile = "afpNameFile.tmp";
const string CProDeal::logNumCount = "logNumCount.tmp";
const string CProDeal::fltLogsFile = "fltLogsFile.tmp";
const string CProDeal::topAfpsFile = "topAfpsFile.tmp";
const string CProDeal::botAfpsFile = "botAfpsFile.tmp";

int cmp(const pair<string, int>& x, const pair<string, int>& y) {
    return x.second > y.second;
}

int lmp(const pair<string, int>& x, const pair<string, int>& y) {
    return x.second < y.second;
}

int CProDeal::getAfpName(const char *org, const char *dst) {
    DIR *dp = 0;
    struct dirent *dirp;
    FILE *fp = 0;
    char fname[MAX_NAME_LEN];
    char buff[MAX_LINE_LEN];

    if (!(dp = opendir(org))) {
        // log here
        printf("getAfpName dir : %s open error!\n", org);
        goto Error;
    }
    sprintf(fname, "%s/%s", dst, afpNameFile.c_str());

    if (!(fp = fopen(fname, "wb"))) {
        // log here
        printf("getAfpName file : %s open error!\n", fname);
        goto Error;
    }

    while (dirp = readdir(dp)) {
        if (!strcmp(".", dirp->d_name) ||
            !strcmp("..", dirp->d_name)) {
            continue;
        }
        sprintf(buff, "%s\n", dirp->d_name);
        fputs(buff, fp);
    }
    fclose(fp);
    closedir(dp);
    return 0;
Error:
    return -1;
}

int CProDeal::movLogFile(const char *org, const char *dst) {
    DIR *dpo, *dpd;
    struct dirent *dirp;
    FILE *forg, *fdst;
    char orgname[MAX_NAME_LEN];
    char dstname[MAX_NAME_LEN];
    char buffer[MAX_LINE_LEN];
	time_t t, now, before;
	char s[MAX_NAME_LEN];
	struct tm *p;

	t = time(&now);
	before = t - 3600*24*6;
	p = gmtime(&before);
	strftime(s, sizeof(s), "%Y-%m-%d_%H-%M-%S.log", p);

    if (!(dpd = opendir(dst))) {
        printf("movLogFile dir : %s open error!\n", dst);
        goto Error;
    } 
    while (dirp = readdir(dpd)) {
        if (!strcmp(".", dirp->d_name) ||
            !strcmp("..", dirp->d_name)) {
            continue;
        }
		if (strcmp(dirp->d_name, s) < 0) {
			sprintf (dstname, "%s/%s", dst, dirp->d_name);
			if (remove(dstname)) {
				printf("movLogFile file : %s remove error", dst);
				//log point
			}
		}
    }
    closedir(dpd);

    if (!(dpo = opendir(org))) {
        printf("movLogFile dir : %s open error!\n", org);
        // log point
        goto Error;
    }
    while (dirp = readdir(dpo)) {
        if (!strcmp(".", dirp->d_name) ||
            !strcmp("..", dirp->d_name)) {
            continue;
        }
        sprintf (orgname, "%s/%s", org, dirp->d_name);
        sprintf (dstname, "%s/%s", dst, dirp->d_name);
        if (strcmp("server.log", dirp->d_name)) {
            rename(orgname, dstname);
            continue;
        }
        forg = fopen(orgname, "rb");
        if (!forg) {
            continue;
        }
        fdst = fopen(dstname, "wb");
        while (fgets(buffer, MAX_LINE_LEN, forg)) {
            fputs(buffer, fdst);
        }
        fclose(forg);
        fclose(fdst);
    }
    closedir(dpo);
    return 0;
Error:
    return -1;
}

int CProDeal::cntSrhNums(const char *org, const char *dst, int flg) {
    DIR *dp;
    struct dirent *dirp;
    FILE *forg, *fdst;
    char fname[MAX_NAME_LEN];
    char line[MAX_LINE_LEN];
    char buf[MAX_LINE_LEN];
    map<string, int> cntmap;
    map<string, int>::iterator mapit;
    vector<pair<string, int> > veccnt;
    vector<pair<string, int> >::iterator itv;

    if (!(dp = opendir(org))) {
        printf("cntSrhNums dir : %s open error!\n", org);
        // log point
        goto Error;
    }
    sprintf (fname, "%s/%s", dst, logNumCount.c_str());
    if(!(fdst = fopen(fname, "wb"))) {
        printf("cntSrhNums file : %s open error!\n", fname);
        // log point
        goto Error;
    }
    while (dirp = readdir(dp)) {
        if (!strcmp("..", dirp->d_name) ||
            !strcmp(".", dirp->d_name)) {
            continue;
        }
        sprintf (fname, "%s/%s", org, dirp->d_name);
        if (!(forg = fopen(fname, "rb"))) {
            continue;
        }
        while (fgets(line, MAX_LINE_LEN, forg)) {
            if(line[0] != '$' || line[1] != '$') {
                continue;
            }
            string tmp(line);
            int pos1 = tmp.find("|");
            if (pos1 == tmp.npos) {
                continue;
            }
            int pos2 = tmp.find("|", pos1+1);
            if (pos2 == tmp.npos) {
                continue;
            }
            string sid = tmp.substr(pos1+1, pos2-pos1-1);
            if (sid == "-1") {
                continue;
            }
            cntmap[sid]++;
        }
        fclose(forg);
    } 
    mapit = cntmap.begin();
    for (; mapit != cntmap.end(); ++mapit) {
        veccnt.push_back(make_pair(mapit->first, mapit->second));
    }
    if (!flg) {
		sort(veccnt.begin(), veccnt.end(), cmp);
	}
	else {
		sort(veccnt.begin(), veccnt.end(), lmp);
	}
    itv = veccnt.begin();
    for (; itv != veccnt.end(); ++itv) {
        sprintf(buf, "%s|%d\n",itv->first.c_str(), itv->second);
        fputs(buf, fdst);
    }
    fclose(fdst);
    closedir(dp);
    return 0;
Error:
    return -1;
}

int CProDeal::fltLogFile(const char *org, const char *dst) {
	FILE *fpafp, *fplog, *fpdst;
	set<int> filter;
	char line[MAX_LINE_LEN];
	char name[MAX_NAME_LEN];

	sprintf (name, "%s/%s", org, afpNameFile.c_str());
	fpafp = fopen(name, "rb");
	if (!fpafp) {
        printf ("fltLogFile file : %s open error!\n", name);
		goto Error;
	}
	sprintf (name, "%s/%s", org, logNumCount.c_str());
	fplog = fopen(name, "rb");
	if (!fplog) {
        printf ("fltLogFile file : %s open error!\n", name);
		fclose(fpafp);
		goto Error;
	}
	sprintf (name, "%s/%s", dst, fltLogsFile.c_str());
	fpdst = fopen(name, "wb");
	if (!fpdst) {
        printf ("fltLogFile file : %s open error!\n", name);
		fclose(fpafp);
		fclose(fplog);
		goto Error;
	}
	while (fgets(line, MAX_LINE_LEN, fpafp)) {
		int num = atoi(line);
		filter.insert(num);
	}
	while (fgets(line, MAX_LINE_LEN, fplog)) {
		int num = atoi(line);
		if (!filter.count(num)) {
			continue;
		}
		fputs(line, fpdst);
	}
	fclose (fpafp);
	fclose (fplog);
	fclose (fpdst);
	return 0;
Error:
	return -1;
}

int CProDeal::topAfpFile(const char *org, const char *dst, int cnt) {
	FILE *fpo, *fpd;
	char name[MAX_NAME_LEN];
	char line[MAX_LINE_LEN];
	int count = 0;

	sprintf (name, "%s/%s", org, fltLogsFile.c_str());
	fpo = fopen(name, "rb");
	if (!fpo) {
        printf ("topAfpFile file : %s open error!\n", name);
		goto Error;
	}
	sprintf (name, "%s/%s", dst, topAfpsFile.c_str());
	fpd = fopen(name, "wb");
	if (!fpd) {
        printf ("topAfpFile file : %s open error!\n", name);
		fclose (fpo);
		goto Error;
	}
	while (fgets(line, MAX_LINE_LEN, fpo)) {
		if (count >= Config::GROUP_LIMIT) {
			break;
		}
		char *songid = strtok(line, "|");
		char *val = strtok(NULL, "|");
		int num = atoi(val);
		if (num <= cnt) {
			break;
		}
		count ++;
		sprintf (line, "%s.afp\n", songid);
		fputs(line, fpd);
	}
	fclose (fpo);
	fclose (fpd);
	return count;
Error:
	return -1;
}

int CProDeal::botAfpFile(const char *org, const char *dst) {
	FILE *fpa, *fpo, *fpd;
	char name[MAX_NAME_LEN];
	set<int> fliter;
	char line[MAX_LINE_LEN];
	int count = 0, thr = 0;

	sprintf (name, "%s/%s", org, afpNameFile.c_str());
	if(!(fpa = fopen(name, "rb"))) {
        printf ("botAfpFile file : %s open error!\n", name);
		goto Error;
	}
	sprintf (name, "%s/%s", org, fltLogsFile.c_str());
	if(!(fpo = fopen(name, "rb"))) {
        printf ("botAfpFile file : %s open error!\n", name);
		fclose(fpa);
		goto Error;
	}
	sprintf (name, "%s/%s", dst, botAfpsFile.c_str());
	if(!(fpd = fopen(name, "wb"))) {
        printf ("botAfpFile file : %s open error!\n", name);
		fclose(fpa);
		fclose(fpo);
		goto Error;
	}
	
	while(fgets(line, MAX_LINE_LEN, fpo)) {
		fliter.insert(atoi(line));
	}
	
	while(fgets(line, MAX_LINE_LEN, fpa)) {
		if(count >= Config::CACHE_LIMIT) {
			break;
		}
		if(fliter.count(atoi(line))) {
			continue;
		}
		fputs(line, fpd);
		count++;
	}
	
	fseek(fpo, 0, SEEK_SET);
	while(fgets(line, MAX_LINE_LEN, fpo)) {
		if(count >= Config::CACHE_LIMIT) {
			break;
		}
		char *songid = strtok(line, "|");
		char *val = strtok(NULL, "|");
		thr  = atoi(val);
		sprintf(line, "%s\n", songid);
		fputs(line, fpd);
		count++;
	}
		
	fclose(fpo);
	fclose(fpa);
	fclose(fpd);
	return thr;
Error:
	return -1;
}

int CProDeal::movAfpFile(const char *org, const char *dst, 
	const char *wrk) {
	FILE *fp;
	char name[MAX_NAME_LEN];
	char line[MAX_LINE_LEN];
	DIR *dp;
	struct dirent *dirp;
	char oname[MAX_NAME_LEN];
	char dname[MAX_NAME_LEN];
	
	if (!(dp = opendir(dst))) {
        printf("movAfpFile dir : %s open error!\n", dst);
		goto Error;
	}
	
    while (dirp = readdir(dp)) {
        if (!strcmp(".", dirp->d_name) ||
            !strcmp("..", dirp->d_name)) {
            continue;
        }
        sprintf (name, "%s/%s", dst, dirp->d_name);
        remove(name);
    }

	sprintf (name, "%s/%s", wrk, topAfpsFile.c_str());
	if(!(fp = fopen(name, "rb"))) {
	    printf("movAfpFile file : %s open error!\n", name);
    	goto Error;
	}
	
	while (fgets(line, MAX_LINE_LEN, fp)) {
		int len = strlen(line);
		line[len-1] = 0;
		sprintf (oname, "%s/%s", org, line);
		sprintf (dname, "%s/%s", dst, line);
		rename(oname, dname);
	}

	fclose (fp);
	return 0;
Error:
	return -1;
}

int CProDeal::movCntFile(const char *org, const char *dst, 
	const char *wrk, int pos, int cnt) {
	FILE *fp = 0;
	char oname[MAX_NAME_LEN];
	char dname[MAX_NAME_LEN];
	DIR  *dp = 0;
	char name[MAX_NAME_LEN];
	char line[MAX_LINE_LEN];
	struct dirent *dirp = 0;
	int count = 0;

	if (!(dp = opendir(dst))) {
        printf("movCntFile dir : %s open error!\n", dst);
		goto Error;
	}
	while (dirp = readdir(dp)) {
		if (!strcmp(".", dirp->d_name) ||
			!strcmp("..", dirp->d_name)) {
			continue;
		}
		sprintf (name, "%s/%s", dst, dirp->d_name);
		remove(name);
	}
	sprintf (name, "%s/%s", wrk, botAfpsFile.c_str());	
	if (!(fp = fopen(name, "rb"))) {
        printf("movCntFile file : %s open error!\n", name);
		goto Error;
	}
	while (fgets(line, MAX_LINE_LEN, fp)) {
		if (count < pos) {
			count ++;
			continue;
		}
		if (count >= pos+cnt) {
			break;
		}
		int len = strlen(line);
		line[len-1] = 0;
		sprintf (oname, "%s/%s", org, line);
		sprintf (dname, "%s/%s", dst, line);
		rename(oname, dname);
		count++;
	}
	return 0;
Error:
	return -1;
}

/*
int main(int argc, char **argv) {
//	CProDeal::getAfpName(argv[1], argv[2]);
	CProDeal::getAfpName("/home/xiwen.yxw/afp_new", ".");
	CProDeal::movLogFile("./runlog", "./wrk");
	CProDeal::cntSrhNums("./wrk", ".", 1);
	CProDeal::fltLogFile(".", ".");
	//CProDeal::topAfpFile(".", ".", 40);
	int ret = CProDeal::botAfpFile(".", ".");
	printf ("ret = %d\n", ret);
	//CProDeal::movAfpFile("/home/xiwen.yxw/afp_new", "./afp", ".");
	CProDeal::movCntFile("/home/xiwen.yxw/afp_new", "./afp", ".", 110, 100);
	return 0;
}
*/
