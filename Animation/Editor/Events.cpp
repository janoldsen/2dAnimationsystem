#include "Events.h"
#include "MyMath.h"



LowEventStack::LowEventStack()
	: m_Top(0),
	m_Size(0)
{
}


void LowEventStack::Push(const SDL_Event& event)
{
	
	m_Events[m_Top] = event;

	m_Top = (m_Top + 1)%SIZE;
	m_Size = MIN((m_Top + 1), SIZE);
}

SDL_Event LowEventStack::Top()
{
	int pos = (m_Top) > 0 ? m_Top-1 : SIZE + (m_Top-1);
	
	return m_Events[pos];
}

SDL_Event LowEventStack::Pop()
{
	m_Top = (m_Top) > 0 ? m_Top - 1 : SIZE + (m_Top - 1);

	return m_Events[m_Top];
}


int LowEventStack::getSize()
{
	return m_Size;
}