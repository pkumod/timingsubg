#ifndef _MSNODE_H
#define _MSNODE_H

#include "../timing/match.h"
#include "../util/list_T.h"

class msNode;
class query;
class JoinResult{
public:
	msNode* first;
	List<match>* second;
	
	~JoinResult();
};

class msNode{
public:
	msNode(msNode* _f, msNode* _c, match* _mat);
	~msNode();

	long long int to_size();
	long long int no_ms_size();
	long long int level_mat_size();
	long long int whole_mat_size();

	msNode* father;
	msNode* child_first;
	msNode* next;
	msNode* prev;
	match* mat;
	match* path_match;
	
#ifdef MARK_DEL
	bool mark_del;
#endif

	/* join owned match with _matches into jrList */
	bool joinwith(List<match>* _matches, List<JoinResult>* jrList, query* _q);
	/* create new children over _matches and add into childrenlist */
	bool addBranches(List<match>* _matches, bool& _is_first_child);
	/* add new_list into children list */
	bool addChildren(msNode* _new_child_list, bool& _is_first_child);

	msNode* be_removed();
	bool is_dedge(dEdge* _e);
	
	static msNode* build_mslist(List<match>* _matlist, msNode* _father);

	/* get whole match */
	match* get_whole_match();
	string whole_match_str();
};

#endif
