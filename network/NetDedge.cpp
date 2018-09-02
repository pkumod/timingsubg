#include "NetDedge.h"
#include "NetQedge.h"

netDedge::netDedge(int _s, int _t) : dEdge(_s, _t)
{
	
}

netDedge::~netDedge(){

}

netDedge::netDedge(string _str_e) : dEdge(-1, -1)
{
		stringstream _ss(_str_e);
		_ss >> this->t_sec >> this->t_msec;
		_ss >> this->s >> this->t;
		{
			char _prot[100];
			_ss >> _prot;
			this->prot = _prot;
		}
		_ss >> this->s_port;
		_ss >> this->t_port;
}

bool netDedge::is_same(dEdge* _d)
{
	netDedge* _nd = (netDedge*)_d;
	if(this == _nd) return true;
	
	if(this->s != _nd->s) return false;
	if(this->t != _nd->t) return false;
	if(this->s_port != _nd->s_port) return false;
	if(this->t_port != _nd->t_port) return false;
	if(this->size != _nd->size) return false;
	if(this->prot != _nd->prot) return false;
	if(this->t_sec != _nd->t_sec) return false;
	if(this->t_msec != _nd->t_msec) return false;
	
	return true;
}

bool netDedge::is_before(dEdge* _d)
{
	netDedge* _nd = (netDedge*)_d;
	if(this->t_sec < _nd->t_sec) return true;
	if(this->t_sec > _nd->t_sec) return false;

	return this->t_msec < _nd->t_msec;
}

bool netDedge::is_after(dEdge* _d)
{
	return ((netDedge*)_d)->is_before( (dEdge*)this );
}

bool netDedge::is_match(qEdge* _q)
{
	netQedge* _nq = (netQedge*)_q;
	if(_nq->prot == "HTTP")
	{
		if(this->prot == "HTTP")
			return true;

		if(this->prot == "TCP" && 
			(this->t_port == 80 || this->t_port == 443))
		{
			return true;
		}
		else
			return false;
	}
	else
	if(this->prot != _nq->prot){
		return false;
	}
	else
	if(_nq->s_port != -1 && _nq->s_port != this->s_port)
	{
		return false;
	}
	else
	if(_nq->t_port != -1 && _nq->t_port != this->t_port)
	{
		return false;
	}

	return true;
}

string netDedge::to_str()
{
	stringstream _ss;
	_ss << t_sec << "-" << t_msec;
	_ss << " " << s << " " << t << " ";
	_ss << prot << " ";
	_ss << s_port << " " << t_port;
	//_ss << " " << size;
	return _ss.str();
}
