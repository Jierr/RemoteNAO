#ifndef __EVENTLIST__
#define __EVENTLIST__

#include <althread/almutex.h>
#include <althread/alcriticalsection.h>
#include <string>

#include "gen.h"

using namespace std;


/**
\brief structure which concentrates the information needed to create a valid event
*/
typedef struct{
	int type; ///< Comand Specifier TOKEN --> gen.h
	int iparams[IPARAM_LEN]; ///< holds integer parameters
	string sparam; ///< holds string parameter
} event_params_t;


class EventList;

/**
\brief Contains a fully parsed and valid comand.

It realizes a list, the access on this list is done by EventList class.
*/
class Event{
	friend class EventList; 
	private:
		Event* next; ///< pointer to the next Event 
		
	public:
		const void* const id; ///< Unique identifier.
		int classification; ///< Current processing state of this event. Defined in gen.h 
		int taskID; ///< reserved
		event_params_t ep; ///< struct with key datas
		
		/**
		\brief Constructor
		
		Creates an initially invalid event.
		*/
		Event():id(this)
		{
			ep.type = CODE_INVALID;
			next = 0;
			taskID = 0;
			classification = EVT_PENDING;
		}
		
		/**
		\brief Copy Constructor
		*/
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


/**
\brief Manges Access on a list of events

Each access needs to be atomic for each instance of EventList, since it is supposed to be used in multiple threads.
Modules Manger and Executer use the same instance of EventList.
*/
class EventList
{
	private:
		boost::shared_ptr<AL::ALMutex> mutex; ///< Grants atomic access.
		int order; ///< specifies the priority to execute the events, hence retreiving them via hasPending()
		Event* first; ///< Pointer to the first Event in the list
		Event* last; ///< Pointer to the last Event in the list 
		
	public:
		/// Constructor: Creates empty event list.
		EventList();
		///Destructer: Deletes all events
		~EventList();
		///Check if the list is empty.
		bool isEmpty();
		///Check if the list has none processed events to be executed.
		bool hasPending();
		/**
		\brief Sets priorities (#order) to retrieve events, defined in gen.h
		
		\param ord
		*/
		void setOrder(int ord);
		/**
		\brief Adds event to the ending of the list.
		
		\param ep Contains key data.
		*/
		void addEvent(event_params_t ep);
		/**
		\brief Adds event to the beginning of the list.
		
		\param ep Contains key data.
		*/
		void addFirst(event_params_t ep);
		/**
		\brief Remove the Event with Event::id == \a iid.
		*/
		void removeEvent(const void* const iid);
		/**
		\brief Remove all finisched events.
		*/
		void removeDone();
		/**
		\brief Remove all pending events.
		*/
		void removePending();
		/**
		\brief Remove all events.
		*/
		void removeAll();
		/**
		\brief Debug: list events on stdout
		*/
		void list();
		/**
		\brief Change the classification of the event with iid.
		
		\param iid
		\param classf
		
		\see gen.h
		*/
		void setClassf(const void* const iid, int classf);
		
		/**
		\brief Get the classification of the event with iid.
		
		\param iid
		\return Classification of the event.
		
		\see gen.h
		*/
		int getClassf(const void* const iid);
		
		///reserved
		void setTask(const void* const iid, const int& tid);
		
		/**
		\brief Get the first event.
		*/
		Event getFirst();
		/**
		\brief Get the last event.
		*/
		Event getLast();	
		///Get the first Event of the list fulfilling the conditions of #order, hence a given priority.
		Event* getPending();
		
		/**
		\brief Get the event with coresponding id. 
		\param iid
		*/ 
		Event withID(const void* const iid);
			
};

#endif
