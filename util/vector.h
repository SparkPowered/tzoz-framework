#pragma once

#include "../stdafx.h"
#include "address.h"

#include <algorithm>

namespace mu
{
	template <typename T>
	class vector {
	public:
		// for sorting
		typedef bool (*cmp_t)(T &a, T &b);

		vector()
		{
			reset();
		}

		vector (const vector<T> &vec)
		{
			*this = vec;
		}

		~vector()
		{
			free();
		}

		void reset()
		{
			m_count = 0;
			m_data = nullptr;
		}
		
		// access by index
		T &operator[] (int index)
		{
			return m_data[index];
		}

		const T &operator[] (int index) const
		{
			return m_data[index];
		}

		// check if the array is initialized
		bool is_null()
		{
			return m_data == nullptr;
		}

		// number of elements
		dword count() const
		{
			return m_count;
		}

		// size of array in bytes
		size_t size() const
		{
			return m_count * sizeof T;
		}

		// start of the array
		T *begin()
		{
			return is_null() ? nullptr : &m_data[0];
		}

		const T *begin() const
		{
			return is_null() ? nullptr : &m_data[0];
		}

		// final element in the array
		T *back()
		{
			return is_null() ? nullptr : &m_data[m_count - 1];
		}

		const T *back() const
		{
			return is_null() ? nullptr : &m_data[m_count - 1];
		}

		// boundary for array's memory region
		T *end()
		{
			return is_null() ? nullptr : &m_data[m_count];
		}

		const T *end() const
		{
			return is_null() ? nullptr : &m_data[m_count];
		}
		
		T &push();
		T &pop();
		T *insert (dword index, T &v);

		bool remove (dword index);
		void free_contents();

		// push an element to the back of the list
		T *append (T &v)
		{	
			T &r = push() = v;
			return &r;
		}

		// null the array contents
		void zero()
		{
			if (is_null())
				return;

			memset(m_data, 0, size());
		}

		// sort the list by a specified priority routine
		void sort (cmp_t fn)
		{
			if (is_null())
				return;

			std::sort(begin(), end(), fn);
		}

		// delete the array
		void free()
		{
			if (m_data != nullptr)
				delete m_data;
			
			reset();
		}
		
	private:
		// allocate list if there isn't one already
		void validate (size_t size)
		{
			if (!is_null())
				return;

			m_data = reinterpret_cast<T*>(new byte[size]);
		}

		void shift_down (dword index);
		void shift_up (dword index);

		// number of elements
		dword m_count;

		// array of elements
		T *m_data;
	};

	// grow the list
	template <typename T>
	T &vector<T>::push()
	{
		// add another element
		size_t newsize = size() + sizeof T;

		// allocate
		m_data = is_null()
			? reinterpret_cast<T*>(new byte[newsize])
			: realloc(m_data, newsize).as<T*>();

		return m_data[m_count++];
	}

	// shrink the list
	template <typename T>
	T &vector<T>::pop()
	{
		size_t newsize = size();
		
		if (m_count)
		{
			// remove an element
			newsize -= sizeof T;
			m_data = realloc(m_data, newsize).as<T*>();

			return m_data[--m_count];
		}
		else
		{
			// initialize the list if necessary
			validate(newsize);

			// nothing to pop, return the front of the list
			return m_data[0];
		}
	}

	template <typename T>
	T *vector<T>::insert (dword index, T &v)
	{
		// cap the index
		if (index < 0)
			index = 0;
		else if (index > m_count)
			index = m_count;

		push();
		shift_up(index);

		T &r = m_data[index] = v;
		return &r;
	}

	template <typename T>
	bool vector<T>::remove (dword index)
	{
		if (index < 0 || index >= m_count)
			return false; // invalid
		
		shift_down(index);
		pop();

		return true;
	}

	// if this is a list of allocations then delete them to avoid memory leak
	template <typename T>
	void vector<T>::free_contents()
	{
		if (is_null())
			return;

		for (dword i = 0; i < m_count; delete m_data[i++]);
		
		free();
	}

	template <typename T>
	void vector<T>::shift_down (dword index)
	{
		if (is_null())
			return;

		for (dword i = index; i < m_count; i++)
			m_data[i] = m_data[i + 1];
	}
	
	template <typename T>
	void vector<T>::shift_up (dword index)
	{
		if (is_null())
			return;
		
		for (dword i = m_count; i > index; --i)
			m_data[i] = m_data[i - 1];
	}
};