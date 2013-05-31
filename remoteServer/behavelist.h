#ifndef _BEHAVELIST_
#define _BEHAVELIST_

#include <string>

using namespace std;


/**
\brief Specifies a behavior found on the robot. These behaviors are uploaded with Coreographe. \n
	   Behaviors will be treated as Events
*/
typedef struct behaviour_t
{
	string name; ///< name of the behavior without state transition
	string full; ///< actual filename of the behavior
	int fstate; ///< defined starting state for the transition
	int lstate; ///< defined ending state for the transition
	struct behaviour_t* next; ///< next behavior in the list
} behave_t;


/**
\brief This class manages a list of struct behaviour_t.
*/
class Behavelist 
{
	private:
		behave_t* first; ///< first element in the list
		behave_t* last; ///< last element in the list
	
	public:
		Behavelist();
		~Behavelist();
		
		///< find an behavior at index \a nr of the list
		behave_t* getwNr(const int& nr);
		
		///< get the size of the list
		int getSize();
		
		///< find behavior with \a name
		behave_t* getBehave(const string& name);
		
		///< get the starting state of the behavior with \a name
		int getfState(const string& name);
		
		///< get the ending state of the behavior with \a name
		int getlState(const string& name);
		
		///< add behavior to the ending of the list
		void addBehave(const int& fstate, const int& lstate, const string& name, const string& full);
		
		///< get content for debugging
		string list();
};

#endif
