#ifndef _GTRANSACTION_H_
#define _GTRANSACTION_H_

#include "../msforest/nodeOP.h"
#include "../util/util.h"
#include "../msforest/lockReq.h"

class qEdge;
class dEdge;
class gstream;
class msforest;
class teNode;
class query;

class gtransaction{
public:
	gtransaction();
	~gtransaction();


	bool thread_join();
	void set(dEdge* _e, qEdge* _qe, OPlist* _oplist, gstream* _g, msforest* _m, query* _q);
	void del_set(dEdge* _e, teNode* _node, gstream* _g, msforest* _m, query* _q);

	dEdge* e;
	qEdge* qe;
	teNode* tenode;
	LRlist* lr_list;
	gstream* G;
	msforest* M;
	query* Q;
	pthread_t id;
	List<teNode> del_tenodes;
	bool initialized;
	int count;
	bool flag_exit;
};

#endif
