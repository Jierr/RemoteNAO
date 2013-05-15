#include <iostream>
#include "eventlist.h"


EventList::EventList()
:	mutex(AL::ALMutex::createALMutex())
{
	first = 0; 
	last = 0;
	inspect = 0;
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
	inspect = 0;
	mutex->unlock();
}

bool EventList::isEmpty()
{
	return (first == 0);
}

bool EventList::hasPending()
{
	bool result = false;
	Event* curr;
	mutex->lock();
	curr = first;
	while (curr && (curr->classification != EVT_PENDING))
	{
		curr = curr->next;
	}
	if (curr)
		result = true;
	mutex->unlock();
	return result;
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
	
	inspect = first;
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
	if(curr)
	{
		if (curr == inspect)
			inspect = curr->next;
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
			if (curr == inspect)
				inspect = curr->next;
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

void EventList::removePending()
{
	Event* curr;
	Event* prev;
	mutex->lock();
	
	prev = 0;
	curr = first;
	
	while(curr)
	{
		if (curr->classification == EVT_PENDING)
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
	
	inspect = first;
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
			<< ", valid? -> " << (curr == curr->id) << ", Classification: " << curr->classification << endl;
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

int EventList::getClassf(const void* const iid)
{
	int result;
	Event* curr;
	mutex->lock();
	curr = first;
	while (curr && (curr->id != iid))
		curr = curr->next;
		
	if (curr)
		result = curr->classification;
	else
		result = EVT_DONE;
	mutex->unlock();
	return result;
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


Event* EventList::getPending(const bool& restart)
{	
	Event* curr;
	
	mutex->lock();
	
	if (restart || !inspect)
		inspect = first;
	curr = inspect;
	while (curr && (curr->classification != EVT_PENDING))
	{
		curr = curr->next;
	}
	
	//Pending Event found
	if (curr)
	{
		//Event result(*curr);
		inspect = curr->next;
		mutex->unlock();
		return curr;
	}
	else
	{
		//Event result;
		mutex->unlock();
		return 0;
	}
}


Event EventList::withID(const void* const iid)
{
	Event* curr;
	mutex->lock();
	curr = first;
	while(curr && (curr->id != iid))
		curr = curr->next;
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

Event* EventList::getFirst()
{
	mutex->lock();
	inspect = first;
	mutex->unlock();
	return first;
}


Event* EventList::getLast()
{	
	mutex->lock();
	inspect = last;
	mutex->unlock();
	return last;
}


void EventList::setOrder(int ord)
{
//	mutex->lock();
//	if ((ord == EVT_PENDINGABS) || (ord == EVT_PENDINGPAR) )
//		order = ord;
//	else 
//		order = EVT_PENDING;
//	mutex->unlock();
}


bool EventList::reduceLastWalking()
{
	Event* start;
	Event* end;
	Event* curr;
	Event* prev;
	mutex->lock();
	
	start = first;
	end = 0;
	prev = 0;
	curr = first;
	
	while (start && !((start->ep.type == CODE_MOV) && (start->classification == EVT_BUSY)))
	{
		start = start->next;
	}
	cout<< "[Eventlist] Start found!" << endl;
	if (!start)
	{
		mutex->unlock();
		return false;
	}
	
	end = 0;	
	prev = start;
	start = start->next;
	curr = start;
	while(curr && ((curr->ep.type == CODE_MOV) || (curr->classification == EVT_BUSY) || (curr->classification == EVT_DONE)) )
	{
		if ((curr->ep.type == CODE_MOV) && (curr->classification == EVT_PENDING)) 
		{
			end = curr;
		}
		curr = curr->next;
	}
	cout<< "[Eventlist] End found!" << endl;
	
	if (!end)
	{
		mutex->unlock();
		return false;
	}
	
	cout<< "[Eventlist] Start deleting!" << endl;
	while (start != end)
	{
		curr = start->next;
		if ((start->ep.type == CODE_MOV) && (start->classification == EVT_PENDING))
		{		
			cout<< "[Eventlist] Delete start" << endl;
			if(start)
			{
				if (start == inspect)
					inspect = start->next;
				//curr != first
				if(prev)
				{
					prev->next = start->next;
					//if curr is the last event, and not
					//the onliest
					if (start == last)
						last = prev;
					delete start;
				}
				else
				{
					first = start->next;
					//if curr is the onliest event then
					if (start == last)
					{
						first = 0;
						last = 0;
					}
					delete start;
				}
			}
		}
		else 
		{
			prev = start;
		}
		start = curr;
	}	
	mutex->unlock();
	return true;
}




