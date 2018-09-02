#ifndef _MSFOREST_H_
#define _MSFOREST_H_

#include "../timing/query.h"
#include "../util/list_T.h"
#include "msNode.h"
#include "../util/util.h"
#include "../timing/edge.h"
#include "nodeOP.h"
#include "lockReq.h"
//in msNode.h: typedef pair<msNode*, List<match>* > JoinResult;
class timingconf;
class query;
class match;

class msforest{
public:
	msforest(timingconf* _tconf, query* _q);
	~msforest();

	void initial();

	/* there is order required on oplists  */
	bool getOPlists(dEdge* _e, vector<OPlist*>& _oplists, vector<qEdge*>& _match_edges);
	bool getTElist(dEdge* _e, vector<teNode*>& _telist, vector<qEdge*>& _match_edges);

	/*  */
	bool remove(dEdge* _e, teNode* _node, List<lockReq>* _lr_list);
	/*  */
	bool insert(dEdge* _e, qEdge* _qe, List<lockReq>* _lrlist);

	long long int to_size();
	long long int no_ms_size();
	string whole_str();
	string answers_str();

private:
	/* If _msN2matches is empty, we need to remove the lr
	 * if _branches is not NULL, clear it and add new branches into _branches
	 * */
	bool insert(teNode* _node, lockReq* _lr, List<JoinResult>* _msN2matches, List<msNode>* _branches = NULL);
	string new_match_str(List<msNode>* _branches);
	string expired_match_str(msNode* _mslist);
	static string matches_str(List<match>* _matlist);
	static string matches_str(msNode* _mslist);
	/* _node is at_left()
	 * join _matches with those in _node 
	 * results is put into _jrlist
	 * if first_join:
	 *		_matches contains only the incoming edge
	 * else 
	 *		_matches contains all new matches of a TCnode(Right sibling of _node)
	 * */
	bool join_left(List<match>* _matches, teNode* _node, lockReq* _lr,  List<JoinResult>& _jrlist);
	bool remove_used_mat(List<match>* _matches, List<JoinResult>& _jrlist);
	/* 
	 * _node is at_right()
	 * join matches from _branch_nodes with matches in _node
	 * results is put into _jrlist
	 * join_right will happen only if _node is tc_or_upper node
	 * */
	bool join_right(List<msNode>* _branch_nodes, teNode* _node, lockReq* _lr, List<JoinResult>& _jrlist);
	/* 
	 * further join for upper nodes
	 * _cur_te is the first upper teNode
	 * _branches is the valid when _cur_te is the leftmost upper node
	 * when _cur_te is non-leftmost upper node
	 * build List<match>* _mlist with _branches
	 * */
	bool further_join(teNode* _cur_te, List<msNode>* _branches, LRlist* _lrlist);

	/* return removed_list
	 * if(_rm_fathers==NULL) return NULL;
	 * _node could be upper or lower 
	 * */
	msNode* remove(teNode* _node, msNode* _rm_fathers, lockReq* _lr);
	/* only called for upper remove */
	msNode* remove(teNode* _node, List<match>* _matches, lockReq* _lr);
	void gc_release(vector<msNode*>& gclist_vec);

	match* new_match();
	match* new_match(qEdge* _qe, dEdge* _e);
	match* gather_match(match* _m);

	string tetree_str();
	string q2te_str();
	string oplist_str();
	void build_te_tree();
	void build_e2oplist();


	query* q;
	timingconf* tconf;
	map<qEdge*, OPlist*> edge2opList;
	teNode* teroot;
	map<qEdge*, teNode*> edge2node;	
	vector<teNode*> tenodes;
	/* should be saved by mutex */
	vector<match*> mat_pool;
	pthread_mutex_t matpool_mutex;
#ifdef PESSIMISTIC_LOCK
	/* -1 for Xlock, 0 for empty, >0 for Slock */
	map<teNode*, int> te2lock;
	pthread_cond_t pessimistic_cond;
	pthread_mutex_t pessimistic_mutex;

	void init_te2lock();
public:
	/* for insertion */
	void pessimistic_apply(OPlist* _oplist);
	/* for deletion */
	void pessimistic_apply(List<teNode>& _telist);
	/* for insertion */
	void reset_te2lock(List<lockReq>* _lrlist);
	/* for deletion */
	void reset_te2lock(List<teNode>& _telist);
private:
	bool is_apply(List<teNode>& _telist, char _locktype);
	bool set_apply(List<teNode>& _telist, char _locktype);
#endif
};

#endif
