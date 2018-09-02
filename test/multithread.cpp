#include<iostream>
using namespace std;
#include<pthread.h>
#include<stdio.h>
#include<vector>
#include<queue>
#include<map>
#include<stdlib.h>

class lockreq{
public:
	lockreq(pthread_t* _p_tid, int _v){
		this->val = _v;
		this->p_tid = _p_tid;
	}
	
	lockreq& operator=(const lockreq & _lr)
	{
		this->val = _lr.val;
		this->p_tid = _lr.p_tid;
		return *this;
	}

	int val;
	pthread_t* p_tid;
};
class test{
public:
	test(){}


	static void* print_thread(void*)
	{
		while(true)
		{
			pthread_mutex_lock(&test::mutex_print);
			if(iqueue.empty()){
				pthread_t t_this = pthread_self();
				cout << "iqueue is empty from " << t_this << endl;
				pthread_mutex_unlock(&test::mutex_print);
				break;
			}

			lockreq lr_h = iqueue.front();
			pthread_t t_this = pthread_self();
			if(pthread_equal(t_this, *(lr_h.p_tid)))
			{
				iqueue.pop();
				cout << t_this << " print = " << lr_h.val << endl;
			}
			pthread_mutex_unlock(&test::mutex_print);
		}
	}

	void run(){
		while(! iqueue.empty()) iqueue.pop();

		pthread_t thread1, thread2;
		iqueue.push(lockreq(&thread1, 1));	
		iqueue.push(lockreq(&thread2,2));	
		iqueue.push(lockreq(&thread1,3));	
		iqueue.push(lockreq(&thread2,4));	
		iqueue.push(lockreq(&thread1,5));	
		iqueue.push(lockreq(&thread2,6));	
		iqueue.push(lockreq(&thread1,7));	
		iqueue.push(lockreq(&thread1,8));	
		iqueue.push(lockreq(&thread1,9));	
		iqueue.push(lockreq(&thread2,10));	
		iqueue.push(lockreq(&thread2,11));
		
		pthread_create(&thread1, NULL,  test::print_thread, NULL);
		pthread_create(&thread2, NULL,  test::print_thread, NULL);
		cout << "thread1 id = " <<  thread1 << endl;
		cout << "thread2 id = " <<  thread2 << endl;
		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);
	}	



	static pthread_mutex_t mutex_print;
	static queue<lockreq> iqueue;
};

queue<lockreq> test::iqueue;
pthread_mutex_t test::mutex_print = PTHREAD_MUTEX_INITIALIZER;

int main()
{
	cout << "In main" << endl;

	test _itest;
	_itest.run();

	cout << "Out main" << endl;
	return 0;
}

