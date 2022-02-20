#include "../stdafx.h"
#include "rand.h"

namespace mu
{
	// array for the state vector
	ulong mt19937::s_mt[N];

	// initialization index
	int mt19937::s_mti = N + 1;

	// set based on seed
	void mt19937::set (ulong seed)
	{
		for (s_mti = 1, s_mt[0] = seed & B; s_mti < N; s_mti++)
			s_mt[s_mti] = (69069 * s_mt[s_mti - 1]) & B;
	}

	// generate random number
	ulong mt19937::get()
	{
		// result
		ulong r;

		// include the constant vector a
		static ulong mag[2] = {0, 0x9908b0df};

		// generate N words at one time
		if (s_mti >= N)
		{
			int i;

			// make sure the table is initialized
			if (s_mti == (N + 1))
				set(TSC16);

			// first
			for (i = 0; i < (N - M); i++)
			{
				r = (s_mt[i] & HI) | (s_mt[i + 1] & LO);
				s_mt[i] = s_mt[i + M] ^ (r >> 1) ^ mag[r & 1];
			}

			// second
			for (; i < (N - 1); i++)
			{
				r = (s_mt[i] & HI) | (s_mt[i + 1] & LO);
				s_mt[i] = s_mt[i + (M - N)] ^ (r >> 1) ^ mag[r & 1];
			}

			// final
			r = (s_mt[N - 1] & HI) | (s_mt[0] & LO);
			s_mt[N - 1] = s_mt[M - 1] ^ (r >> 1) ^ mag[r & 1];
			s_mti = 0;
		}

		// perform tempering
		r = s_mt[s_mti++];		
		r ^= (r >> 11); // U
		r ^= (r << 7) & 0x9d2c5680; // S & B
		r ^= (r << 15) & 0xefc60000; // T & C
		r ^= (r >> 18); // L

		return r;
	}
	
	// apply to a chunk of memory
	void mt19937::mem (void *buffer, size_t length)
	{
		if (buffer == nullptr)
			return;

		// randomize each byte
		for (size_t i = 0; i < length; ((byte*)buffer)[i++] = get8());
	}
};