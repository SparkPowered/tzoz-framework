#pragma once

#include "../stdafx.h"
#include <algorithm>

namespace mu
{
	// static array wrapper
	template <typename T, dword N>
	class staticlist {
	public:
		// for sorting
		typedef bool (*cmp_t)(T &a, T &b);

		// instantiate with uninitialized memory
		staticlist()
		{
			reset();
		}
	
		// zero upon destruction
		~staticlist()
		{
			clear();
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

		// copy all contents
		staticlist<T, N> &operator= (const staticlist<T, N> &ls)
		{
			m_count = ls.m_count;
			memmove(&m_data[0], &ls.m_data[0], sizeof m_data);
			return *this;
		}

		// number of elements
		dword count() const
		{
			return m_count;
		}

		// maximum allowed elements
		dword max_count() const
		{
			return N;
		}

		// size of array in bytes
		size_t size() const
		{
			return count() * sizeof T;
		}

		// start of the array
		T *begin()
		{
			return &m_data[0];
		}

		const T *begin() const
		{
			return &m_data[0];
		}

		// final element in the array
		T *back()
		{
			return &m_data[m_count - 1];
		}

		const T *back() const
		{
			return &m_data[m_count - 1];
		}

		// extent of elements
		T *last()
		{
			return &m_data[m_count];
		}

		const T *last() const
		{
			return &m_data[m_count];
		}

		// boundary for array's memory region
		T *end()
		{
			return &m_data[N];
		}

		const T *end() const
		{
			return &m_data[N];
		}

		// increment
		T &push()
		{
			return m_count >= N
				? m_data[m_count - 1] // full
				: m_data[m_count++];
		}

		// decrement
		T &pop()
		{
			return !m_count
				? m_data[0] // nothing to pop
				: m_data[--m_count];
		}

		// push an element to the back of the staticlist
		T *append (T &v)
		{
			if (m_count >= N)
				return nullptr; // staticlist is full
		
			// store at back of the staticlist
			T &r = push() = v;
			return &r;
		}
	
		T *insert (dword index, T &v)
		{
			if (index < 0)
				index = 0; // cap the index
			else if (index > m_count)
				index = m_count;

			push();
			shift_up(index);

			T &r = m_data[index] = v;
			return &r;
		}
	
		bool remove (dword index)
		{
			if (index < 0 || index >= m_count)
				return false; // invalid
		
			shift_down(index);
			pop();
			return true;
		}
	
		// reset element count
		void reset()
		{
			m_count = 0;
		}

		// clear elements
		void zero()
		{
			memset(m_data, 0, sizeof m_data);
		}

		// total wipe
		void clear()
		{
			reset();
			zero();
		}

		// sort the staticlist by a specified priority routine
		void sort (cmp_t fn)
		{
			std::sort(begin(), last(), fn);
		}
	
		void shift_down (dword index)
		{
			for (dword i = index; i < m_count; i++)
				m_data[i] = m_data[i + 1];
		}
	
		void shift_up (dword index)
		{
			for (dword i = m_count; i > index; --i)
				m_data[i] = m_data[i - 1];
		}

		// if this is a list of allocations then delete them to avoid memory leak
		void free_contents()
		{
			for (dword i = 0; i < m_count; delete m_data[i++]);
			clear();
		}
	
	private:
		// number of elements
		dword m_count;
		// array of elements
		T m_data[N];
	};
};