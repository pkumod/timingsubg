#ifndef _EDGE_H_
#define _EDGE_H_

#include "../util/util.h"

class Edge{
public:
	Edge(int _s, int _t);
	int s;
	int t;

	bool ss(Edge* _e);
	bool tt(Edge* _e);
	bool st(Edge* _e);
	bool ts(Edge* _e);
	bool adj(Edge* _e);
};

class qEdge;

class dEdge: public Edge{
public:
	dEdge(int _s, int _t);
	virtual ~dEdge() = 0;
	virtual bool is_same(dEdge* _e) = 0;
	virtual bool is_before(dEdge* _e) = 0;
	virtual bool is_after(dEdge* _e) = 0;
	virtual bool is_match(qEdge* _d) = 0;

	virtual string to_str() = 0;
};


class qEdge: public Edge{
public:
	qEdge(int _s, int _t);
	virtual ~qEdge() = 0;

	bool add_preedge(qEdge* _qe);
	bool fill_pre();
	string pre_str();
	virtual bool is_before(qEdge* _q) = 0;
	virtual bool is_after(qEdge* _q) = 0;
	virtual bool is_match(dEdge* _d) = 0;
	virtual string to_str() = 0;
    virtual string label() = 0;

	/*qedges before*/
	set<qEdge*> preEdges;
	int id;
};

#endif
