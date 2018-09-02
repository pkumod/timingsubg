#include "nodeOP.h"
#include "teNode.h"

nodeOP::nodeOP(char _op_type, teNode* _node)
{
	this->op_type = _op_type;
	this->onode = _node;
}

bool nodeOP::is_join(){
	return this->op_type == 'j';	
}

bool nodeOP::is_insert(){
	return this->op_type == 'i';
}

bool nodeOP::is_remove(){
	return this->op_type == 'r';
}

bool nodeOP::is_write(){
	return this->op_type == 'i' || this->op_type == 'r';
}

bool nodeOP::is_read(){
	return this->op_type == 'j';
}

string nodeOP::to_str(){
	stringstream _ss;
	_ss << "[op:" << this->op_type << "]--" << this->onode->to_str();
	return _ss.str();
}
