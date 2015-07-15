#ifndef STRUCT_ARRAY_H
#define STRUCT_ARRAY_H

#include <memory>
#include <stack>

template <typename T>
class StructArray
{
public:
	StructArray(int capacity = 2);
	StructArray(T* ts, int numT);
	~StructArray();

	StructArray(const StructArray& structArray);
	StructArray& operator==(const StructArray& structArray);


	int push(T str);
	T& operator[](int idx);

	void removeID(int id);
	void removeStr(T str);

	void free(int id, const T& replacement);

	void clear();

	int size();

private:

	int m_capacity;
	int m_size;

	std::stack<int> m_freeIDs;


	T* m_t;

};


template <typename T>
StructArray<T>::StructArray(int capacity)
	: m_capacity(capacity),
	m_size(0)
{
	m_t = new T[capacity];
}


template <typename T>
StructArray<T>::StructArray(T* ts, int numT)
	: m_capacity(numT),
	m_size(numT)
{
	m_t = new T[capacity];

	memcpy(m_T, ts, numT * sizeof(T));
}

template <typename T>
StructArray<T>::~StructArray()
{
	delete[] m_t;
}

template <typename T>
StructArray<T>::StructArray(const StructArray<T>& structArray)
	: m_capacity(structArray.m_capacity),
	m_size(structArray.m_size)
{
	m_t = new T[m_capacity];
	m_freeIDs = structArray.m_freeIDs;
	memcpy(m_t, structArray.m_t, sizeof(T) * m_size);
}

template <typename T>
StructArray<T>& StructArray<T>::operator==(const StructArray<T>& structArray)
{
	delete m_t[];

	*this = structArray;
	return *this;
}




template<typename T>
int StructArray<T>::push(T str)
{
	if (m_freeIDs.size() > 0)
	{
		int id = m_freeIDs.top();
		m_freeIDs.pop();

		m_t[id] = str;
		return id;
	}

	if (m_size + 1 > m_capacity)
	{
		T* newT = new T[m_capacity * 2];

		memcpy(newT, m_t, m_capacity * sizeof(T));
		
		delete[] m_t;


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
void StructArray<T>::free(int id, const T& replacement)
{
	m_t[id] = replacement;

	m_freeIDs.push(id);
}


template<typename T>
void StructArray<T>::clear()
{
	m_size = 0;
	
	while (!m_freeIDs.empty())
	{
		m_freeIDs.pop();
	}
}


template<typename T>
int StructArray<T>::size()
{
	return m_size;
}




#endif
