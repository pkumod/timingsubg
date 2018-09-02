#ifndef _NETSTREAM_H_
#define _NETSTREAM_H_

#include "../gstream/gstream.h"
#include "RdfDedge.h"
#include "RdfQedge.h"

class rdfstream : public gstream{
public:
	rdfstream(string _dat_set);
	~rdfstream();
	
	bool load_edges(int _avg_win_tuple_num);
	bool is_expire(dEdge* _e_old, dEdge* _e_new);

private:
	int avg_span_t;
};

#endif
