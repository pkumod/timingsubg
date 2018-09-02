#ifndef _MATCH_H_
#define _MATCH_H_

#include "edge.h"
#include "../util/util.h"

typedef map<qEdge*, dEdge*> qdMap;

class match{
public:
	match(qEdge* _qe, dEdge* _e);

	match(match& _m1, match& _m2);
	match(match* _m1, match* _m2);

	match();

	match(const match& _m);

	void merge(match& _m1, match& _m2);

	match& operator=(const match& _m);

	qdMap* get_q2dM();
	
	bool add(qEdge* _qe, dEdge* _e);

	bool add(match* _mat);

	bool remove(qEdge* _qe);

	bool is_edge(dEdge * _e);

	bool contain(dEdge * _e);

	bool contain(qEdge* _qe, dEdge * _e);
	
	int size();
	int to_size();

	string to_str() const;
	string mat_query_str() const;
	string mat_data_str() const;

	map<qEdge*, dEdge*>* get_pairs();
	
//private:
	map<qEdge*, dEdge*> pairs;
};

#endif
