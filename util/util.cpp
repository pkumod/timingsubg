#include "util.h"
#include "list_T.h"


ofstream* util::f_track = NULL;
ofstream* util::f_threadlog = NULL;
pthread_mutex_t util::mutex_log;
pthread_mutex_t util::mutex_track;
map<pthread_t, int> util::t2count;





void util::reg_track(int _count){
	pthread_mutex_lock(& util::mutex_track);
	util::t2count[pthread_self()] = _count;
	pthread_mutex_unlock(& util::mutex_track);
}

void util::init_track(string _track_path){
	if(util::f_track != NULL){
		exit(-1);
	}

	util::f_track = new ofstream(_track_path.c_str(), ios::out);
	if( !(*f_track) ){
		cout << "err failed open " << _track_path << endl;
		exit(-1);
	}
	pthread_mutex_init(& util::mutex_track, NULL);
}

void util::track(stringstream& _ss){
	util::track(_ss.str(), "");
}

void util::track(string _s, string _lat){
	pthread_mutex_lock(& util::mutex_track);

	if(util::f_track == NULL){
		cout << "err NULL f_track" << endl;
		exit(-1);
	}
	

#ifndef NO_THREAD
	int _count = t2count[pthread_self()];
	if(_count < 0) 
		*(util::f_track) << "T[**MAIN**]";
	else
		*(util::f_track) << "T[" << _count/100 << "-"<< _count%100 << "," << pthread_self() << "]";
#endif

	*(util::f_track) << _s << _lat;
	(util::f_track)->flush();
#ifdef STD_TRACK
#ifndef NO_THREAD
	cout << "T[" << _count/100 << "-"<< _count%100 << "," << pthread_self() << "]";
#endif
	cout << _s << _lat;
	cout.flush();
#endif

	pthread_mutex_unlock(& util::mutex_track);
}

void util::init_threadlog(string _log_path){
	if(util::f_threadlog != NULL){
		exit(-1);
	}

	util::f_threadlog = new ofstream(_log_path.c_str(), ios::out);
	if( !(*f_threadlog) ){
		cout << "err failed open " << _log_path << endl;
		exit(-1);
	}
	pthread_mutex_init(& util::mutex_log, NULL);
}

void util::threadlog(stringstream& _ss){
	util::threadlog(_ss.str(), "");
}

void util::threadlog(string _s, string _lat){
	pthread_mutex_lock(& util::mutex_log);

	*(util::f_threadlog) << "\nT[" << pthread_self() << "]\n" << _s << _lat;
	(util::f_threadlog)->flush();

	pthread_mutex_unlock(& util::mutex_log);
}


void util::finalize()
{

#ifdef DEBUG_TRACK
	util::track("IN finalize");
	cout << "IN finalize" << endl;
#endif

	if(util::f_track != NULL){
		util::f_track->close();
		delete util::f_track;
		util::f_track = NULL;
	}

	if(util::f_threadlog!= NULL){
		util::f_threadlog->close();
		delete util::f_threadlog;
		util::f_threadlog = NULL;
	}
#ifdef DEBUG_TRACK
	cout << "OUT finalize" << endl;
#endif

}

