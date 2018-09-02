#ifndef _UTIL_H_
#define _UTIL_H_

#include <iostream>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <queue>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
using namespace std;


class util {
public:
	static ofstream* f_track;
	static ofstream* f_threadlog;
	static pthread_mutex_t mutex_log;
	static pthread_mutex_t mutex_track;
	static map<pthread_t, int> t2count;

	static void reg_track(int _count);
	static void init_track(string _track_path);
	static void track(stringstream& _ss);
	static void track(string _s, string _lat = "\n");

	static void init_threadlog(string _log_path);
	static void threadlog(stringstream& _ss);
	static void threadlog(string _s, string _lat = "\n");
	
	static void finalize();
};

#endif
