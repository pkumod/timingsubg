/*
 * runtime.cpp
 *
 */
#include "runtime.h"

runtime::runtime(){
 this->initial();
}
void runtime::initial(){
	sum = 0;
	dup_num = 0;
	cur = 0;
}
void runtime::begin(){
	runtime::get_time_cur(&(this->t_begin));
	this->dup_num ++;
}
void runtime::end(){
	runtime::get_time_cur(&(this->t_end));
	this->cur = runtime::get_interval(&(this->t_begin), &(this->t_end));	
	this->sum += this->cur;
	this->dup_num ++;

	if(this->dup_num % 2 != 0){
		cout << "num err != 0 " << this->dup_num << endl;
	}
}
double runtime::getsum(){
	if(this->dup_num % 2 != 0){
		cout << "err sum " << this->dup_num << endl;
		system("pause");
		return -1;
	}

	return this->sum;
}
double runtime::getcur(){
	if(this->dup_num % 2 != 0){
		cout << "err cur " << this->dup_num << endl;
		system("pause");
		exit(-1);
	}

	return this->cur;
}
double runtime::getavg()
{
	return this->getsum() / (this->getnum());
}

int  runtime::getnum(){
	if(this->dup_num % 2 != 0){
		cout << "err num " << this->dup_num << endl;
		return -1;
	}

	return this->dup_num / 2;
}


/*  */
void runtime::get_time_cur(Rtime * _rtime)
{
	gettimeofday(_rtime, NULL);
}
/*micro second*/
long runtime::get_interval(Rtime* _t1, Rtime* _t2){
	return (_t2->tv_sec-_t1->tv_sec)*1000.0 + (_t2->tv_usec - _t1->tv_usec)/1000.0;
}
