#ifndef _NODEOP_H_
#define _NODEOP_H_

#include "../util/list_T.h"

class teNode;

class nodeOP{
public:
	teNode* onode;
	
	nodeOP(char _lock, teNode* _node);

	bool is_join();
	bool is_insert();
	bool is_remove();
	bool is_write();
	bool is_read();

	string to_str();

private:
	
	/* S for share, X for excluse */
	char lock_type;
	/* j for join, i for insert, r for remove */
	char op_type;

};

typedef List<nodeOP> OPlist;

#endif
