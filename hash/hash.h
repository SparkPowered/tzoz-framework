#pragma once

#include "../stdafx.h"
#include "../util/address.h"

namespace mu
{
	class process;

	// modified DBJ2 algorithm
	class hash_t {
	public:
		hash_t()
		{
			reset();
		}

		hash_t (address buffer, size_t length)
		{
			get(buffer, length);
		}

		hash_t (string str)
		{
			getstr(str);
		}

		~hash_t()
		{
			reset();
		}
		
		// arithmetic
		operator dword() const
		{
			return m_hash;
		}

		// zero the hash
		void reset()
		{
			m_hash = 0;
		}

		// set initial value
		void init()
		{
			m_hash = 5381;
		}

		// hash a single buffer of data
		dword get (address buffer, size_t length)
		{
			init();
			process(buffer, length);
			return m_hash;
		}

		// hash a string
		dword getstr (string s)
		{
			return get(s.str(), s.length());
		}
		
		// compute
		void process (address buffer, size_t length);
		
		// calculate 32-bit signature
		dword getsig (address buffer, dword mask);

	private:
		// the hash value
		dword m_hash;
	};
	
	extern address findpattern (address ptr, size_t length, dword signature, dword mask);
	extern address remotefindpattern (const mu::process &p, address ptr, size_t length, dword signature, dword mask);
};