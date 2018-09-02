#ifndef _NETQUERY_H_
#define _NETQUERY_H_

#include "../timing/query.h"

class netquery: public query{
public:
	netquery(string _query_path);
	~netquery();

	bool parseQuery();
};

#endif
