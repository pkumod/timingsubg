#ifndef _TIMINGCONF_H_
#define _TIMINGCONF_H_

#include "../util/util.h"

class timingconf
{
public:
	timingconf(string _conf_path);
	/* load other conf from _dat */
	timingconf(int _win, int _max_thread, string _dat, string _q);
	string dataset();
	string vertex2id();
	string vlabel2id();
	string elabel2id();
	int getvNum();
	int geteNum();
	int getmaxthreadNum();
	int getmaxquerysize();
	int getwinsz();

	string to_str();

private:
	void loadconf();
	void setkey2val(string _key, string _val);

	string conf_path;
	string path_dataset;
	string path_vertex2id;
	string path_vlabel2id;
	string path_elabel2id;
	int vNum;
	int eNum;
	int Max_Thread_Num;
	int Max_Query_Size;
	int window_size;

	map<string, string> key2val;
};

#endif
