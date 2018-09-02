#include "rdfstream.h"
#include "../util/util.h"
#include "rdfquery.h"
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
		for(int i = 0; i < argc; i ++)
		{
			cout << argv[i] << "\t\t";
		}
		cout << endl;
		exit(0);
	}
#ifdef GLOBAL_COMMENT
	cout << "num of argc: " << argc << endl;
#endif

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

#ifdef GLOBAL_COMMENT
	cout << "runtime=" << _frtime << endl;
#endif

	timingconf _tconf(_window, max_thread_num, _dataset, _query);
	rdfstream _NS(_dataset);
	rdfquery _Q(_query);
	_Q.parseQuery();
#ifdef RUN_COMMENT
	cout << "query Q: \n" << _Q.to_str() << endl;
	cout << "pre Q: \n" << _Q.timingorder_str() << endl;
#endif

	timingsubg tsubg(0, _frtime);
	tsubg.exename = string(argv[0]);
	tsubg.run(0, &_NS, &_Q, &_tconf);
#ifdef GLOBAL_COMMENT
	cout << "finish timingsubg run" << endl;
#endif
	
	tsubg.write_stat();

	util::finalize();
}

