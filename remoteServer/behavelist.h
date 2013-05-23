#ifndef _BEHAVELIST_
#define _BEHAVELIST_

#include <string>

using namespace std;

typedef struct behaviour_t
{
	string name;
	string full;
	int fstate;
	int lstate;
	struct behaviour_t* next;
} behave_t;

class Behavelist 
{
	private:
		behave_t* first;
		behave_t* last;
	
	public:
		Behavelist();
		~Behavelist();
		behave_t* getwNr(const int& nr);
		int getSize();
		behave_t* getBehave(const string& name);
		int getfState(const string& name);
		int getlState(const string& name);
		void addBehave(const int& fstate, const int& lstate, const string& name, const string& full);
		string list();
};

#endif
