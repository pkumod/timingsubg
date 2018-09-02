#include "lockReq.h"
#include "../util/util.h"
#include "nodeOP.h"

/* OP will be set as null when lr is created for delete transaction */
lockReq::lockReq(nodeOP* _nOP, pthread_t* _tid, int _ts):
op(_nOP), tid(_tid), timestamp(_ts)
{
	pthread_cond_init(&(this->t_cond), NULL);
}

lockReq::~lockReq(){
	pthread_cond_destroy(&(this->t_cond));
}

/*  */
bool lockReq::isXlock(){
	if(this->op == NULL) return true;

	return this->op->is_write();	
}

bool lockReq::isSlock(){
	if(this->op == NULL) return false;

	return this->op->is_read();
}

string lockReq::to_str()
{
	stringstream _ss;
	_ss << "[" << *(this->tid) << "," << &(this->t_cond) << "]";
	return _ss.str();
}

