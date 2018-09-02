#ifndef _NETQEDGE_H_
#define _NETQEDGE_H_

#include "../timing/edge.h"

class netQedge : public qEdge{
public:
	netQedge(int _s, int _t);
	netQedge(string _str_e);
	netQedge(int _s, int _t, int _sp, int _tp, int _sz, string _prot);
	~netQedge();

	bool is_before(qEdge* _q);
	bool is_after(qEdge* _q);
	bool is_match(dEdge* _d);
	string to_str();
	string label();


	int s_port;
	int t_port;
	int size;
	string prot;
};


#endif
