#include "teNode.h"
#include "lockReq.h"
#include "nodeOP.h"
#include "../util/util.h"

teNode::teNode(qEdge* _qedge, bool _leftmost){
	this->subquery.clear();
	this->subquery.push_back(_qedge);
	this->sibling = NULL;
	this->father = NULL;
	this->left_child = NULL;
	this->right_child = NULL;

	pthread_mutex_init(&(this->te_mutex), NULL);
	pthread_mutex_init(&(this->queue_mutex), NULL);

	this->flag_leftmost = _leftmost;
	this->flag_tc_or_upper = false;
	this->head_mslist = new msNode(NULL, NULL, NULL);

	this->share_num = 0;
}

teNode::teNode(teNode* _left, teNode* _right){
	this->set_children(_left, _right);
	this->subquery.clear();
	vector<qEdge*>* _subq = NULL;
	_subq = _left->get_subq();
	this->subquery.insert(this->subquery.end(), _subq->begin(), _subq->end());
	_subq = _right->get_subq();
	this->subquery.insert(this->subquery.end(), _subq->begin(), _subq->end());

	this->sibling = NULL;
	this->father = NULL;
	_left->set_sibling(_right);
	_right->set_sibling(_left);
	_left->set_father(this);
	_right->set_father(this);

	pthread_mutex_init(&(this->te_mutex), NULL);
	pthread_mutex_init(&(this->queue_mutex), NULL);
	
	this->flag_leftmost = false;
	this->flag_tc_or_upper = false;
	this->head_mslist = new msNode(NULL, NULL, NULL);

	this->share_num = 0;
}

teNode::~teNode(){
#ifdef DESTRUCT_LOG
	cout << "IN destruct teNode: " << this->to_querystr() << endl;
#endif
	msNode* _cur = this->head_mslist;
	msNode* _prev;
	while(_cur != NULL){
		_prev = _cur;
		_cur = _cur->next;
		delete _prev;
	}

	pthread_mutex_destroy(&(this->queue_mutex));
	pthread_mutex_destroy(&(this->te_mutex));

#ifdef DESTRUCT_LOG
	cout << "OUT destruct teNode" << endl;
#endif
}

bool teNode::is_root(){
	
	return this->father == NULL;
}

bool teNode::is_leaf(){
	return this->left_child == NULL && this->right_child == NULL;
}

bool teNode::is_leftmost(){
	return this->flag_leftmost;
}
bool teNode::has_sibling(){
	return this->sibling != NULL;
}

bool teNode::at_left(){
	if(this->father == NULL) return false;
	return this->father->get_left() == this;
}

bool teNode::at_right(){
	if(this->father == NULL) return false;
	return this->father->get_right() == this;
}

bool teNode::is_TCnode_or_upper(){
	return this->flag_tc_or_upper;
}

void teNode::setTC_or_upper(bool _flag){
	this->flag_tc_or_upper = _flag;
}

teNode*  teNode::get_sibling(){
	return this->sibling;
}

void teNode::set_sibling(teNode* _sibling){
	this->sibling = _sibling;
}

void teNode::set_children(teNode* _left, teNode* _right){
	this->left_child = _left;
	this->right_child = _right;
}

teNode* teNode::get_left(){
	return this->left_child;
}

teNode* teNode::get_right(){
	return this->right_child;
}

void teNode::set_father(teNode* _father){
	this->father = _father;
}

teNode*  teNode::get_father(){
	return this->father;
}

vector<qEdge*>* teNode::get_subq(){
	return &(this->subquery);
}

bool teNode::get_mslist(List<msNode>& _mslist){
	if(this->head_mslist->next == NULL){
		return false;;
	}

	_mslist.clear();
	msNode* _cur = this->head_mslist->next;
	while(_cur != NULL)
	{
		_mslist.add(_cur);
		_cur = _cur->next;
	}
	return true;
}

bool  teNode::append_lockreq(lockReq* _lr){
#ifndef NO_THREAD
#ifdef DEBUG_TRACK
	util::track("append lr= "+ _lr->to_str() +" @ node "+this->to_str());
#endif
	pthread_mutex_lock(&(this->queue_mutex));

	this->lock_req_queue.push(_lr);

	pthread_mutex_unlock(&(this->queue_mutex));
#ifdef DEBUG_TRACK
	util::track("finish lr @ node "+this->to_str());
#endif
#endif

	return true;
}


bool teNode::pop_lockreq(){
#ifndef NO_THREAD
#ifdef DEBUG_TRACK
	util::track("\t\tpop lr @ node "+this->to_str());
#endif
	pthread_mutex_lock(&(this->queue_mutex));
	if(this->lock_req_queue.empty()){
		cout << "queue empty" << endl;
		exit(0);
	}
	this->lock_req_queue.pop();
	this->signal_next_nolock();

	pthread_mutex_unlock(&(this->queue_mutex));
#ifdef DEBUG_TRACK
	util::track("\t\tfinish pop lr @ node "+this->to_str());
#endif
#endif
	return true;
}

bool teNode::signal_next(){

#ifdef DEBUG_TRACK
	util::track("\t\tsignal_next @ node "+this->to_str());
#endif
	pthread_mutex_lock(&(this->queue_mutex));
	this->signal_next_nolock();
	pthread_mutex_unlock(&(this->queue_mutex));

#ifdef DEBUG_TRACK
	util::track("\t\tfinish signal_next @ node "+this->to_str());
#endif
	return true;
}
bool teNode::signal_next_nolock(){
#ifdef DEBUG_TRACK
	util::track("\t\tsignal_next_nolock @ node "+this->to_str());
	stringstream _ss;
	_ss << "cur front is : ";
#endif
	if(! this->lock_req_queue.empty())
	{
#ifdef DEBUG_TRACK
		_ss << *(this->lock_req_queue.front()->tid);
		_ss << " condptr=" << &(this->lock_req_queue.front()->t_cond) << endl;
#endif
		pthread_cond_signal(&(this->lock_req_queue.front()->t_cond));
	}
	else{
#ifdef DEBUG_TRACK
		_ss << "NULL"  << endl;
#endif
	
	}
#ifdef DEBUG_TRACK
	util::track(_ss);
	util::track("\t\tfinish signal_next_nolock @ node "+this->to_str());
#endif
	
	return true;
}

bool teNode::is_cur_head(pthread_t& _tid){

#ifdef DEBUG_TRACK
	util::track("\t\tcheck head @ node "+this->to_str());
#endif
	pthread_mutex_lock(&(this->queue_mutex));

	bool _is = ( 0 != pthread_equal(_tid, *(lock_req_queue.front()->tid)) );
	
	pthread_mutex_unlock(&(this->queue_mutex));
#ifdef DEBUG_TRACK
	{
		stringstream _ss;
		_ss << "\t\tfinish head with " << _is << " @ node "+this->to_str() << endl;
		util::track(_ss);
	}
#endif

	return _is;
}

/* can only be used in locked env */
/*
lockReq* teNode::lr_head(){
	
	if(this->lock_req_queue.empty())
	{
		return NULL;
	}

	return this->lock_req_queue.front();
}
*/

bool teNode::S_lock(lockReq* _lr){
#ifndef NO_THREAD

#ifdef DEBUG_TRACK
	util::track("slock @ node "+this->to_str());
#endif
	pthread_mutex_lock(&(this->te_mutex));

	while(! this->is_cur_head( *(_lr->tid)) )
	{
		pthread_cond_wait(&(_lr->t_cond), &(this->te_mutex));
	}

	/* when here the lock status must be non-exclusive */
	if(this->share_num < 0){
		cout << "share num err: " << this->to_str() << endl;
		exit(-1);
	}

	this->share_num ++;
	this->pop_lockreq();

	pthread_mutex_unlock(&(this->te_mutex));
#ifdef DEBUG_TRACK
	util::track("finish slock @ node "+this->to_str());
#endif

#endif

	return true;
}

bool teNode::S_release(lockReq* _lr){	
#ifndef NO_THREAD

#ifdef DEBUG_TRACK
	util::track("\n-----S_release @ node "+this->to_str());
#endif
	pthread_mutex_lock(&(this->te_mutex));
	
	this->share_num --;
	if(this->share_num == 0)
	{
		this->signal_next();
	}

	pthread_mutex_unlock(&(this->te_mutex));
#ifdef DEBUG_TRACK
	util::track("-----finish S_release @ node "+this->to_str());
#endif

#endif
	return true;
}

bool teNode::X_lock(lockReq* _lr){
#ifndef NO_THREAD

#ifdef DEBUG_TRACK
	if(_lr == NULL){
		util::track("_lr NULL@"+this->to_str());
	}
	else
	if(_lr->op == NULL){
		util::track("_lr->op NULL@"+this->to_str()+" (normal when for delete)");
	}
	else
	if(_lr->op->onode != this)
	{
		stringstream _ss;
		_ss << "err : lr not right" << endl;
		_ss << "\tlr: " << _lr->op->onode->to_str() << endl;
		_ss << "\tthis: " << this->to_str() << endl;
		cout << _ss;	
		util::track(_ss);
		exit(-1);
	}
	util::track("\n-----xlock "+ _lr->to_str() +  " @ node "+this->to_str());
#endif
	pthread_mutex_lock(&(this->te_mutex));
	
	while(! this->is_cur_head( *(_lr->tid) ) )
	{
		pthread_cond_wait(&(_lr->t_cond), &(this->te_mutex));
#ifdef DEBUG_TRACK
		{
			stringstream _ss;
			_ss << pthread_self() << " wake up" << endl;
			util::track(_ss);
		}
#endif
	}

	while(this->share_num != 0)
	{
		pthread_cond_wait(&(_lr->t_cond), &(this->te_mutex));
	}

	this->share_num = -1;

#ifdef DEBUG_TRACK
	util::track("\n-----finish xlock @ node "+this->to_str());
#endif

#endif /* endif for NO_THREAD */
	return true;
}

bool teNode::X_release(lockReq* _lr){
#ifndef NO_THREAD

#ifdef DEBUG_TRACK
	util::track("\n-----X_release @ node "+this->to_str());
#endif
	this->share_num = 0;
	this->pop_lockreq();

	pthread_mutex_unlock(&(this->te_mutex));
#ifdef DEBUG_TRACK
	util::track("\n-----finish X_release @ node "+this->to_str());
#endif

#endif
	return true;
}

/* 
 * 
 * */
msNode* teNode::remove_edge(dEdge* _e, lockReq* _lr){
#ifdef DEBUG_TRACK
	{
		util::track("In remove_edge@"+this->to_str(), "\n");
	}
#endif
	this->X_lock(_lr);
	msNode* _ret_mslist = NULL;
	msNode* _tail = NULL;
	msNode* _next = NULL;
	msNode* _cur_ms = this->head_mslist->next;

#ifdef DEBUG_TRACK
	{
		stringstream _ss;
		_ss << "rm: " <<  _e->to_str() << endl;
		_ss << "from tenode " << this->to_str() << endl;
		_ss << this->to_matches_str() << endl;
		util::track(_ss);
	}
#endif

	while(_cur_ms != NULL)
	{
	/*	
		if(_cur_ms->father != this->head_mslist->father)
		{
			break;
		}
	*/	
	
		if(_cur_ms->is_dedge(_e))
		{
			_next = this->remove_ms(_cur_ms);
			this->add_rmlist(_ret_mslist, _tail, _cur_ms);
			_cur_ms = _next;
		}
		else
		{
			_cur_ms = _cur_ms->next;	
		}
		
	}

	this->X_release(_lr);

	return _ret_mslist;
}
	
msNode* teNode::remove_match(set<match*>& _mset)
{
#ifdef DEBUG_TRACK
	{
		util::track("IN remove_match(set)");
	}
#endif
	msNode* _ret_mslist = NULL;
	msNode* _tail = NULL;
	msNode* _next = NULL;
	msNode* _cur_ms = this->head_mslist->next;
	while(_cur_ms != NULL)
	{
		/*
		if(_cur_ms->father != this->head_mslist->next->father)
		{
			break;
		}
		*/

		if(_mset.find(_cur_ms->mat) != _mset.end())
		{
			_next = this->remove_ms(_cur_ms);
			this->add_rmlist(_ret_mslist, _tail, _cur_ms);
			/* several days debug for lack of this IFs ! */
			if(_cur_ms->father != NULL)
			{
				if(_cur_ms->father->child_first == _cur_ms)
				{
					/* assign new child_first for _cur_ms->father  */
					if(_next != NULL)
					{
						if(_next->father == _cur_ms->father)
						{
							_cur_ms->father->child_first = _next;	
						}
						else
							_cur_ms->father->child_first = NULL;
					}
					else
					{
						_cur_ms->father->child_first = NULL;
					}
				}
			}
			_cur_ms = _next;
			
		}
		else
		{
			_cur_ms = _cur_ms->next;	
		}
	}

	return _ret_mslist;
}

/* remove _ms and return _ms->next */
msNode* teNode::remove_ms(msNode* _cur_ms)
{
#ifdef MARK_DEL
	_cur_ms->mark_del = true;
#endif

#ifdef DEBUG_TRACK
	{
		stringstream _ss;
		_ss << "rm ms:" << _cur_ms->whole_match_str() << endl;
		util::track(_ss);
	}
#endif

	msNode* _next = _cur_ms->next;
	_cur_ms->prev->next = _next;
	if(_next != NULL)
		_next->prev = _cur_ms->prev;

	_cur_ms->next = NULL;
	_cur_ms->prev = NULL;
#ifdef DEBUG_TRACK
	{
		stringstream _ss;
		if(_next != NULL)
		{
			_ss << "\t_next=" << _next->mat->to_str() << endl;
			if(_next->prev != NULL)
			{
				if(_next->prev == this->head_mslist)
					_ss << "\tprev is head" << endl;
				else
					_ss << "\tprev=" << _next->prev->mat->to_str() << endl;
			}
			else
			{
				_ss << "\tprev= NULL" <<  endl;
			}
		}
		else{
			_ss << "\t _next= NULL" << endl; 
		}

		util::track(_ss);
	}
#endif
	return _next;
}

/*
msNode* teNode::remove_match(List<match>* _mslist)
{
	if(_mslist != NULL){
		cout << "call wrong func" << endl;
		exit(-1);
	}
	return NULL;
}
*/
	
bool teNode::add_rmlist(msNode* & _ret_mslist, msNode* & _tail, msNode* _cur_ms){
	if(_ret_mslist == NULL)
	{
		_ret_mslist = _cur_ms;
		_ret_mslist->next = NULL;
		_ret_mslist->prev = NULL;
		_tail = _ret_mslist;
	}
	else
	{
		_cur_ms->prev = _tail;
		_cur_ms->next = NULL;
		_tail->next = _cur_ms;
		_tail = _tail->next;
	}

	return true;
}

/* head_mslist contains no data, initialed as new msNode()*/
bool teNode::add_msnodes(List<msNode>* _mlist){
	_mlist->reset();
	while(_mlist->hasnext())
	{
		msNode* _m = _mlist->next();
		if(this->head_mslist->next != NULL){
			this->head_mslist->next->prev = _m;	
		}

		_m->next = this->head_mslist->next;
		this->head_mslist->next = _m;
		_m->prev = this->head_mslist;
	}
	return true;
}

bool teNode::add_msnodes(msNode* _mlist){
	if(_mlist == NULL){
#ifdef DEBUG_TRACK
		util::track("_mlist is NULL@add_msnodes");
#endif
		return false;
	}

	if(this->head_mslist->next == NULL){
		this->head_mslist->next = _mlist;
		_mlist->prev = this->head_mslist;
		return true;
	}
	
	msNode* _tail = _mlist;
	while(_tail->next != NULL) _tail = _tail->next;

	_tail->next = this->head_mslist->next;
	_tail->next->prev = _tail;
	this->head_mslist->next = _mlist;
	_mlist->prev = this->head_mslist;

	return true;
}

/* when _is_level_mat==true
 * results in _mlist may be duplicated match pointers 
 * thus, _is_level_mat==true is only used for to_matches_str() case
 * */
void teNode::get_all_matches(List<match>* _mlist, bool _is_level_mat){
	msNode* _cur_ms = this->head_mslist->next;
	while(_cur_ms != NULL)
	{
		if(_is_level_mat)
		{
			_mlist->add(_cur_ms->mat);	
		}
		else
		{
			_mlist->add(_cur_ms->get_whole_match());	
		}
		_cur_ms = _cur_ms->next;
	}
}

void teNode::build_matlist(List<match>& _matlist, msNode* _mlist_head){
	_matlist.clear();
	msNode* _cur = _mlist_head;
	while(_cur != NULL)
	{
		_matlist.add(_cur->get_whole_match());
		_cur = _cur->next;
	}
}

string teNode::to_matches_str(bool _is_level_mat){
	List<match> _mlist;
	this->get_all_matches(&_mlist, _is_level_mat);
	
	stringstream _ss;
	_ss << "\tThere are " << _mlist.size() << " matches of " << this->to_str() << " as following:" << endl;

	_mlist.reset();
	while(_mlist.hasnext())
	{
		_ss << "\t\t" << _mlist.next()->to_str() << endl;
	}

	return _ss.str();
}
	
string teNode::to_answer_str()
{
    if(! this->is_root()){
        cerr << "NOt root for answer" << endl;
        exit(-1);
    }
   
#ifndef NO_THREAD
    pthread_mutex_lock(&(this->te_mutex));
#endif

	List<match> _mlist;
	this->get_all_matches(&_mlist);
#ifndef NO_THREAD
    pthread_mutex_unlock(&(this->te_mutex));
#endif
    if(_mlist.empty())
    {
        return "\tThere is no answers";
    }

	stringstream _ss;
	_ss << "\tThere are " << _mlist.size() << " matches of " << this->to_str() << " as following:" << endl;


	_mlist.reset();
    match* _tmpmat = _mlist.next();
    _ss << "\t\t" << _tmpmat->mat_query_str() << endl;
    _ss << "\t\t" << _tmpmat->mat_data_str() << endl;
	while(_mlist.hasnext())
	{
		_ss << "\t\t" << _mlist.next()->mat_data_str() << endl;
	}
    return _ss.str();
}

string teNode::to_str(){
	stringstream _ss;
	_ss << this->to_querystr();
#ifdef QUERYSTR
	return _ss.str();
#endif

	if(this->left_child != NULL){
		_ss << " left" << this->left_child->to_querystr();
	}else{
		_ss << " left:NULL";
	}

	if(this->right_child != NULL){
		_ss << " right" << this->right_child->to_querystr();
	}else{
		_ss << " right:NULL";
	}

	if(this->sibling != NULL){
		_ss << " sibling" << this->sibling->to_querystr();
	}else{
		_ss << " sibling:NULL";
	}

	if(this->father != NULL){
		_ss << " father" << this->father->to_querystr();
	}else{
		_ss << " father:NULL";
	}

	return _ss.str();
}

string teNode::to_querystr()
{
	stringstream _ss;

#ifdef DEBUG_TRACK
	{
		stringstream _tmpss;
		_tmpss << "size of subq = " << this->subquery.size() << endl;
		if(this->subquery[0] == NULL){
			_tmpss << "NULL err subq" << endl;
		}
		if(this->subquery.size()>1){
			_tmpss << "second null: " << (this->subquery[1] == NULL) << endl;
		}
//		util::track(_tmpss);
	}
#endif

	_ss << "[(" << this->subquery[0]->to_str() << ")";
	for(int i = 1; i< (int)this->subquery.size(); i ++){
		_ss << ", (" << this->subquery[i]->to_str() << ")";
	}
	_ss << "]";
	
	return _ss.str();
}
	
string teNode::to_spacestr()
{
	stringstream _ss;
	_ss << "sz=" << this->to_size() << ", num_mat=" << this->num_match();
	_ss << ", noms=" << this->no_ms_size() << ", gap=" << this->no_ms_size()-this->to_size();
	return _ss.str();
}
	
long long int teNode::to_size()
{
	long long int _sz = 0;
	_sz += sizeof(teNode);
	
	msNode* _cur = this->head_mslist->next;
	while(_cur != NULL)
	{
		_sz += _cur->to_size();
		_cur = _cur->next;
	}

	return _sz;
}
	
long long int teNode::no_ms_size()
{
#ifdef SPACE_LOG
	cout << "no_ms@teNode" << this->to_str() << endl;
#endif

	long long int _sz = 0;
	_sz += sizeof(teNode);
	
	msNode* _cur = this->head_mslist->next;
	while(_cur != NULL)
	{
		_sz += _cur->no_ms_size();
		_cur = _cur->next;
	}

	return _sz;

}
	
int teNode::num_match()
{
	int _num = 0;
	
	msNode* _cur = this->head_mslist->next;
	while(_cur != NULL)
	{
		_num ++;	
		_cur = _cur->next;
	}

	return _num;
}
