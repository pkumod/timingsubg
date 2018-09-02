#ifndef _NETSTREAM_H_
#define _NETSTREAM_H_

#include "../gstream/gstream.h"
#include "NetDedge.h"
#include "NetQedge.h"

class netstream : public gstream{
public:
	netstream(string _dat_set);
	~netstream();
	
	bool load_edges(int _avg_win_tuple_num);
	bool is_expire(dEdge* _e_old, dEdge* _e_new);

private:
	int avg_span_t1;
	int avg_span_t2;
};

#endif
