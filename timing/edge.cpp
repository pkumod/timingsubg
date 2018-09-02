#include "edge.h"

Edge::Edge(int _s, int _t):	s(_s),t(_t)
{
		
}


bool Edge::ss(Edge* _e){
	if(this->s == _e->s) return true;
	return false;
}

bool Edge::tt(Edge* _e){
	if(this->t == _e->t) return true;
	return false;
}

bool Edge::st(Edge* _e){
	if(this->s == _e->t) return true;
	return false;

}

bool Edge::ts(Edge* _e){
	if(this->t == _e->s) return true;
	return false;
}

bool Edge::adj(Edge* _e)
{
	return this->ss(_e) || this->st(_e) 
        	 || this->tt(_e) || this->ts(_e);
}

dEdge::dEdge(int _s, int _t): Edge( _s,  _t)
{

}

dEdge::~dEdge(){

}

string dEdge::to_str(){
	return "false";
}
bool dEdge::is_same(dEdge* _e){return false;}
bool dEdge::is_before(dEdge* _e){return false;}
bool dEdge::is_after(dEdge* _e){return false;}
bool dEdge::is_match(qEdge* _q){return false;}

qEdge::qEdge(int _s, int _t): Edge( _s,  _t)
{
	this->preEdges.clear();
}

qEdge::~qEdge()
{

}
	
bool qEdge::fill_pre()
{
	set<qEdge*>::iterator itr = this->preEdges.begin();
	set<qEdge*> _tmpset;
	while(itr != this->preEdges.end())
	{
		(*itr)->fill_pre();
		_tmpset.insert((*itr)->preEdges.begin(), (*itr)->preEdges.end());
		++ itr;
	}		

	this->preEdges.insert(_tmpset.begin(), _tmpset.end());

	return true;
}
	
string qEdge::pre_str()
{
	stringstream _ss;
	set<qEdge*>::iterator itr = this->preEdges.begin();
	while(itr != this->preEdges.end())
	{
		_ss << "\t\t" << (*itr)->to_str() << endl;
		++ itr;
	}
	return _ss.str();
}

bool qEdge::add_preedge(qEdge* _qe)
{
	this->preEdges.insert(_qe);
	return true;
}

string qEdge::to_str(){return "false";}
string qEdge::label(){return "false";};
bool qEdge::is_before(qEdge* _q){return false;}
bool qEdge::is_after(qEdge* _q){return false;}
bool qEdge::is_match(dEdge* _d){return false;}
