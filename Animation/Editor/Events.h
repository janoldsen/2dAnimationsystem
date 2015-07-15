#ifndef EVENTS_H
#define EVENTS_H

#include "SDL_events.h"

struct EventType
{
	enum Type
	{
		KEY,

	};
};

struct Event
{
	EventType Type;

	union 
	{
		struct KeyEvent
		{
			int key;
			int modifier;
		};
		

	};

};



class HighEventStack
{
public:
	HighEventStack();

	void Push(Event event);
	Event Top();
	Event Pop();
	
	int getSize();

private:
	static const int SIZE = 128;

	int m_Top;
	int m_Size;
	Event m_Events[SIZE];

};



class LowEventStack
{
public:
	LowEventStack();

	void Push(const SDL_Event& event);
	SDL_Event Top();
	SDL_Event Pop();

	int getSize();

private:
	static const int SIZE = 128;

	int m_Top;
	int m_Size;
	SDL_Event m_Events[SIZE];

};






#endif