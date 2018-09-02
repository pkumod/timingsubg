#include "gtransaction.h"
#include "../msforest/teNode.h"


gtransaction::gtransaction()
	: e(NULL), qe(NULL), tenode(NULL), G(NULL), M(NULL), Q(NULL)
{
	this->lr_list = new List<lockReq>(true);
	this->id = 1;
	this->initialized = false;
	this->count = 0;
	this->flag_exit = false;
}

gtransaction::~gtransaction()
{
	delete this->lr_list;
}

void gtransaction::set(dEdge* _e, qEdge* _qe, OPlist* _oplist, gstream* _g, msforest* _m, query* _q)
{
	e = (_e);
	qe = (_qe);
	G = (_g);
	M = (_m);
	Q = (_q);
	tenode = NULL;
	this->del_tenodes.clear();

	if(this->lr_list->empty())
	{/* for lrlist */
		/* dispatch lock requirements  */
		_oplist->reset();
		while(_oplist->hasnext())
		{
			nodeOP* _op = _oplist->next();
			lockReq* _lr = new lockReq(_op, &(this->id), -1);
			_op->onode->append_lockreq(_lr);
			this->lr_list->add(_lr);
		}
	}
	else
	{
		cout << "LR not empty" << endl;
		exit(-1);
	}
}

void gtransaction::del_set(dEdge* _e, teNode* _node, gstream* _g, msforest* _m, query* _q)
{
	e = (_e);
	G = (_g);
	M = (_m);
	Q = (_q);
	tenode = _node;
	qe = NULL;
	this->del_tenodes.clear();

	if(this->lr_list->empty())
	{/* for lrlist */
		/* dispatch lock requirements  */
		teNode* _cur_node = _node->get_father();
		if(_node->is_leftmost())
		{
			_cur_node = _node;	
		}

		while(_cur_node != NULL)
		{
			lockReq* _lr = new lockReq(NULL, &(this->id), -1);
			_cur_node->append_lockreq(_lr);
			this->del_tenodes.add(_cur_node);
			this->lr_list->add(_lr);
			_cur_node = _cur_node->get_father();
		}
	}
	else
	{
		cout << "LR not empty" << endl;
		exit(-1);
	}
}

bool gtransaction::thread_join()
{
#ifndef NO_THREAD
#ifdef DEBUG_TRACK
	{
		stringstream _ss;
		_ss << "DEBUG_TRACK " << this->id << " is to be thread_join with exit= " << this->flag_exit << endl;
		util::track(_ss);
	}
#endif
#endif

#ifndef NO_THREAD
#ifdef STD_MAIN
	{
		stringstream _ss;
		_ss << "STD_MAIN " << this->id << " is to be thread_join with exit= " << this->flag_exit << endl;
		cout << _ss.str() << endl;
	}
#endif
#endif

	if(this->initialized)
	{
#ifndef NO_THREAD
		pthread_join( this->id, NULL );
#endif
	}
	else
	{
		this->initialized = true;
	}
	this->lr_list->clear();

#ifdef DEBUG_TRACK
#ifndef NO_THREAD
	util::track("finish thread_join");
#endif
#endif

#ifdef STD_MAIN
#ifndef NO_THREAD
	cout << "finish thread_join with exit = " << this->flag_exit << endl;
#endif
#endif
	
	this->flag_exit = false;	

	return true;
}
