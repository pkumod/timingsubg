#ifndef _QUERY_H_
#define _QUERY_H_

#include "edge.h"
class match;
class query{
public:
	query(string _query_path);
	virtual ~query() = 0;

	virtual bool parseQuery() = 0;
	bool decompose();
	vector<vector<qEdge*> >* getTCdecomp();
	bool get_matches(dEdge* _e, vector<qEdge*>& _qedges);

	bool compatible(match* _m1, match* _m2);
	bool compatible(match* _m, qEdge* _qe, dEdge* _e);
	bool compatible(qEdge* _qe1, dEdge* _e1, qEdge* _qe2, dEdge* _e2);

	void fill_timeorder();

	string to_str();
	string timingorder_str();
	string name_str();
	string param_str();
    bool is_rightmost(qEdge* _qe);

	int size();

	string query_path;
	int NumTC;
	int qNO;
	double p;
	/* Each vec<qEdge*> is a timing sequence of a TCsubq 
	 * The whole vec<vec> is a prefix-connected seq over TCsubqs
	 * */
	vector<vector<qEdge*> > TCdecomp;
	vector<qEdge*> left2right_leaf;
};

#endif
