#include "RdfQedge.h"
#include "RdfDedge.h"

rdfQedge::rdfQedge(int _s, int _t) : qEdge(_s, _t)
{
	this->literal = "NULL";	
}

rdfQedge::rdfQedge(string _str_e) : qEdge(-1, -1)
{
	if(_str_e == ""){
	
	}
}

rdfQedge::~rdfQedge()
{

}

rdfQedge::rdfQedge(int _s, int _t, string _stype, string _otype, string _pre, string _literal)
	: qEdge(_s, _t), stype(_stype), otype(_otype), pre(_pre), literal(_literal)
{
	
}

bool rdfQedge::is_before(qEdge* _q)
{
	return (_q)->preEdges.find(this) != _q->preEdges.end();
}

bool rdfQedge::is_after(qEdge* _q)
{
	return this->preEdges.find(_q) != this->preEdges.end();
}

bool rdfQedge::is_match(dEdge* _d)
{
	return ((rdfDedge*)_d)->is_match( (qEdge*)this );

}

string rdfQedge::to_str()
{
	stringstream _ss;
	_ss << this->stype << this->s << " " << this->otype << this->t << " ";
	_ss << this->pre << " " << this->literal;
	//_ss << s_port << " " << t_port;
	//_ss << " " << size;
	return _ss.str();
}
	
string rdfQedge::label()
{
    return this->stype+"_"+this->pre+"_"+this->otype;
}
