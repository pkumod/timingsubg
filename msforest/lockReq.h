#ifndef _LOCKREQ_H_
#define _LOCKREQ_H_

#include "../util/list_T.h"
#include "../util/util.h"
class nodeOP;
class lockReq {
public:
	lockReq(nodeOP* _nOP,  pthread_t* _tid, int _ts);
	~lockReq();

	/*  */
	bool isXlock();
	bool isSlock();
	string to_str();
	nodeOP * op;
	pthread_cond_t  t_cond; // = PTHREAD_COND_INITIALIZER;
	pthread_t* tid;
	int timestamp;
};

typedef List<lockReq> LRlist;
#endif
