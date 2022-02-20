#include "../stdafx.h"
#include "md5.h"

#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))
#define MD5STEP(f, w, x, y, z, data, s) \
	(w += f(x, y, z) + data, w = w<<s | w>>(32-s), w += x)

namespace mu
{
	void md5::update (byte *data, dword len)
	{
		dword t = m_ctx.bits[0];

		// update bitcount		
		if ((m_ctx.bits[0] = t + (len << 3)) < t)
			m_ctx.bits[1]++; // carry from low to high

		m_ctx.bits[1] += (len >> 29);

		// handle any leading odd-sized chunks
		if ((t = (t >> 3) & 0x3f) != 0)
		{
			byte *p = (byte*)((byte*)m_ctx.in + t);

			t = 64 - t;

			if (len < t)
			{
				memcpy(p, data, len);
				return;
			}
		
			memcpy(p, data, t);
			transform(m_ctx.buf, m_ctx.in);
			data += t;
			len -= t;
		}
	
		// process data in 64-byte chunks
		for (; len >= 64; data += 64, len -= 64)
		{
			memcpy(m_ctx.in, data, 64);
			transform(m_ctx.buf, m_ctx.in);
		}
		
		// handle any remaining bytes of data
		memcpy(m_ctx.in, data, len);
	}

	void md5::finalize (md5value &digest)
	{
		byte *p;
		dword count;

		// compute number of bytes mod 64
		count = (m_ctx.bits[0] >> 3) & 0x3f;

		// set the first char of padding to 0x80
		// this is safe since there is always at least one byte free
		p = (byte*)((byte*)m_ctx.in + count);
		*p++ = 0x80;

		// bytes of padding needed to make 64 bytes
		count = 64 - 1 - count;

		// pad out to 56 mod 64
		if (count < 8)
		{
			// two lots of padding; pad the first block to 64 bytes
			memset(p, 0, count);
			transform(m_ctx.buf, m_ctx.in);

			// now fill the next block with 56 bytes
			memset(m_ctx.in, 0, 56);
		}
		else
		{
			// pad block to 56 bytes
			memset(p, 0, count - 8);
		}

		// append length in bits
		m_ctx.in[14] = m_ctx.bits[0];
		m_ctx.in[15] = m_ctx.bits[1];

		// transform
		transform(m_ctx.buf, m_ctx.in);
		memcpy(digest.m_bits, m_ctx.buf, k_md5DigestLength);
		reset();
	}

	void md5::transform (dword buf[4], const dword in[16])
	{
		register unsigned int a, b, c, d;
	
		a = buf[0];
		b = buf[1];
		c = buf[2];
		d = buf[3];

		MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
		MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
		MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
		MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
		MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
		MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
		MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
		MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
		MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
		MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
		MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
		MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
		MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
		MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
		MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
		MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

		MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
		MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
		MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
		MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
		MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
		MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
		MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
		MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
		MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
		MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
		MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
		MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
		MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
		MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
		MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
		MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

		MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
		MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
		MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
		MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
		MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
		MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
		MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
		MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
		MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
		MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
		MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
		MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
		MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
		MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
		MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
		MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

		MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
		MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
		MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
		MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
		MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
		MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
		MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
		MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
		MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
		MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
		MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
		MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
		MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
		MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
		MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
		MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

		buf[0] += a;
		buf[1] += b;
		buf[2] += c;
		buf[3] += d;
	}
};

#undef F1
#undef F2
#undef F3
#undef F4
#undef MD5STEP