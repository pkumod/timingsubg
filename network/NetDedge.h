#ifndef _NETDEDGE_H_
#define _NETDEDGE_H_

#include "../timing/edge.h"

class netDedge : public dEdge{
public:
	netDedge(int _s, int _t);
	netDedge(string _str_e);
	~netDedge();
	
	bool is_same(dEdge* _d); 
	bool is_before(dEdge* _d);
	bool is_after(dEdge* _d);
	bool is_match(qEdge* _q);

	string to_str();


	int s_port;
	int t_port;
	int size;
	int t_sec;
	int t_msec;
	string prot;
};

#endif
