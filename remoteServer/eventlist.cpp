#include <iostream>
#include "eventlist.h"


EventList::EventList()
:	mutex(AL::ALMutex::createALMutex())
{
	first = 0; 
	last = 0;
}

EventList::~EventList()
{
	mutex->lock();
	Event* next = first;
	while(first)
	{
		next = first->next;
		delete first;
		first = next;
	} 
	first = 0;
	last = 0;
	mutex->unlock();
}

bool EventList::isEmpty()
{
	return (first == 0);
}


void EventList::addFirst(event_params_t ep)
{
	Event* scnd;
	
	mutex->lock();
	scnd = first;
	first = new Event;
	first->next = scnd;
	if (!scnd)
		last = first;
	first->ep = ep;
	first->taskID = 0;	
	
	if (ep.type >= CODE_PARALLEL)
		first->classification = EVT_PENDINGPAR;
	else
		first->classification = EVT_PENDINGABS;
	mutex->unlock();
}

void EventList::addEvent(event_params_t ep)
{
	mutex->lock();
	if(!first)
	{
		first = new Event;
		last = first;
	}
	else
	{
		last->next = new Event;
		last = last->next;	
	}
	
	if (ep.type >= CODE_PARALLEL)
		last->classification = EVT_PENDINGPAR;
	else
		last->classification = EVT_PENDINGABS;
	last->ep = ep;
	last->taskID = 0;
	last->next = 0;
	mutex->unlock();
}

void EventList::removeEvent(const void* const iid)
{
	Event* prev;
	Event* curr;
	
	mutex->lock();
	prev = 0;
	curr = first;
	while(curr && (curr != iid))
	{
		prev = curr;
		curr = curr->next;
	}
	
	//is found?
	if(curr && !curr->taskID)
	{
		//curr != first
		if(prev)
		{
			prev->next = curr->next;
			//if curr is the last event, and not
			//the onliest
			if (curr == last)
				last = prev;
			delete curr;
		}
		else
		{
			first = curr->next;
			//if curr is the onliest event then
			if (curr == last)
			{
				first = 0;
				last = 0;
			}
			delete curr;
		}
	}
	mutex->unlock();
}

void EventList::removeDone()
{
	Event* curr;
	Event* prev;
	mutex->lock();
	
	prev = 0;
	curr = first;
	
	while(curr)
	{
		if (curr->classification == EVT_DONE)
		{
			//curr != first
			if(prev)
			{
				prev->next = curr->next;
				//if curr is the last event, and not
				//the onliest
				if (curr == last)
					last = prev;
				delete curr;
				curr = prev->next;
			}
			else
			{
				first = curr->next;
				//if curr is the onliest event then
				if (curr == last)
				{
					first = 0;
					last = 0;
				}
				delete curr;
				curr = first;
			}
			
		}
		else
		{		
			prev = curr;
			curr = curr->next;
		}
		
	}
	
	mutex->unlock();
}

void EventList::removeAll()
{
}


void EventList::list()
{
	Event* curr = first;
	int n = 0;
	while (curr)
	{
		cout<< "Event[" << n << "] = " << curr->ep.type 
			<< ", valid? -> " << (curr == curr->id) << endl;
		++n;
		curr = curr->next;	
	} 
}


void EventList::setClassf(const void* const iid, int classf)
{
	Event* curr;
	mutex->lock();
	curr = first;
	while(curr && (curr->id != iid))
	{
		curr = curr->next;
	}
	if(curr)
		curr->classification = classf;
	
	mutex->unlock();
}


void EventList::setTask(const void* const iid, const int& tid)
{
	Event* curr;
	mutex->lock();	
	curr = first;
	while(curr && (curr->id != iid))
	{
		curr = curr->next;
	}
	if(curr)
		curr->taskID = tid;
	
	mutex->unlock();
}


Event EventList::getPending()
{	
	Event* curr;
	int evt_state;
	
	mutex->lock();
	
	if (order == ORD_ABS)
		evt_state = EVT_PENDINGABS;
	else if (order == ORD_PAR)
		evt_state = EVT_PENDINGPAR;
	else 
		evt_state = EVT_PENDING;
	
	curr = first;
	while (curr && 
	       	((	(evt_state != EVT_PENDING) && 
	       		(curr->classification != evt_state)) || 
	        (	(evt_state == EVT_PENDING) && 
	        	(curr->classification != EVT_PENDINGPAR) && 
	        	(curr->classification != EVT_PENDINGABS) )))
	{
		curr = curr->next;
	}
	
	//Pending Event found
	if (curr)
	{
		Event result(*curr);
		mutex->unlock();
		return result;
	}
	else
	{
		Event result;
		mutex->unlock();
		return result;
	}
}

Event EventList::getFirst()
{
	mutex->lock();
	Event result(*first);
	mutex->unlock();
	return result;
}


Event EventList::getLast()
{	
	mutex->lock();
	Event result(*last);
	mutex->unlock();
	return result;
}


void EventList::setOrder(int ord)
{
	mutex->lock();
	if ((ord == EVT_PENDINGABS) || (ord == EVT_PENDINGPAR) )
		order = ord;
	else 
		order = EVT_PENDING;
	mutex->unlock();
}





