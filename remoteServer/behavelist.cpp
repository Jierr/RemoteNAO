#include <sstream>  
#include "behavelist.h"


Behavelist::Behavelist()
{
	first = 0;
	last = 0;
}

Behavelist::~Behavelist()
{
	behave_t* curr = first;
	while(curr)
	{
		curr = first->next;
		delete first;
		first = curr;
	}
	first = last = 0;
}

behave_t* Behavelist::getBehave(const string& name)
{
	behave_t* curr = first;
	
	while (curr && (curr->name.compare(name) != 0))
		curr = curr->next;
		
	return curr;
}

behave_t* Behavelist::getwNr(const int& nr)
{
	behave_t* curr = first;
	int n = 0;
	
	while ((n!=nr) && curr)
	{
		curr = curr->next;
		++n;	
	}
				
	return curr;	
}

int Behavelist::getSize()
{
	behave_t* curr = first;
	int n = 0;
	
	while (curr)
	{
		curr = curr->next;
		++n;	
	}

	return n;
}

int Behavelist::getfState(const string& name)
{
	behave_t* curr = first;
	
	while (curr && (curr->name.compare(name) != 0))
		curr = curr->next;
	if (curr)
	{
		return curr->fstate;
	}
}

int Behavelist::getlState(const string& name)
{
	behave_t* curr = first;
	
	while (curr && (curr->name.compare(name) != 0))
		curr = curr->next;
	if (curr)
	{
		return curr->lstate;
	}
}

void Behavelist::addBehave(const int& fstate, const int& lstate, const string& name, const string& full)
{
	behave_t* bnew = new behave_t;
	bnew->fstate = fstate;
	bnew->lstate = lstate;
	bnew->name = name;
	bnew->full = full;
	bnew->next = 0;
	if(!first)
	{
		first = last = bnew;
		last->next = 0;
	}
	else
	{
		last->next = bnew;
		last = bnew;
		last->next = 0;
	}
}

string Behavelist::list()
{
	string str = "";
	behave_t* curr;
	
	stringstream all;
	curr = first;
	
	while (curr)
	{
		all<< curr->name;
		all<<"\n";
		all<< curr->full;
		all<<"\n";
		all<<"	>fstate = ";
		all<< curr->fstate;
		all<<"\n";
		all<<"	>lstate = ";
		all<< curr->lstate;
		all<<"\n";
		curr = curr->next;
	}
	str = all.str();
	return str;
}

