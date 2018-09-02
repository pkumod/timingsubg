#ifndef _LIST_T_H_
#define _LIST_T_H_

#include "util.h"
template <typename Item>
class List{
public:

	List(bool _to_be_free = false){
		this->to_be_free = _to_be_free;
		this->vpool.clear();	
	}

	~List(){
		if(this->to_be_free)
		{
			this->destroy();
		}
	}

	bool reset(){
		this->cur_itr = this->vpool.begin();
		return true;
	}
	
	bool hasnext(){
		return this->cur_itr != this->vpool.end();
	}
	
	Item* next(){
		if(this->cur_itr == this->vpool.end()){
			cout << "err end vpool" << endl;
			exit(-1);
		}
		Item* _ret = *(this->cur_itr);
		++ this->cur_itr;
		return _ret;
	}

	Item* current()
	{
		if(this->cur_itr == this->vpool.end()){
			cout << "err end vpool" << endl;
			exit(-1);
		}
		return *(this->cur_itr);
	}

	Item* pop_current()
	{
		if(this->cur_itr == this->vpool.end()){
			cout << "err end vpool" << endl;
			exit(-1);
		}
		Item* _cur = *(this->cur_itr);
		this->cur_itr = this->vpool.erase(this->cur_itr);	

		return _cur;
	}

	Item* first(){
		return *(this->vpool.begin());
	}

	Item* second(){
		typename vector<Item*>::iterator itr = this->vpool.begin();
		++itr;
		return *(itr);
	}
	
	Item* last(){
		return *(this->vpool.rbegin());
	}

	bool add(Item* _a){
		this->vpool.push_back(_a);
		return true;
	}

	bool add_copy(Item& _a){
	    /* should not be called */	
		return true;
	}

	void destroy()
	{
		for(int i = 0; i < (int)this->vpool.size(); i ++)
		{
			delete (this->vpool[i]);		
		}
	}

	bool clear()
	{
		if(this->to_be_free)
		{
			this->destroy();
		}
		this->vpool.clear();
		this->cur_itr = this->vpool.begin();
		return true;
	}

	bool empty(){
		return this->vpool.empty();
	}

	int size(){
		return (int)this->vpool.size();
	}

	bool is_to_free(){
		return this->to_be_free;
	}

private:
	bool to_be_free;
	typename vector<Item*>::iterator cur_itr;
	vector<Item*> vpool;
};

#endif
