#pragma once

#include "../stdafx.h"
#include "rand.h"

namespace mu
{
	class process;

	template <typename T>
	inline size_t offset_fwd (void *start, T value, size_t len)
	{
		for (auto it = (T*)start, end = it+len; it<end; ++it)
			if (*it == value)
				return (size_t)((byte*)it - (byte*)start);

		return -1;
	}

	template <typename T>
	inline size_t offset_rwd (void *start, T value, size_t len)
	{
		for (auto it = (T*)start, end = it-len; it>end; --it)
			if (*it == value)
				return (size_t)((byte*)start - (byte*)it);

		return -1;
	}

	// simple wrapper for a pointer
	class address {
	public:
		address()
		{
			reset();
		}

		address (const void *ptr)
		{
			set(ptr);
		}

		address (ulong addr)
		{
			set(reinterpret_cast<const void*>(addr));
		}

		~address()
		{
			reset();
		}

		// arithmetic
		operator ulong() const
		{
			return as<ulong>();
		}

		// pointer type
		operator void*() const
		{
			return ptr();
		}

		bool operator== (void *p) const
		{
			return m_ptr == p;
		}

		bool operator!= (void *p) const
		{
			return m_ptr != p;
		}

		// the pointer
		void *ptr() const
		{
			return m_ptr;
		}
		
		// null the value
		void reset()
		{
			set(nullptr);
		}

		// set the value
		void set (const void *ptr)
		{
			m_ptr = (void*)ptr;
		}

		// cast to type
		template <typename T>
		T as() const
		{
			return reinterpret_cast<T>(m_ptr);
		}

		// dereference to type
		template <typename T>
		T to() const
		{
			return *reinterpret_cast<T*>(m_ptr);
		}

		// offset the pointer
		template <typename T>
		address get (T offset) const
		{
			return address(as<byte*>() + offset);
		}
		 
		// subtract the pointer
		template <typename T>
		address delta (T offset) const
		{
			return address(as<byte*>() - offset);
		}

		// check if this pointer is within a certain region
		bool within (address ptr, size_t length)
		{
			return m_ptr >= ptr && m_ptr < ptr.get<size_t>(length);
		}

		// count continuing bytes
		int ccbyte (byte b)
		{
			byte *s, *d;
			for (s = d = as<byte*>(); *d == b; d++);
			return (int)(d - s);
		}

		// scan byte signature
		static address findpattern (address ptr, string signature, size_t length);
		static address remotefindpattern (const process &p, address ptr, size_t length, string signature);
		
	protected:
		// the pointer value
		void *m_ptr;
	};

	// pseudo-encrypted pointer
	class xaddress {
	public:
		// make sure the key & offset are generated
		xaddress()
		{
			reset();
		}

		xaddress (const void *ptr)
		{
			reset();
			set(ptr);
		}
		
		~xaddress()
		{
			zero();
		}

		// decode and cast to type
		template <typename T>
		T as() const
		{
			return get().as<T>();
		}

		// decode and dereference to type
		template <typename T>
		T to() const
		{
			return get().to<T>();
		}
				
		xaddress &operator= (const xaddress &a);
		address get() const;
		void set (const void *ptr);
		void reset();

		// complete zero
		void zero()
		{
			m_ptr = m_key = m_offset = 0;
		}
		
	protected:
		uintptr_t m_ptr, m_key, m_offset;
	};

	extern address realloc (address data, size_t size);
};