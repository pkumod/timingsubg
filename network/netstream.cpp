#include "netstream.h"

netstream::netstream(string _dat_set): gstream(_dat_set)
{

}

netstream::~netstream(){
	cout << "netstream destruction..." << endl;
}

bool netstream::load_edges(int _avg_win_tuple_num){
	cout << "IN load_edges from [" << this->data_path << "]" << endl; 
#ifdef DEBUG_TRACK
	util::track("IN load_edges from " + this->data_path);
#endif
	ifstream fin(this->data_path.c_str(), ios::in);
	if(!fin){
		cout << "err: failed open " << this->data_path << endl;
		exit(-1);
	}
	/* load edges */
	char _buf[5000];

	netDedge* _nd = NULL;
	while(! fin.eof())
	{
		fin.getline(_buf, 4999, '\n');
		if(strlen(_buf) < 2) continue;
		_nd = new netDedge(_buf);
		this->alledges.push_back((dEdge*)_nd);
	}
	fin.close();

	/* calculate avg time span */
	int all_enum = this->alledges.size();
	netDedge* _last = (netDedge*)(this->alledges[all_enum-1]);
	netDedge* _first= (netDedge*)(this->alledges[0]);

	long long int _tspan1 = _last->t_sec - _first->t_sec;
	long long int _tspan2 = _last->t_msec - _first->t_msec;
	long long int _scale = 1000*1000*1000;
	if(_tspan2 < 0){
		_tspan1 --;
		_tspan2 += _scale;
	}
	long long int _span = _tspan1*_scale+_tspan2;
	long long int _win_times = 1+ this->alledges.size()/_avg_win_tuple_num;
	long long int _avg_span = 1+ _span/_win_times;
	this->avg_span_t1 = (_avg_span/(_scale));
	this->avg_span_t2 = (_avg_span%(_scale));

	cout << "span["<< _span << "]," << "all[" << this->alledges.size() << "], ";
	cout << "avg_tuple=" << _avg_win_tuple_num << ", avg_span=" << _avg_span << endl;

#ifdef INVALID_READ
	if(this->alledges.size() < 200)
	{
		cerr << "remember uncomment the following codes" << endl;
	}
	
	cerr << "the follwing code need to be omitted" << endl;
	this->avg_span_t1 = 0;
	this->avg_span_t2 = 13135013;
	cerr << "the span_t1 and span_t2 has seen modified" << endl;
	
#endif
	cout << "avg t1 = " << this->avg_span_t1 << " ";
	cout << "avg t2 = " << this->avg_span_t2 << " ";
	cout << "avg tuple = " << _avg_win_tuple_num << endl;

	cout << "OUT load_edges: " << this->alledges.size() << endl;
	return true;
}

bool netstream::is_expire(dEdge* _e_old, dEdge* _e_new){
	netDedge* _ne1 = (netDedge*)_e_old;
	netDedge* _ne2 = (netDedge*)_e_new;
	if(_ne2->t_sec - _ne1->t_sec < this->avg_span_t1) return false;
	if(_ne2->t_sec - _ne1->t_sec > this->avg_span_t2) return true;
	if(_ne2->t_msec - _ne1->t_msec < this->avg_span_t2 ) return false;
	
	return true;
}
