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
	first->classification = EVT_PENDING;
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
	last->classification = EVT_PENDING;
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
			delete curr;
		}
		else
		{
			first = curr->next;
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
			if(prev)
			{
				prev->next = curr->next;
				delete curr;
			}
			else
			{
				first = curr->next;
				delete curr;
			}
		}
		prev = curr;
		curr = curr->next;
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
	
	mutex->lock();
	
	curr = first;
	while (curr && (curr->classification != EVT_PENDING))
		curr = curr->next;
	
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





