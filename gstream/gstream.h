#ifndef _GSTREAM_H_
#define _GSTREAM_H_

#include "../util/util.h"

class dEdge;
class gstream{
public:
	gstream(string& _data_path);
	~gstream();
	/*
	 * check empty of alledges
	 * read all edges into alledges
	 * call reset();
	 */
	virtual bool load_edges(int _avg_win_tuple_num) = 0;
	virtual bool is_expire(dEdge* _e_old, dEdge* _e_new) = 0;
	bool reset();
	bool hasnext();
	dEdge * next();
	int size();

protected:
	vector<dEdge*>::iterator cur_itr;
	string data_path;
	vector<dEdge*> alledges;
};


#endif
