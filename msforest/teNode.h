#ifndef _TENODE_H_
#define _TENODE_H_

#include "msNode.h"
#include "../util/list_T.h"
#include "../util/util.h"

class lockReq;
class nodeOP;

class teNode{
public:
	teNode(qEdge* _qedge, bool _leftmost=false);
	teNode(teNode* _left, teNode* _right);
	~teNode();

	bool is_root();
	bool is_leaf();
	bool is_leftmost();
	bool has_sibling();
	bool at_left();
	bool at_right();
	/* the one corresponding to TCsubq */
	bool is_TCnode_or_upper();
	void setTC_or_upper(bool _flag);

	teNode* get_sibling();
	void set_sibling(teNode* _sibling);
	teNode* get_father();
	void set_father(teNode* _father);
	void set_children(teNode* _left, teNode* _right);
	teNode* get_left();
	teNode* get_right();
	vector<qEdge*>* get_subq();

	bool get_mslist(List<msNode>& _mslist);
	bool append_lockreq(lockReq* _lr);
	bool pop_lockreq();
	bool is_cur_head(pthread_t& _tid);
	bool signal_next();
	bool signal_next_nolock();
//	lockReq* lr_head();

	bool S_lock(lockReq* _lr);
	bool X_lock(lockReq* _lr);
	bool S_release(lockReq* _lr);
	bool X_release(lockReq* _lr);


	msNode* remove_edge(dEdge* _e, lockReq* _lr);
	msNode* remove_match(set<match*>& _mset);
//	msNode* remove_match(List<match>* _mslist);
	/* remove _ms and return _ms->next */
	msNode* remove_ms(msNode* _cur_ms);
	bool add_rmlist(msNode* & _ret_mslist, msNode* & _tail, msNode* _cur_ms);

	

	/* head_mslist contains no data, initialed as new msNode */
	bool add_msnodes(List<msNode>* _mlist);
	bool add_msnodes(msNode* _mlist);

	void get_all_matches(List<match>* _mlist, bool _is_level_mat=false);

	static void build_matlist(List<match>& _matlist, msNode* _mlist_head);

	string to_matches_str(bool _is_level_mat = false);
	string to_str();
	string to_answer_str();
	string to_querystr();
	string to_spacestr();
	long long int to_size();
	long long int no_ms_size();
	int num_match();

	pthread_mutex_t te_mutex;
	pthread_mutex_t queue_mutex;
	/* 0 for empty, >0 for share, <0 for exclusive */
	int share_num;

private:	
	queue<lockReq*> lock_req_queue;
	vector<qEdge*> subquery;
	teNode* father;
	teNode* left_child;
	teNode* right_child;
	teNode* sibling;
	msNode* head_mslist;
	bool flag_leftmost;
	bool flag_tc_or_upper;
};


#endif
