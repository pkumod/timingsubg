#ifndef _NETDEDGE_H_
#define _NETDEDGE_H_

#include "../timing/edge.h"

class rdfDedge : public dEdge{
public:
	rdfDedge(int _s, int _t);
	rdfDedge(string _str_e);
	~rdfDedge();
	
	bool is_same(dEdge* _d); 
	bool is_before(dEdge* _d);
	bool is_after(dEdge* _d);
	bool is_match(qEdge* _q);
	bool is_literal();

	string to_str();


	string stype;
	string otype;
	string pre;
	int t_sec;
	string literal;
};

#endif
