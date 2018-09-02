#include "netstream.h"
#include "../util/util.h"
#include "netquery.h"
#include "../timing/query.h"
#include "../timing/timingconf.h"
#include "../timing/timingsubg.h"

/* 
 * argv0 : exe
 * argv1 : dataset
 * argv2 : query
 * argv3 : winsz
 * argv4 : Max_Thread_Num
 * 
 * */
int main(int argc, char* argv[])
{
#ifdef DEBUG_TRACK
#endif

#ifdef THREAD_LOG
#endif
	
	if(argc < 6){
		cout << "err argc" << endl;
		exit(0);
	}
	cout << "num of argc: " << argc << endl;

	string _dataset, _query, _frtime;
	_dataset = string(argv[1]);
	_query = string(argv[2]);
	_frtime = string(argv[5]);
	
	int _window, max_thread_num;
	{
		stringstream _ss;
		for(int i = 3; i < argc; i ++) 
			_ss << argv[i] << " ";
		_ss >> _window >> max_thread_num;
	}

	cout << "runtime=" << _frtime << endl;

	timingconf _tconf(_window, max_thread_num, _dataset, _query);
	netstream _NS(_dataset);
	netquery _Q(_query);
	_Q.parseQuery();
#ifdef RUN_COMMENT
	cout << "query Q: \n" << _Q.to_str() << endl;
	cout << "pre Q: \n" << _Q.timingorder_str() << endl;
#endif

	timingsubg tsubg(0, _frtime);
	tsubg.exename = string(argv[0]);
	tsubg.run(0, &_NS, &_Q, &_tconf);
	cout << "finish timingsubg run" << endl;

	tsubg.write_stat();

	util::finalize();
}

