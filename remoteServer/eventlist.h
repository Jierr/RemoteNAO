#ifndef __EVENTLIST__
#define __EVENTLIST__

#include <althread/almutex.h>
#include <althread/alcriticalsection.h>
#include <string>

#include "gen.h"

using namespace std;

typedef struct{
	int type;
	int iparams[IPARAM_LEN];
	string sparam;
} event_params_t;

class EventList;

class Event{
	friend class EventList; 
	private:
		Event* next;
		
	public:
		const void* const id;
		int classification;
		int taskID;
		event_params_t ep;
		Event():id(this)
		{
			ep.type = CODE_INVALID;
			next = 0;
			taskID = 0;
			classification = EVT_PENDING;
		}
		Event(const Event& event)
		:	id(event.id)
		{
			ep = event.ep;
			next = event.next;
			taskID = event.taskID;
			classification = event.classification;
		}
		~Event(){}
};

class EventList
{
	private:
		boost::shared_ptr<AL::ALMutex> mutex;
		int order;
		Event* first;
		Event* last;
		
	public:
		EventList();
		~EventList();
		bool isEmpty();
		bool hasPending();
		void setOrder(int ord);
		void addEvent(event_params_t ep);
		void addFirst(event_params_t ep);
		void removeEvent(const void* const iid);
		void removeDone();
		void removePending();
		void removeAll();
		void list();
		void setClassf(const void* const iid, int classf);
		int getClassf(const void* const iid);
		void setTask(const void* const iid, const int& tid);
		Event getFirst();
		Event* getPending();
		Event withID(const void* const iid);
		Event getLast();		
};

#endif
