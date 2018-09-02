#include "RdfDedge.h"
#include "RdfQedge.h"

rdfDedge::rdfDedge(int _s, int _t) : dEdge(_s, _t)
{
	
}

rdfDedge::~rdfDedge(){

}

rdfDedge::rdfDedge(string _str_e) : dEdge(-1, -1)
{
		stringstream _ss(_str_e);
		char buf[200];
		/* stype */
		memset(buf, 0, sizeof(buf));
		_ss >> buf;
		this->stype = string(buf);
		/* s */
		_ss >> this->s;
		/* pred */
		memset(buf, 0, sizeof(buf));
		_ss >> buf;
		this->pre = string(buf);
		/* otype */
		memset(buf, 0, sizeof(buf));
		_ss >> buf;
		this->otype = string(buf);
		/* o(t) */
		_ss >> this->t;
		/* literal */
		memset(buf, 0, sizeof(buf));
		_ss >> buf;
		this->literal = string(buf);
		/* time */
		_ss >> this->t_sec;

}

bool rdfDedge::is_same(dEdge* _d)
{
	return false;
	rdfDedge* _nd = (rdfDedge*)_d;
	if(this == _nd) return true;
	
	if(this->s != _nd->s) return false;
	if(this->t != _nd->t) return false;
	if(this->pre != _nd->pre) return false;
	if(this->t_sec != _nd->t_sec) return false;
	if(this->stype != _nd->stype ) return false;
	if(this->otype != _nd->otype ) return false;
	if(this->literal != _nd->literal) return false;
	
	return true;
}

bool rdfDedge::is_before(dEdge* _d)
{
	rdfDedge* _nd = (rdfDedge*)_d;
	if(this->t_sec <= _nd->t_sec) return true;

	return false;
}

bool rdfDedge::is_after(dEdge* _d)
{
	return ((rdfDedge*)_d)->is_before( (dEdge*)this );
}

bool rdfDedge::is_match(qEdge* _q)
{
	rdfQedge* _rq = (rdfQedge*)_q;
	if(this->pre != _rq->pre) return false;
	if(this->stype != _rq->stype) return false;
	if(this->otype != _rq->otype) return false;
#ifdef ENABLE_LITERAL
	if(this->is_literal() && _rq->literal != "NULL")
	{
		if(this->literal != _rq->literal)
			return false;
	}
#endif

	return true;
}
	
bool rdfDedge::is_literal()
{
	return this->otype == "literal";
}

string rdfDedge::to_str()
{
	stringstream _ss;
	_ss << this->stype << this->s << " " << this->pre << " " << this->otype << this->t << " ";
	_ss << this->literal;
	_ss << " " << t_sec;
	//_ss << " " << size;
	return _ss.str();
}
