#include "timingconf.h"

timingconf::timingconf(string _conf_path)
{
	this->conf_path = _conf_path;
	this->vNum = -1;
	this->eNum = -1;
	this->window_size = -1;
	this->key2val.clear();
	this->path_dataset = "";
	this->path_elabel2id = "";
	this->path_vertex2id = "";
	this->path_vlabel2id = "";
	/* load config here */

}

/* load other conf from _dat */
timingconf::timingconf(int _win, int _max_thread, string _dat, string _q)
{
	this->window_size = _win;
	this->Max_Thread_Num = _max_thread;
	this->path_dataset = _dat;
	this->path_elabel2id = "";
	this->path_vertex2id = "";
	this->path_vlabel2id = "";
	this->Max_Query_Size = 10;
	this->conf_path = "";
}

string timingconf::dataset(){
	if(this->path_dataset == ""){
		this->path_dataset = this->key2val["dataset"];
	}
	return this->path_dataset;
}

string timingconf::vertex2id(){
	if(this->path_vertex2id == ""){
		this->path_vertex2id = this->key2val["vertex2id"];
	}
	return this->path_vertex2id;
}


string timingconf::vlabel2id(){
	if(this->path_vlabel2id == ""){
		this->path_vlabel2id = this->key2val["vlabel2id"];
	}
	return this->path_vlabel2id;
}

string timingconf::elabel2id(){
	if(this->path_elabel2id == ""){
		this->path_elabel2id = this->key2val["elabel2id"];
	}
	return this->path_elabel2id;
}


int timingconf::getvNum(){
	if(this->vNum == -1){
		stringstream _ss(this->key2val["vertex_num"]);
		_ss >> this->vNum;
	}
	return this->vNum;
}

int timingconf::geteNum(){
	if(this->eNum == -1){
		stringstream _ss(this->key2val["edge_num"]);
		_ss >> this->eNum;
	}
	return this->eNum;
}

int timingconf::getmaxthreadNum(){
	if(this->Max_Thread_Num == -1){
		stringstream _ss(this->key2val["max_thread_num"]);
		_ss >> this->Max_Thread_Num;
	}
	return this->Max_Thread_Num;
}

int timingconf::getmaxquerysize(){
	if(this->Max_Query_Size == -1){
		stringstream _ss(this->key2val["max_query_size"]);
		_ss >> this->Max_Query_Size;
	}
	return this->Max_Query_Size;
}

int timingconf::getwinsz(){
	if(this->window_size == -1){
		stringstream _ss(this->key2val["win_size"]);
		_ss >> this->window_size;
	}
	return this->window_size;
}

string timingconf::to_str(){
	stringstream _ss;
	map<string, string>::iterator itr = this->key2val.begin();
	while(itr != this->key2val.end())
	{
		_ss << itr->first << " = " << itr->second << endl;
		++ itr;
	}

	return _ss.str();
}

/* private  */
void timingconf::loadconf(){
	fstream _fin(this->conf_path.c_str(), ios::in);
	if(! _fin){
		cout << "err: fail to open " << this->conf_path << endl;
	}
	char buf[1000];
	char key[1000];
	char val[1000];
	char _equal;
	int _i_len = 0;
	while(! _fin.eof())
	{
		memset(buf, 0, sizeof(buf));
		_fin.getline(buf, 999, '\n');
		_i_len = strlen(buf);
		if(_i_len < 2) continue;
		if(buf[0] == '#') continue;

		stringstream _ss(buf);
		memset(key, 0, sizeof(key));
		memset(val, 0, sizeof(val));
		_ss >> key;
		_ss >> _equal;
		_ss >> val;
		this->setkey2val(key, val);	
	}
}

void timingconf::setkey2val(string _key, string _val){
	key2val[_key] = _val;
	/*
	if(_key == "dataset"){
		this->path_dataset = _val;
	}
	else if(_key == "vertex2id"){
		this->path_vertex2id = _val;
	}
	else if(_key == "vlabel2id"){
		this->path_vlabel2id = _val;
	}
	else if(_key == "elabel2id"){
		this->path_elabel2id = _val;
	}
	else
	{// number 
		stringstream _ss(_val);	
		if(_key == "vertex_num"){
			
		}	
	}
	*/
}
