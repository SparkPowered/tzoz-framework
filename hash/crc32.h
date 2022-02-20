#pragma once

#include "../stdafx.h"

namespace mu
{
	// Source SDK CRC32 algorithm
	class crc32 {
	public:
		crc32()
		{
			reset();
		}

		crc32 (void *buffer, size_t length)
		{
			get(buffer, length);
		}

		crc32 (string str)
		{
			getstr(str);
		}

		~crc32()
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
			m_hash = -1;
		}

		// get the final value
		ulong finish()
		{
			return (m_hash ^= -1);
		}

		// hash a single buffer of data
		ulong get (void *buffer, size_t length)
		{
			init();
			process(buffer, length);
			return finish();
		}

		// hash a string
		ulong getstr (string str)
		{
			return get(str, str.length());
		}

		// checksum this chunk of memory
		void process (void *buffer, size_t length);

		// for shared seeds
		int seedFileLineHash (int seedValue, string sharedName, int additionalSeed = 0);

	private:
		// the result
		ulong m_hash;

		// no idea what polynomial we're using...
		static ulong s_table[256];
	};
};