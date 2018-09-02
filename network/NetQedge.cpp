#include "NetQedge.h"
#include "NetDedge.h"

netQedge::netQedge(int _s, int _t) : qEdge(_s, _t)
{
	
}

netQedge::netQedge(string _str_e) : qEdge(-1, -1)
{
	if(_str_e == ""){
	
	}
}

netQedge::~netQedge()
{

}

netQedge::netQedge(int _s, int _t, int _sp, int _tp, int _sz, string _prot)
	: qEdge(_s, _t), s_port(_sp), t_port(_tp), size(_sz), prot(_prot)
{
	
}

bool netQedge::is_before(qEdge* _q)
{
	return (_q)->preEdges.find(this) != _q->preEdges.end();
}

bool netQedge::is_after(qEdge* _q)
{
	return this->preEdges.find(_q) != this->preEdges.end();
}

bool netQedge::is_match(dEdge* _d)
{
	return ((netDedge*)_d)->is_match( (qEdge*)this );

}

string netQedge::to_str()
{
	stringstream _ss;
	_ss << s << " " << t << " ";
	_ss << prot << " " << this->id;
	//_ss << s_port << " " << t_port;
	//_ss << " " << size;
	return _ss.str();
}
	
string netQedge::label()
{
    stringstream _ss;
    _ss << prot << ":" << this->t_port;
    return _ss.str();
}
