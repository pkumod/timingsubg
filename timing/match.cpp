#include "match.h"


match::match(qEdge* _qe, dEdge* _e)
{
	pairs.clear();
	pairs[_qe] = _e;
}

match::match(match& _m1, match& _m2)
{
	this->merge(_m1, _m2);
}
	
match::match(match* _m1, match* _m2)
{
	this->merge(*_m1, *_m2);
}

match::match(){
	this->pairs.clear();
}
	
match::match(const match& _m)
{
	this->pairs = _m.pairs;
}
	
void match::merge(match& _m1, match& _m2)
{
	this->pairs = _m1.pairs;
	this->pairs.insert(_m2.pairs.begin(), _m2.pairs.end());
	return;
}
	
match& match::operator=(const match& _m)
{
	this->pairs = _m.pairs;
	return *this;
}

qdMap* match::get_q2dM()
{
	return &(this->pairs);
}

bool match::add(qEdge* _qe, dEdge* _e){
	this->pairs[_qe] = _e;
	return true;
}

bool match::add(match* _mat){
	qdMap* _qdmap = _mat->get_pairs();	

#ifdef DEBUG_TRACK
	if(this == _mat){
		cout << "**************err: match the same" << endl;
	}
#endif

	this->pairs.insert(_qdmap->begin(), _qdmap->end());	
	return true;
}
	
bool match::remove(qEdge* _qe)
{
	this->pairs.erase(_qe);
	return true;
}

bool match::is_edge(dEdge * _e)
{
	if(this->pairs.size() == 0){
		cout << "err size 0" << endl;
		int i;
		cin >> i;
	}
	return (this->pairs.begin())->second == _e;
}
	
bool match::contain(dEdge * _e)
{
	if(this->pairs.size() == 0){
		cout << "err size 0" << endl;
		int i;
		cin >> i;
	}

	qdMap::iterator itr = this->pairs.begin();
	while(itr != this->pairs.end())
	{
		if(itr->second == _e) return true;
		itr ++;
	}

	return false;
}
	
bool match::contain(qEdge* _qe, dEdge * _e)
{
	if(this->pairs.find(_qe) != this->pairs.end())
	{
		return this->pairs[_qe] == _e;
	}

	return false;
}

int match::size()
{
	return this->pairs.size();
}
	
int match::to_size()
{
	return this->size()*(sizeof(dEdge*)+sizeof(qEdge*));
}

string match::to_str() const{
	stringstream _ss;
	qdMap::const_iterator qditr = this->pairs.begin();
	while(qditr != this->pairs.end())
	{
		_ss << "[q(" << qditr->first->s << "," << qditr->first->t << "," << qditr->first->id << "), ";
		_ss << "(" << qditr->second->to_str() << ")"  << "]; ";
		qditr ++;
	}
	return _ss.str();
} 

string match::mat_query_str() const
{   
	stringstream _ss;
	qdMap::const_iterator qditr = this->pairs.begin();
	while(qditr != this->pairs.end())
	{
		_ss << "(" << qditr->first->s << "," << qditr->first->t << ")\t | \t";
		qditr ++;
	}
	return _ss.str();
}
string match::mat_data_str() const
{
	stringstream _ss;
	qdMap::const_iterator qditr = this->pairs.begin();
	while(qditr != this->pairs.end())
	{
		_ss << "(" << qditr->second->to_str() << ") | ";
		qditr ++;
	}
	return _ss.str();
}

map<qEdge*, dEdge*>* match::get_pairs(){
	return &this->pairs;
}
