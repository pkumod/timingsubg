#include "gstream.h"
#include "../timing/edge.h"

gstream::gstream(string& _data_path){
	this->data_path = _data_path;
	this->alledges.clear();
}

gstream::~gstream(){
	for(int i = 0; i < (int)alledges.size(); i ++)
	{
		delete this->alledges[i];
		this->alledges[i] = NULL;
	}
	this->alledges.clear();
#ifdef GLOBAL_COMMENT
	cout << "OUT gstream destruction..." << endl;
#endif
}

bool gstream::load_edges(int _avg_win_tuple_num)
{
	return _avg_win_tuple_num < 0;
}

bool gstream::is_expire(dEdge* _e_old, dEdge* _e_new)
{
	return false;
}

int gstream::size(){
	return this->alledges.size();
}

bool gstream::reset(){
	this->cur_itr = this->alledges.begin();
	return true;
}

bool gstream::hasnext(){
	return this->cur_itr != this->alledges.end();
}

dEdge* gstream::next(){
	if(this->cur_itr != this->alledges.end())
	{
		dEdge* _ret = *(this->cur_itr);
		++ this->cur_itr;
		return _ret;
	}
	return NULL;
}

//private:


