
#include <memory>

template <typename T>
StructArray<T>::StructArray(int capacity)
	: m_capacity(capacity),
	m_size(0)
{
	m_t = new T[capacity];
}

template<typename T>
int StructArray<T>::push(T str)
{
	if (m_size+1 >= m_capacity)
	{	
		T* newT = new T[m_capacity*2];

		memcpy(newT, m_t, m_capacity * sizeof(T));
		m_t = newT;
		m_capacity *= 2;
	}

	m_t[m_size] = str;
	m_size++;

	return m_size - 1;
}

template<typename T>
T& StructArray<T>::operator[](int idx)
{
	return m_t[idx];
}


template<typename T>
void StructArray<T>::removeID(int id)
{
	m_t[id] = m_t[--m_size];
}

template<typename T>
void StructArray<T>::removeStr(T str)
{
	for (int id = 0; id < m_size; id++)
	{
		if (m_t[id] == str)
		{
			m_t[id] = m_t[--m_size];
			return;
		}
	}
}


template<typename T>
void StructArray<T>::clear()
{
	m_size = 0;
}


template<typename T>
int StructArray<T>::size()
{
	return m_size;
}


