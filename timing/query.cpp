#include "query.h"
#include "match.h"

query::query(string _query_path){
	this->query_path = _query_path;
	string _file = this->name_str();
	int _qsz = 0;
	int _qno = 0;
	double _p = 0;
	int _c = 0;
	for(int i = 0; i < (int)_file.size(); i ++)
	{
		if(_file[i] == '_') _c ++;
	}

	if(_c >= 2)
	{
		sscanf(_file.c_str(), "%d_%d_%lf", &_qsz, &_qno, &_p);
		this->qNO = _qno;
		this->p = _p;
	}
}

query::~query(){
}

bool query::decompose(){
	
	return true;
}

bool query::parseQuery(){
	return false;
}

bool query::get_matches(dEdge* _e, vector<qEdge*>& _qedges)
{
	_qedges.clear();
	vector<qEdge*>::reverse_iterator ritr = this->left2right_leaf.rbegin();
	while(ritr != this->left2right_leaf.rend())
	{
		if(_e->is_match(*ritr)){
			_qedges.push_back(*ritr);
		}
		ritr ++;
	}

	return true;
}

vector<vector<qEdge*> >* query::getTCdecomp(){
	
	return &(this->TCdecomp);
}

bool query::compatible(match* _m1, match* _m2)
{
	map<qEdge*, dEdge*>* _qd1 = _m1->get_q2dM();
	map<qEdge*, dEdge*>* _qd2 = _m2->get_q2dM();
	map<qEdge*, dEdge*>::iterator _it1 = _qd1->begin();
	bool _tmp_bool;
	while(_it1 != _qd1->end())
	{		
		map<qEdge*, dEdge*>::iterator _it2 = _qd2->begin();
		while(_it2 != _qd2->end())
		{
			_tmp_bool = 
			  this->compatible(_it1->first, _it1->second, _it2->first, _it2->second);
			if(!_tmp_bool) return false;	

			++ _it2;
		}
		++ _it1;
	}
	return true;
}

bool query::compatible(match* _m, qEdge* _qe, dEdge* _e)
{
	map<qEdge*, dEdge*>* _qd = _m->get_q2dM();
	map<qEdge*, dEdge*>::iterator _it = _qd->begin();
	bool _tmp_bool;
	while(_it != _qd->end())
	{
		_tmp_bool = 
		this->compatible(_it->first, _it->second, _qe, _e);
		if(!_tmp_bool) return false;	

		++ _it;
	}
	return true;	
}

bool query::compatible(qEdge* _qe1, dEdge* _e1, qEdge* _qe2, dEdge* _e2)
{
#ifdef UNCOMP
	{
		stringstream _ss;
		_ss << _qe1->to_str() << "; " << _e1->to_str() << endl;
		_ss << _qe2->to_str() << "; " << _e2->to_str() << endl;
		util::track(_ss);	
	}
#endif

	/* source to source */
	if(_qe1->ss(_qe2))
	{
		if(! _e1->ss(_e2))
		{
#ifdef UNCOMP
			{
				stringstream _ss;
				_ss << "!ss" << endl;
				util::track(_ss);
			}
#endif

			return false;
		}
	}
	else
	{
		if(_e1->ss(_e2))
		{
#ifdef UNCOMP
			{
				stringstream _ss;
				_ss << "ss" << endl;
				util::track(_ss);
			}
#endif
			return false;
		}
	}
	/* target to target */
	if(_qe1->tt(_qe2))
	{
		if(! _e1->tt(_e2))
		{
#ifdef UNCOMP
			{
				stringstream _ss;
				_ss << "!tt" << endl;
				util::track(_ss);
			}
#endif
			return false;
		}
	}
	else
	{
		if(_e1->tt(_e2)) 
		{
#ifdef UNCOMP
			{
				stringstream _ss;
				_ss << "tt" << endl;
				util::track(_ss);
			}
#endif
			return false;
		}
	}
	/* source to target */
	if(_qe1->st(_qe2))
	{
		if(! _e1->st(_e2))
		{
#ifdef UNCOMP
			{
				stringstream _ss;
				_ss << "!st" << endl;
				util::track(_ss);
			}
#endif
			return false;
		}
	}
	else
	{
		if(_e1->st(_e2))
		{
#ifdef UNCOMP
			{
				stringstream _ss;
				_ss << "st" << endl;
				util::track(_ss);
			}
#endif
			return false;
		}
	}
	/* target to source */
	if(_qe1->ts(_qe2))
	{
		if(! _e1->ts(_e2))
		{
#ifdef UNCOMP
			{
				stringstream _ss;
				_ss << "!ts" << endl;
				util::track(_ss);
			}
#endif
			return false;
		}
	}
	else
	{
		if(_e1->ts(_e2))
		{
#ifdef UNCOMP
			{
				stringstream _ss;
				_ss << "ts" << endl;
				util::track(_ss);
			}
#endif
			return false;
		}
	}

	/* after */
	if(_qe1->is_after(_qe2))
	{
		if(_e1->is_after(_e2) == false)
		{
#ifdef UNCOMP
			{
				stringstream _ss;
				_ss << "!after" << endl;
				util::track(_ss);
			}
#endif
			return false;
		}
	}
	/* before */
	if(_qe1->is_before(_qe2))
	{
		if(_e1->is_before(_e2) == false)
		{
#ifdef UNCOMP
			{
				stringstream _ss;
				_ss << "!before" << endl;
				util::track(_ss);
			}
#endif
			return false;
		}
	}
/*
	if(_e1->is_same(_e2))
	{
#ifdef UNCOMP
			{
				stringstream _ss;
				_ss << "is_same" << endl;
				util::track(_ss);
			}
#endif
		return false;
	}
*/
	return true;
}

void query::fill_timeorder()
{
	for(int i = 0; i < this->size(); i ++)
	{
		this->left2right_leaf[i]->fill_pre();	
	}
	
}


string query::to_str(){
	stringstream _ss;
	_ss << "{";
	for(int i = 0; i < (int)this->TCdecomp.size(); i ++)
	{
		_ss << i << "[" << 0 << "(" << this->TCdecomp[i][0]->to_str() << ")";
		for(int j = 1; j < (int)this->TCdecomp[i].size(); j ++)
		{
			_ss << ", " << j << "(" << this->TCdecomp[i][j]->to_str() << ")";
		}
		_ss << "]; ";
	}
	_ss << "}";
	return _ss.str();
}
	
string query::timingorder_str()
{
	stringstream _ss;
	for(int i = 0; i < this->size(); i ++)
	{
		_ss << this->left2right_leaf[i]->to_str() << endl;
		_ss << this->left2right_leaf[i]->pre_str();	
	}
	return _ss.str();
}
	

int query::size(){
	return this->left2right_leaf.size();
}
	
string query::name_str()
{
	size_t _slash = this->query_path.rfind("/");

	string _file = "";
	if(_slash == string::npos)
	{
		_file = this->query_path;	
	}
	else
	{
		_file = this->query_path.substr(_slash+1);
	}
	
	return _file;
}
	

string query::param_str()
{
	stringstream _ss;
	_ss << this->size();
	_ss << "\t" << this->qNO;
	_ss << "\t" << this->p;
	return _ss.str();	
}
    
bool query::is_rightmost(qEdge* _qe)
{
    if(left2right_leaf.empty()) return false;

    return _qe == *(this->left2right_leaf.rbegin());
}
