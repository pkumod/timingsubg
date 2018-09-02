#include "msforest.h"
#include "teNode.h"
#include "../timing/match.h"
#include "../timing/query.h"
#include "../timing/timingconf.h"

msforest::msforest(timingconf* _tconf, query* _q)
{
	this->q = _q;
	this->tconf = _tconf;
	this->edge2node.clear();
	this->edge2opList.clear();
	this->initial();

	pthread_mutex_init(&(this->matpool_mutex), NULL);
#ifdef PESSIMISTIC_LOCK
	pthread_mutex_init(&(this->pessimistic_mutex), NULL);
	pthread_cond_init(&(this->pessimistic_cond), NULL);
#endif
}

msforest::~msforest()
{
	pthread_mutex_destroy(&(this->matpool_mutex));
#ifdef PESSIMISTIC_LOCK
	pthread_mutex_destroy(&(this->pessimistic_mutex));
	pthread_cond_destroy(&(this->pessimistic_cond));
#endif

#ifdef DESTRUCT_LOG
	cout << "\nIN destruct msforest..." << endl;
#endif

#ifdef DEBUG_TRACK
	util::track("IN destruct msforest");
#endif
	/* free edge2oplist & edge2node */
	{/* OPlist */
		map<qEdge*, OPlist*>::iterator itr = this->edge2opList.begin();
		while(itr != this->edge2opList.end())
		{
			delete itr->second;
			itr ++;
		}
	}

	{/* teNode  */
		teNode* _cur_te = this->teroot;
		queue<teNode*> _q_te;
		_q_te.push(_cur_te);
		if(_cur_te == NULL){
			cout << "err: root NULL" << endl;
			exit(-1);
		}

		while(! _q_te.empty())
		{
			teNode* _n = _q_te.front();
			if(_n->get_left() != NULL){
				_q_te.push(_n->get_left());
			}
			if(_n->get_right() != NULL){
				_q_te.push(_n->get_right());
			}
			_q_te.pop();
			delete _n;
		}
	}

	{/* matches  */
		for(int i = 0; i < (int)this->mat_pool.size(); i ++)
		{
			delete this->mat_pool[i];
		}
	}
	
#ifdef DESTRUCT_LOG
	cout << "OUT destruct msforest..." << endl;
#endif

#ifdef DEBUG_TRACK
	util::track("OUT destruct msforest");
#endif

}

void msforest::initial()
{
	this->build_te_tree();
#ifdef GLOBAL_COMMENT
	cout << this->tetree_str() << endl;
#endif

#ifdef DEBUG_TRACK
	util::track("tetree:\n"+ this->tetree_str());
#endif
	this->build_e2oplist();
#ifdef PESSIMISTIC_LOCK
	this->init_te2lock();
#endif
}
	
long long int msforest::to_size()
{
#ifndef NO_THREAD
	return 0;
#endif
	long long int _sz = 0;
	
	_sz += sizeof(msforest);

	{/* teNode  */
		teNode* _cur_te = this->teroot;
		queue<teNode*> _q_te;
		_q_te.push(_cur_te);
		if(_cur_te == NULL){
			cout << "err: root NULL" << endl;
			exit(-1);
		}

		while(! _q_te.empty())
		{
			teNode* _n = _q_te.front();
			if(_n->get_left() != NULL){
				_q_te.push(_n->get_left());
			}
			if(_n->get_right() != NULL){
				_q_te.push(_n->get_right());
			}
			_q_te.pop();
			
			_sz += _n->to_size();
		}
	}

	return _sz;
}
	
long long int msforest::no_ms_size()
{
#ifndef NO_THREAD
	return 0;
#endif
#ifdef SPACE_LOG
	cout << "@msforest no_ms_size" << endl;
#endif

	long long int _sz = 0;
	
	_sz += sizeof(msforest);


	{/* teNode  */
		teNode* _cur_te = this->teroot;
		queue<teNode*> _q_te;
		_q_te.push(_cur_te);
		if(_cur_te == NULL){
			cout << "err: root NULL" << endl;
			exit(-1);
		}

		while(! _q_te.empty())
		{
			teNode* _n = _q_te.front();
			if(_n->get_left() != NULL){
				_q_te.push(_n->get_left());
			}
			if(_n->get_right() != NULL){
				_q_te.push(_n->get_right());
			}
			_q_te.pop();
			
			_sz += _n->no_ms_size();
#ifdef NUM_MATCH	

			bool _in_gap = false;
			if((!_n->is_leaf()) && _n->num_match() != 0){
				_in_gap = true;
				cerr << _n->to_str() << endl;
				cerr << _n->to_spacestr() << endl;
				cerr << _n->get_left() << " <--> " << _n->get_right() << endl;
				teNode* _left = _n->get_left();
				teNode* _right = _n->get_right();
				if(_left != NULL)
				{
					cerr << "--left" << endl;
					cerr << _left->to_str() << endl;
					cerr << "\t\t" << _left->to_spacestr() << endl;
				}
				if(_right != NULL)
				{
					cerr << _right->to_str() << endl;
					cerr << "\t\t" << _right->to_spacestr() << endl;
				}
				cerr << "input an int to continue..." << endl;
				int i;
			}
#endif	
		}
	}

	return _sz;
}
	

/*  */
bool msforest::getOPlists(dEdge* _e, vector<OPlist*>& _oplists, vector<qEdge*>& _match_edges)
{
	this->q->get_matches(_e, _match_edges);
	_oplists.clear();
	for(int i = 0; i < (int)_match_edges.size(); i ++)
	{
		_oplists.push_back( this->edge2opList[_match_edges[i]] );
	}
	return true;
}

bool msforest::getTElist(dEdge* _e, vector<teNode*>& _telist, vector<qEdge*>& _match_edges){
	this->q->get_matches(_e, _match_edges);
	_telist.clear();

	for(int i = 0; i < (int)_match_edges.size(); i ++)
	{
		_telist.push_back( this->edge2node[_match_edges[i]] );
	}
	return true;
}


/*  */
bool msforest::remove(dEdge* _e, teNode* _node, List<lockReq>* _lr_list){
	teNode* _cur_te = _node;
	msNode* _rm_list = NULL;
	msNode* _rm_fathers = NULL;
	vector<msNode*> gclist_vec;
#ifdef DEBUG_TRACK
	{
		stringstream _ss;
		_ss << "IN remove(dedge, te, lrlist)" << endl;
		_ss << "\t" <<_e->to_str() << endl;
		_ss << "\t[" << _node << "]" << endl;
		util::track(_ss);	

	}
#endif

	_lr_list->reset();
	if(_cur_te->is_TCnode_or_upper())
	{
#ifdef DEBUG_TRACK
		{
			stringstream _ss;
			_ss << "\tte is upper" << endl;
			util::track(_ss);	
		}
#endif
		_rm_list = _cur_te->remove_edge(_e, _lr_list->next());
		gclist_vec.push_back(_rm_list);
	}
	else
	{
#ifdef DEBUG_TRACK
		{
			stringstream _ss;
			_ss << "\tte not upper" << endl;
			util::track(_ss);	
		}
#endif
		if(! _cur_te->is_leftmost())
			_cur_te = _node->get_father();

		_rm_list = _cur_te->remove_edge(_e, _lr_list->next());
		gclist_vec.push_back(_rm_list);

		while(! _cur_te->is_TCnode_or_upper())
		{
			_cur_te = _cur_te->get_father();
			_rm_fathers = _rm_list;
			_rm_list = this->remove(_cur_te, _rm_fathers, _lr_list->next());
			gclist_vec.push_back(_rm_list);
		}
	}
	/* lockreqs are to be removed */
	//if(_rm_list == NULL) return false;

	if(_cur_te->is_root())
	{
		this->gc_release(gclist_vec);
		return true;
	}

	if(_cur_te->at_right())
	{/* change branches into matlist */
		/* build mat_list */
		List<match> _matlist;
		teNode::build_matlist(_matlist, _rm_list);

#ifdef DEBUG_TRACK
		{
			stringstream _ss;
			_ss << "to be removed matches in upper level:" << endl;
			_ss << msforest::matches_str(&_matlist) << endl;
			util::track(_ss);
		}
#endif

		/* for upper remove*/
		_cur_te = _cur_te->get_father();	
		_rm_list = this->remove(_cur_te, &_matlist, _lr_list->next());
		gclist_vec.push_back(_rm_list);
	}
	/* bug: I can not figure out why I added 'else' here, totally wrong  */
	//else
	{/* still branches */
		while(! _cur_te->is_root())
		{
#ifdef DEBUG_TRACK
		{
			stringstream _ss;
			_ss << "\n_cur_te is " << _cur_te->to_str() << endl;
			_ss << msforest::expired_match_str(_rm_list) << endl;
			util::track(_ss);
		}
#endif
			_cur_te = _cur_te->get_father();
			_rm_fathers = _rm_list;
			
			_rm_list = this->remove(_cur_te, _rm_fathers, _lr_list->next());
			gclist_vec.push_back(_rm_list);
		}
	}

	/* report expired answers */
#ifdef GLOBAL_COMMENT
	if(_rm_list != NULL)
	{
		cout << "\n\nexpired answers: \n";
		msNode* _cur_ms = _rm_list;
		while(_cur_ms != NULL)
		{
			cout << _cur_ms->whole_match_str() << endl;
			_cur_ms = _cur_ms->next;
		}
	}
#endif

	this->gc_release(gclist_vec);

	return true;
}

	
void msforest::gc_release(vector<msNode*>& gclist_vec)
{
	/* release all in gclist_vec */
	msNode* _cur_gc = NULL;
	msNode* _pre_gc = NULL;
	for(int i = 0; i < (int)gclist_vec.size(); i ++)
	{
		_cur_gc = gclist_vec[i];
		while(_cur_gc != NULL)
		{
			_pre_gc = _cur_gc;
			_cur_gc = _cur_gc->next;
#ifdef MARK_DEL
			if(_pre_gc->mark_del == false)
			{
				int _tmpc = 0;
				while(_tmpc ++ < 10)
				{
					cout << _tmpc << "delete no mark!!!!" << endl;
				}
			}
#endif
			delete _pre_gc;
		}
	}
	
}

/* 
 * 
 * */
bool msforest::insert(dEdge* _e, qEdge* _qe, List<lockReq>* _lrlist)
{
	nodeOP* _first_op = _lrlist->first()->op;
	teNode* _first_node = _first_op->onode;
	if(_first_node->is_root()){
		cout << "insert err: root" << endl;
		return false;
	}

	
	_lrlist->reset();
	if(_first_node->is_leftmost() && _first_op->is_insert())
	{

#ifdef DEBUG_TRACK 
		util::track("\t\tfirst node is at_leftmost&insert");
		util::track("\t\t add match to first node");
#endif

		/* insert the match into the MS-tree at depth=1 */
		msNode* _mslist = new msNode(NULL, NULL, this->new_match(_qe, _e));
		/* apply lock there */
		_first_node->X_lock(_lrlist->first());
		_first_node->add_msnodes(_mslist);	
		_first_node->X_release(_lrlist->first());
		_lrlist->next();
		/* first node could be tc_or_upper node */
		if(_first_node->is_TCnode_or_upper())
		{/* single edge TCnode  */
			/* further join for upper MS-tree */	

#ifdef DEBUG_TRACK 
			util::track("first node is TC or upper1");
#endif

			List<msNode> _branches(false);
			_branches.add(_mslist);
			this->further_join(_first_node, &_branches, _lrlist);
		}
#ifdef DEBUG_TRACK 
		else
		{
			util::track("first node is not tc or upper2");	
			if(_lrlist->size() != 1){
				cout << "err: lrlist size not 1 = " << _lrlist->size() << endl;
				exit(-1);
			}
		}
#endif

		return true;
	}
	else
	{
		
#ifdef DEBUG_TRACK 
		util::track("first is not leftmost or not insert op");
#endif
		
		/* a join + a insert = |oplist| is 2 */
		List<msNode> _branches(false);
		List<JoinResult> _jrlist(true);
		/* if some match* is used, make sure they are added into mat_pool */
		List<match> _mlist(false);
		match* _mat = new match(_qe, _e);
		_mlist.add(_mat);
		/* when first is join, _first_node is at_right() */
		this->join_left(&_mlist, _first_node, _lrlist->first(), _jrlist);
		_lrlist->next();

		if(_jrlist.size() != 0){
			this->gather_match(_mat);
		}
		else{
			delete _mat;
		}

		teNode* _father = _first_node->get_father();


		this->insert(_father, _lrlist->second(), &_jrlist, &_branches);	
		_lrlist->next();

		if(_father->is_TCnode_or_upper())
		{/*	child of a TCnode
			further join for upper MS-tree */
			this->further_join(_father, &_branches, _lrlist);
		}
	}

	return true;
}


/* If _msN2matches is empty, we need to remove the lr 
 * if _branches is not NULL, clear it and add new branches into _branches
 * */
bool msforest::insert(teNode* _node, lockReq* _lr, List<JoinResult>* _msN2matches, List<msNode>* _branches)
{
	
#ifdef DEBUG_TRACK 
		util::track("IN insert te JRlist bran AT "+_node->to_str());
		util::track("matches are : "+_node->to_matches_str());
#endif

	_node->X_lock(_lr);

	if(_branches != NULL){
		_branches->clear();
	}	
	if(_msN2matches->empty()){
		/* remove lr */

#ifdef DEBUG_TRACK 
		util::track("remove lr@insert te jrlist bran AT "+_node->to_str());
#endif
		_node->X_release(_lr);
		return false;
	}

	_msN2matches->reset();
	JoinResult* pair;
	msNode* _msnode;
	List<match>* _mlist;
	msNode* _list;
	msNode* _tmp_node;
	bool _is_first_child = false;


#ifdef DEBUG_TRACK
	{
		stringstream _ss;
		_ss << "size of _msN2matches = " << _msN2matches->size() << endl;
		util::track(_ss);
	}
#endif
	_msN2matches->reset();
	while(_msN2matches->hasnext())
	{
		pair = _msN2matches->next();
		_msnode = pair->first;
		_mlist = pair->second;
		_list = msNode::build_mslist(_mlist, _msnode);
		_tmp_node = _list;
		while(_tmp_node != NULL && _branches != NULL)
		{/* branches should be build before addChildren */
			_branches->add(_tmp_node);
			_tmp_node = _tmp_node->next;
		}

#ifdef DEBUG_TRACK
		{
			stringstream _ss;
			_ss << "\t\tsz of _mlist=" << _mlist->size() << endl;
			_ss << "\t\tsz of branches=";
			if(_branches == NULL)
				_ss <<  ("NULL") << endl;
			else
				_ss << _branches->size() << endl;
			util::track(_ss);
		}
#endif
		_msnode->addChildren(_list, _is_first_child);
		if(_is_first_child)
		{
			_node->add_msnodes(_msnode->child_first);


#ifdef DEBUG_TRACK
			{
				if(_msnode->child_first->prev == NULL)
				{
					util::track("prev is NULL");
				}
			}
#endif

		}
		
	}

	if(_node->is_root() && _branches != NULL)
	{
		if(! _branches->empty())
		{
#ifdef GLOBAL_COMMENT
			cout << this->new_match_str(_branches) << endl;		
#endif
#ifdef CYBER
            cout << this->new_match_str(_branches) << endl;
#endif
		}
	}

	_node->X_release(_lr);

#ifdef DEBUG_TRACK
	util::track("Out insert(te, JRlist, bran)");
#endif
	return true;
}


/* _node is at_left()
 * join _matches with those in _node 
 * results is put into _jrlist
 * if first_join:
 *		_matches contains only the incoming edge
 * else 
 *		_matches contains all new matches of a TCnode(Right sibling of _node)
 * */
bool msforest::join_left(List<match>* _matches, teNode* _node, lockReq* _lr, List<JoinResult>& _jrlist)
{
#ifdef DEBUG_TRACK
	util::track("IN join_left...");
#endif

	_jrlist.clear();
	List<msNode> _mslist(false);

	_node->S_lock(_lr);	

	_node->get_mslist(_mslist);
	
#ifdef DEBUG_TRACK
	util::track("IN join_left with node " + _node->to_str());
	util::track(_node->to_matches_str());
#endif

	if(_mslist.empty()){
		/* remove lr */

#ifdef DEBUG_TRACK
		util::track("remove lr@join_left AT "+_node->to_str());
#endif

		_node->S_release(_lr);
		return false;
	}


#ifdef DEBUG_TRACK
	{
		stringstream _ss;
		_ss << "size of mslist = " << _mslist.size() << endl;
		util::track(_ss);
	}
#endif

	msNode* _cur;
	_mslist.reset();
	while(_mslist.hasnext())
	{
		_cur = _mslist.next();


#ifdef DEBUG_TRACK
		if(_cur == NULL){
			util::track("err NULL1@join_left");
		}
#endif

		_cur->joinwith(_matches, &_jrlist, this->q);
	}

#ifdef DEBUG_TRACK
	{
		stringstream _ss;
		_ss << " join pairs= " << _jrlist.size() << endl;
		_ss << "after joinwith, before remove used" << endl;
		util::track(_ss);
	}
#endif

	/* this will never be TRUE now */
	if(_matches->is_to_free())
	{
		this->remove_used_mat(_matches, _jrlist);
	}

	_node->S_release(_lr);

#ifdef DEBUG_TRACK
	util::track("OUT join_left" );
#endif

	return true;
}

bool msforest::remove_used_mat(List<match>* _matches, List<JoinResult>& _jrlist)
{
	/* collect all matches */
	set<match*> _mset;
	_matches->reset();
	while(_matches->hasnext())
	{
		_mset.insert(_matches->next());
	}

#ifdef DEBUG_TRACK
	{
		stringstream _ss;
		_ss << " size of set of matches: " << _mset.size() << endl;
		util::track(_ss);
	}
#endif

	/* erase used ones */
	_jrlist.reset();
	while(_jrlist.hasnext())
	{
		JoinResult* _jr = _jrlist.next();
		List<match>* _mlist = _jr->second;
		_mlist->reset();
		while(_mlist->hasnext())
		{
			_mset.erase(_mlist->next());
		}
	}


#ifdef DEBUG_TRACK
	{
		stringstream _ss;
		_ss << "remaining of set of matches: " << _mset.size() << endl;
		util::track(_ss);
	}
#endif


#ifdef DEBUG_TRACK
	util::track("after erase used ones, before pop");
	int _count_pop = 0;
#endif

	/* pop: remove but not release used ones */
	_matches->reset();
	while(_matches->hasnext())
	{
		if(_mset.find(_matches->current()) == _mset.end())
		{
			_matches->pop_current();
#ifdef DEBUG_TRACK
			_count_pop ++;
#endif
		}
		else
		{
			_matches->next();
		}
	}

#ifdef DEBUG_TRACK
	{
		stringstream _ss;
		_ss << "Those is" << _count_pop <<  " to be delete: " << endl;
		_matches->reset();
		while(_matches->hasnext())
		{
			_ss << "\t\t" << _matches->next()->to_str() << endl;
		}
		util::track(_ss);
	}
#endif

	return true;
}

/* 
 * _node is at_right()
 * join matches from _branch_nodes with matches in _node
 * results is put into _jrlist
 * join_right will happen only if _node is tc_or_upper node
 * */
bool msforest::join_right(List<msNode>* _branch_nodes, teNode* _node, lockReq* _lr, List<JoinResult>& _jrlist)
{
#ifdef DEBUG_TRACK
	util::track("IN join_right1");
#endif
	_node->S_lock(_lr);

#ifdef DEBUG_TRACK
	util::track("IN join_right");
	util::track(_node->to_matches_str());
#endif

	bool _no_join = false;
	if(_branch_nodes == NULL){
		_no_join = true;
	}
	else
	if(_branch_nodes->empty()){
		_no_join = true;
	}

	if(_no_join)
	{
		/* remove lr  */

#ifdef DEBUG_TRACK
		util::track("remove lr@join_right");
#endif
		_node->S_release(_lr);
		return false;
	}

	List<match> _mlist(false);
	bool _is_level_mat = false;
	_node->get_all_matches(&_mlist, _is_level_mat);
	_branch_nodes->reset();
	msNode* _cur;
	while(_branch_nodes->hasnext())
	{
		_cur = _branch_nodes->next();  
		_cur->joinwith(&_mlist, &_jrlist, this->q);
	}


	_node->S_release(_lr);

#ifdef DEBUG_TRACK
	util::track("Out join_right");
#endif

	return true;	
}

/* 
 * further join for upper nodes
 * _cur_te is the first upper teNode
 * _branches is the valid when _cur_te is the leftmost upper node
 * when _cur_te is non-leftmost upper node
 * build List<match>* _mlist with _branches
 * */
bool msforest::further_join(teNode* _cur_te, List<msNode>* _branches, LRlist* _lrlist)
{
#ifdef DEBUG_TRACK
	util::track("IN further_join");
#endif

	
	List<JoinResult> _jrlist(true);

	if(_cur_te->at_right())
	{/* 
	  *	while non-leftmost should conduct a join-left first
	  *	join new matches in _mlist with matches in sibling of _first_node
	  */


#ifdef DEBUG_TRACK
	util::track("_cur_te at right@further_join");
#endif

		_jrlist.clear();
		List<match> _mlist(false);
		{/* build _mlist over _branches */
			_branches->reset();
			while(_branches->hasnext())
			{
				/* when delete happen, pointer matched is OK */
				_mlist.add(_branches->next()->get_whole_match());
			}
		}


#ifdef DEBUG_TRACK
	util::track("join_left1@further_join");
#endif

		this->join_left(&_mlist, _cur_te->get_sibling(), _lrlist->next(), _jrlist);	


#ifdef DEBUG_TRACK
	util::track("insert1@further_join");
#endif
		
		_cur_te = _cur_te->get_father();
		this->insert(_cur_te, _lrlist->next(), &_jrlist, _branches);	
	}

	while(! _cur_te->is_root())
	{
		_jrlist.clear();
		this->join_right(_branches, _cur_te->get_sibling(), _lrlist->next(), _jrlist);
		this->insert(_cur_te->get_father(), _lrlist->next(), &_jrlist, _branches);
		_cur_te = _cur_te->get_father();
	}
	
#ifdef DEBUG_TRACK
	util::track("OUT further_join");
#endif
	return true;
}

string msforest::new_match_str(List<msNode>* _branches){
	stringstream _ss;
	_ss << "\n**************There are " << _branches->size() << " new matches" << endl;
	_branches->reset();
	while(_branches->hasnext())
	{
		msNode* _next = _branches->next();
		_ss << "\t" << _next->whole_match_str() << endl;
	}

/*
#ifdef DEBUG_TRACK
	util::track(_ss);
#endif
*/
	return _ss.str();
}
	
string msforest::expired_match_str(msNode* _mslist)
{
	stringstream _ss;
	int _count= 0;
	_ss << "There are expired matches" << endl;
	msNode* _cur_ms = _mslist;
	while(_cur_ms != NULL)
	{
		_ss << "\t" << (++_count);
#ifdef MARK_DEL	
		_ss << ": mark(" << _cur_ms->mark_del << ")";
#endif
		_ss << _cur_ms->whole_match_str() << endl;
		_cur_ms = _cur_ms->next;
	}


#ifdef DEBUG_TRACK
	_ss << "\t\ttotal exp = " << _count << endl;
	util::track(_ss);
#endif

	return _ss.str();
}

string msforest::matches_str(List<match>* _matlist)
{
	stringstream _ss;

	_matlist->reset();
	while(_matlist->hasnext())
	{
		_ss << _matlist->next()->to_str() << endl;
	}

	return _ss.str();
}

string msforest::matches_str(msNode* _mslist)
{
	stringstream _ss;

	msNode* _cur_ms = _mslist;
	while(_cur_ms != NULL)
	{
		_ss << _cur_ms->whole_match_str() << endl;
		_cur_ms = _cur_ms->next;
	}

	return _ss.str();
}

/* remove all msNodes whose fathers are in _rm_fathers list */
msNode* msforest::remove(teNode* _node, msNode* _rm_fathers, lockReq* _lr){
	_node->X_lock(_lr);

	if(_rm_fathers == NULL)
	{
		_node->X_release(_lr);
		return NULL;
	}

	msNode* _ret_mlist = NULL;
	msNode* _cur_ms = _rm_fathers;
	msNode* _tail_ms;
	msNode* _tmp_ms;
	while(_cur_ms != NULL)
	{
#ifdef MARK_DEL
		if(_cur_ms->mark_del == false)
		{
			int _tmpc = 0;
			cerr << _cur_ms->whole_match_str() << endl;
			while(_tmpc ++ < 10)
				cerr << "rm_father is no marked del" << endl;	
			cerr << _node->to_str() << endl;
		}
#endif
		if(_cur_ms->child_first != NULL)
		{
			if(_ret_mlist == NULL)
			{
				_ret_mlist = _cur_ms->be_removed();
				_tail_ms = _ret_mlist;
			}
			else
			{
				_tmp_ms = _cur_ms->be_removed();
				_tail_ms->next = _tmp_ms;
				_tmp_ms->prev = _tail_ms;
			}
			/*adjust new tail*/	
			while(_tail_ms->next != NULL)
			{
				_tail_ms = _tail_ms->next;
			}
		}
		_cur_ms = _cur_ms->next;
	}

	_node->X_release(_lr);

	return _ret_mlist;
}

msNode* msforest::remove(teNode* _node, List<match>* _matches, lockReq* _lr){
#ifdef DEBUG_TRACK
	util::track("IN remove te, matlist");

	{
		stringstream _ss;
		_ss << "matches in node " << _node->to_str() << endl;
		_ss << _node->to_matches_str(true) << endl;
		util::track(_ss);
	}
#endif

	_node->X_lock(_lr);
	if(_matches == NULL){
		_node->X_release(_lr);
		return NULL;
	}

	msNode* _ret_mlist = NULL;	

/* this is ifndef  instead of ifdef */
#ifndef PTR_MATCH
	//_ret_mlist = _node->remove_match(_matches);
#else
	set<match*> _mset;
	_matches->reset();
	while(_matches->hasnext())
	{
		_mset.insert(_matches->next());
	}
	_ret_mlist = _node->remove_match(_mset);
#endif
	
#ifdef DEBUG_TRACK
	{
		stringstream _ss;
		_ss << msforest::expired_match_str(_ret_mlist);
		_ss << "OUT remove(te matlist lr)" << endl;
		util::track(_ss);
	}
#endif

	_node->X_release(_lr);

	return _ret_mlist;
}

match* msforest::new_match(){
	pthread_mutex_lock(&(this->matpool_mutex));
	match* _new = new match();
	this->mat_pool.push_back(_new);
	pthread_mutex_unlock(&(this->matpool_mutex));
	return _new;
}

match* msforest::new_match(qEdge* _qe, dEdge* _e)
{
	pthread_mutex_lock(&(this->matpool_mutex));
	match* _new = new match(_qe, _e);
	this->mat_pool.push_back(_new);
	pthread_mutex_unlock(&(this->matpool_mutex));
	return _new;
}
	
match* msforest::gather_match(match* _m)
{
	pthread_mutex_lock(&(this->matpool_mutex));
	this->mat_pool.push_back(_m);
	pthread_mutex_unlock(&(this->matpool_mutex));
	return NULL;
}

string msforest::tetree_str(){
	stringstream _ss;
	teNode* _cur_te = this->teroot;
	queue<teNode*> _q_te;
	_q_te.push(_cur_te);
	if(_cur_te == NULL) return "teroot=NULL";

	while(! _q_te.empty())
	{
		teNode* _n = _q_te.front();
		_ss << _n->to_str() << endl;
		if(_n->get_left() != NULL){
			_q_te.push(_n->get_left());
		}
		if(_n->get_right() != NULL){
			_q_te.push(_n->get_right());
		}
		_q_te.pop();
	}

	return _ss.str();
}
	
string msforest::whole_str(){
#ifndef NO_THREAD
	return "!NO_THREAD";
#endif
	stringstream _ss;
	teNode* _cur_te = this->teroot;
	queue<teNode*> _q_te;
	_q_te.push(_cur_te);
	if(_cur_te == NULL) return "teroot=NULL";

	while(! _q_te.empty())
	{
		teNode* _n = _q_te.front();
		_ss << _n->to_matches_str() << endl << endl << endl;
		if(_n->get_left() != NULL){
			_q_te.push(_n->get_left());
		}
		if(_n->get_right() != NULL){
			_q_te.push(_n->get_right());
		}
		_q_te.pop();
	}

	return _ss.str();
}

	
string msforest::answers_str()
{
    return this->teroot->to_answer_str();
}

string msforest::oplist_str(){
	stringstream _ss;
	
	map<qEdge*, OPlist*>::iterator itr = this->edge2opList.begin();
	_ss << "edge2oplist:" << endl;
	while(itr != this->edge2opList.end())
	{
		_ss << "\t" << itr->first->to_str() << " has following op:\n  ";
		itr->second->reset();
		while(itr->second->hasnext()){
			nodeOP* _nop = itr->second->next();
			string _tmps = _nop->to_str();
			_ss << "\t\t" << _tmps << endl;
		}
		itr ++;
	}
	

	return _ss.str();
}

string msforest::q2te_str()
{
	stringstream _ss;
	map<qEdge*, teNode*>::iterator itr = this->edge2node.begin();
	while(itr != this->edge2node.end())
	{
		_ss << itr->first->to_str() << "\t" << itr->second->to_str() << endl;
		itr ++;
	}
	return _ss.str();
}

/*
 * build_tree: teroot
 * load edge2node
 * set leftmost
 * set tc_or_upper
 */
void msforest::build_te_tree()
{

#ifdef DEBUG_TRACK
	util::track("In build_te_tree");
#endif

	if(! this->edge2node.empty()){
		cout << "err not empty for edge2node" << endl;
		exit(-1);
	}

	vector<vector<qEdge*> >* tcD = this->q->getTCdecomp();
	
	this->tenodes.clear();
	for(int i = 0; i < (int)(tcD->size()); i ++)
	{
		/* _cur_root is for current TCsubq */
		qEdge* _cur_qe = (*tcD)[i][0];
		teNode* _cur_root = new teNode( _cur_qe, true );
		this->tenodes.push_back(_cur_root);
		this->edge2node[ _cur_qe ] = _cur_root;
		int _i_size = (*tcD)[i].size();
		for(int j = 1; j < _i_size; j ++)
		{
			qEdge* _ij_qe = (*tcD)[i][j];
			teNode* _new_leaf = new teNode(_ij_qe, false);
			this->tenodes.push_back(_new_leaf);
			_cur_root = new teNode(_cur_root, _new_leaf);
			this->tenodes.push_back(_cur_root);
			this->edge2node[_ij_qe] = _new_leaf;
		}

		_cur_root->setTC_or_upper(true);
		/* teroot is the final root */
		if(i == 0) 
			this->teroot = _cur_root;
		else
		{
			this->teroot = new teNode(this->teroot, _cur_root);
			this->tenodes.push_back(this->teroot);
			this->teroot->setTC_or_upper(true);
		}
	}

#ifdef RUN_COMMENT
	cout << "q2testr:\n" << this->q2te_str() << endl;
#endif

#ifdef DEBUG_TRACK
	util::track(this->q2te_str());
	util::track("Out build_te_tree");
#endif
}

void msforest::build_e2oplist()
{

#ifdef DEBUG_TRACK
	util::track("In build_e2oplist");
	{
		stringstream _ss;
		_ss << "size of edge2node = " << this->edge2node.size() << endl;
		util::track(_ss);
	}
#endif

	if(! this->edge2opList.empty()){
		cout << "err not empty for edge2opList" << endl;
		exit(-1);
	}

	map<qEdge*, teNode*>::iterator itr = this->edge2node.begin();
	while(itr != this->edge2node.end())
	{
		OPlist* _list = new OPlist(true);/* to be free */
		teNode* _cur_te = itr->second;
		if(_cur_te->is_leftmost())
		{/* may be not only an insertion */
			nodeOP* _op = new nodeOP('i', _cur_te);			
			_list->add(_op);
		}
		else
		{/* _cur_te must be right_leaf and has a father */
			/* join and insert */
			nodeOP* _op_j = new nodeOP('j', _cur_te->get_sibling());
			_list->add(_op_j);
			nodeOP* _op_i = new nodeOP('i', _cur_te->get_father());
			_list->add(_op_i);
			_cur_te = _cur_te->get_father();
		}

		if(_cur_te != NULL)
		{
			if(!_cur_te->is_root() &&
				_cur_te->is_TCnode_or_upper())
			{/* repeat join and insert */
				while(_cur_te != NULL)
				{
					if(_cur_te->is_root()) break;

					nodeOP* _op_j = new nodeOP('j', _cur_te->get_sibling());
					_list->add(_op_j);
					nodeOP* _op_i = new nodeOP('i', _cur_te->get_father());
					_list->add(_op_i);
					_cur_te = _cur_te->get_father();
				}	
			}
		}

		this->edge2opList[ itr->first ] = _list;

		itr ++;
	}/* while each in edge2node */	

#ifdef RUN_COMMENT
	cout << this->oplist_str() << endl;
#endif

#ifdef DEBUG_TRACK
	util::track(this->oplist_str());
	util::track("Out build_e2oplist");
#endif

}

#ifdef PESSIMISTIC_LOCK
void msforest::init_te2lock()
{
#ifdef DEBUG_TRACK
	util::track(" In init te2lock");
#endif
	this->te2lock.clear();
	for(int i = 0; i < (int)this->tenodes.size(); i ++)
	{
		teNode* _node = this->tenodes[i];
		this->te2lock[_node] = 0;
	}
#ifdef DEBUG_TRACK
	util::track(" Out init te2lock");
#endif
}

void msforest::pessimistic_apply(OPlist* _oplist)
{
#ifdef DEBUG_TRACK
	util::track(" In pessim_apply oplist");
#endif
	pthread_mutex_lock(&(this->pessimistic_mutex));
#ifdef DEBUG_TRACK
	util::track(" lock p_apply_telist pessim");
#endif

	List<teNode> _s_telist;
	List<teNode> _x_telist;

	_oplist->reset();
	while(_oplist->hasnext())
	{
		nodeOP* _nodeop = _oplist->next();	
		if(_nodeop->is_write())
			_x_telist.add(_nodeop->onode);
		else
			_s_telist.add(_nodeop->onode);
	}

	while(! this->is_apply(_s_telist, 'S') || ! this->is_apply(_x_telist, 'X') )
	{
		pthread_cond_wait(&(this->pessimistic_cond), &(this->pessimistic_mutex));	
	}

	this->set_apply(_s_telist, 'S');
	this->set_apply(_x_telist, 'X');

	pthread_mutex_unlock(&(this->pessimistic_mutex));
#ifdef DEBUG_TRACK
	util::track(" Out pessim_apply oplist");
#endif
}

/* for deletion */
void msforest::pessimistic_apply(List<teNode>& _telist)
{
#ifdef DEBUG_TRACK
	util::track(" In pessim_apply telist");
#endif
	pthread_mutex_lock(&(this->pessimistic_mutex));
#ifdef DEBUG_TRACK
	util::track(" lock p_apply_telist pessim");
#endif

	while(!this->is_apply(_telist, 'X'))
	{
		pthread_cond_wait(&(this->pessimistic_cond), &(this->pessimistic_mutex));	
	}

	this->set_apply(_telist, 'X');

	pthread_mutex_unlock(&(this->pessimistic_mutex));
#ifdef DEBUG_TRACK
	util::track(" Out pessim_apply telist");
#endif
}

/*should be called after pessimistic_mutex is locked*/	
bool msforest::is_apply(List<teNode>& _telist, char _locktype)
{
#ifdef DEBUG_TRACK
	util::track(" In is_apply");
#endif
	_telist.reset();
	while(_telist.hasnext())
	{
		int _ilock = this->te2lock[_telist.next()];
		if(_locktype == 'S' && _ilock == -1)
		{
			return false;
		}
		else
		if(_locktype == 'X' && _ilock != 0)
		{
			return false;
		}
		else
		if(_locktype != 'S' && _locktype != 'X')
		{
			cerr << "Neither S nor X" << endl;
			exit(-1);
		}
	}
#ifdef DEBUG_TRACK
	util::track(" Out is_apply ");
#endif

	return true;
}
	
bool msforest::set_apply(List<teNode>& _telist, char _locktype)
{
	_telist.reset();
	while(_telist.hasnext())
	{
		teNode* _node = _telist.next();
		int _ilock = this->te2lock[_node];
		if(_locktype == 'S')
		{
			if(_ilock == -1){
				cerr << "ilock -1 not S" << endl;
				exit(-1);
			}

			this->te2lock[_node] ++;
		}
		else
		if(_locktype == 'X')
		{
			if(_ilock != 0){
				cerr << "ilock != 0 not X" << endl;
				exit(-1);
			}

			this->te2lock[_node] = -1;
		}
		else
		if(_locktype != 'S' && _locktype != 'X')
		{
			cerr << " @set_apply Neither S nor X" << endl;
			exit(-1);
		}
	}	

	return true;
}

void msforest::reset_te2lock(List<lockReq>* _lrlist)
{
#ifdef DEBUG_TRACK
	util::track("In reset lrlist");
#endif

	List<teNode> _telist;
	_lrlist->reset();
	while(_lrlist->hasnext())
	{
		_telist.add(_lrlist->next()->op->onode);
	}
	this->reset_te2lock(_telist);

#ifdef DEBUG_TRACK
	util::track(" Out reset telist");
#endif
}

void msforest::reset_te2lock(List<teNode>& _telist)
{
#ifdef DEBUG_TRACK
	util::track("In reset telist");
#endif
	pthread_mutex_lock(&(this->pessimistic_mutex));
#ifdef DEBUG_TRACK
	util::track("\tlock pessim");
#endif

	_telist.reset();
	while(_telist.hasnext())
	{
		teNode* _node = _telist.next();
		/* Xlock */
		if(te2lock[_node] == -1){
			te2lock[_node] = 0;
		}
		else	/* Slock */
		if(te2lock[_node] > 0){
			te2lock[_node] --;
		}
		else
		{
			cerr << "empty te2lock[_node]" << endl;
		}
	}

	pthread_cond_signal(&(this->pessimistic_cond));
	pthread_mutex_unlock(&(this->pessimistic_mutex));
#ifdef DEBUG_TRACK
	util::track("Out reset telist");
#endif
}
	
#endif
