#pragma once

#include "stdafx.h"

namespace mu
{
	// Mersenne Twister PRNG algorithm with uniform distribution
	class mt19937 {
	public:
		enum
		{
			N = 624, // array count
			M = 397, // coefficient
			B = 0xffffffff, // 32-bit bounds			
			HI = 0x80000000, // most significant w-r bits			
			LO = 0x7fffffff, // least significant r bits
		};

		static void set (ulong seed);
		static ulong get();
		static void mem (void *buffer, size_t length);

		// 16-bit
		static word get16()
		{
			return (word)(get() & 0xffff);
		}
		
		// 8-bit
		static byte get8()
		{
			return (byte)(get() & 0xff);
		}

		// real
		static double getreal()
		{
			return ((double)get() / (ulong)B);
		}

	private:		
		static ulong s_mt[N];
		static int s_mti;
	};
};