#pragma once

#include "../stdafx.h"
#include "../util/address.h"

namespace mu
{
	enum md5length_t
	{
		k_md5DigestLength = 16,
		k_md5BitLength = k_md5DigestLength * sizeof byte,
	};

	class md5value {
	public:
		md5value()
		{
			reset();
		}

		md5value (const md5value &v)
		{
			*this = v;
		}

		~md5value()
		{
			reset();
		}

		md5value &operator= (const md5value &v)
		{
			memcpy(m_bits, v.m_bits, k_md5DigestLength);
			return *this;
		}
	
		bool operator== (const md5value &v) const
		{
			return compare(v) == true;
		}

		bool operator!= (const md5value &v) const
		{
			return compare(v) == false;
		}
	
		void reset()
		{
			memset(m_bits, 0, k_md5DigestLength);
		}

		bool iszero() const
		{
			for (size_t i = 0; i < k_md5DigestLength; i++)
			{
				if (m_bits[i] != 0)
					return false;
			}
			
			return true;
		}

		bool compare (const md5value &v) const
		{
			for (size_t i = 0; i < k_md5DigestLength; i++)
			{
				if (m_bits[i] != v.m_bits[i])
					return false;
			}

			return true;
		}
		
		byte m_bits[k_md5DigestLength]; 
	};

	class md5 {
	public:
		md5()
		{
			reset();
		}

		~md5()
		{
			reset();
		}

		void init()
		{
			m_ctx.buf[0] = 0x67452301;
			m_ctx.buf[1] = 0xefcdab89;
			m_ctx.buf[2] = 0x98badcfe;
			m_ctx.buf[3] = 0x10325476;
			m_ctx.bits[0] = 0;
			m_ctx.bits[1] = 0;
		}
	
		void reset()
		{
			memset(&m_ctx, 0, sizeof m_ctx);
		}

		void get (address data, dword len, md5value &digest)
		{
			init();
			update(data.as<byte*>(), len);
			finalize(digest);
		}

		dword pseudorandom (dword seed)
		{
			md5value val;
			get(&seed, sizeof seed, val);
			return *(dword*)(val.m_bits + 6);
		}

		void update (byte *data, dword len);
		void finalize (md5value &digest);

		static void transform (dword buf[4], const dword in[16]);
	
	private:
		union
		{
			dword buf[4];
			dword bits[2];
			dword in[16];
		} m_ctx;
	};
};