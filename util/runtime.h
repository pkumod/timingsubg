/*
 * runtime.h
 *
 */

#ifndef UTIL_RUNTIME_H_
#define UTIL_RUNTIME_H_
#include "util.h"


typedef timeval Rtime;

class runtime{
public:
	runtime();
	void initial();
	void begin();
	void end();
	double getsum();
	double getcur();
	double getavg();
	int  getnum();

	static long get_interval(Rtime* _t1, Rtime* _t2);
	static void get_time_cur(Rtime* _rtime);

private:

	long long int sum;
	long long int cur;

	Rtime t_begin;
	Rtime t_end;
	int dup_num;
};




#endif /* UTIL_RUNTIME_H_ */
