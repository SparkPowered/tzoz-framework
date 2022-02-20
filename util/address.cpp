#include "../stdafx.h"
#include "address.h"

#include "../util/staticlist.h"

#include "../win/process.h"

namespace mu
{
	address address::findpattern (address ptr, string signature, size_t length)
	{
		if (ptr != nullptr && signature.str() != nullptr)
		{
			// clip the end of the region against our signature
			size_t size = max(length, length - signature.length());
			
			// iterate the region
			for (size_t i = 0; i < size; i++)
			{
				// current loc
				address r = ptr.get<size_t>(i);
				
				// save time by checking against the first byte of the signature
				// credits: fdsasdf
				if (r.to<char>() != signature.at(i))
					continue;
				
				if (signature.sigcmp(r))
					return r;
			}
		}

		return nullptr;
	}

	address address::remotefindpattern (const process &p, address ptr, size_t length, string signature)
	{
		address end, result, adr;

		static byte buf[4096];		

		if (ptr != nullptr && signature != nullptr && length && p.isvalid())
		{
			memset(buf, 0, sizeof buf);

			adr = ptr;
			end = ptr.get<size_t>(length);

			for (;;)
			{
				// remaining bytes to be calculated (for this particular page)
				size_t delta = min((size_t)(end - adr), sizeof buf);
				
				if (!p.read(adr, buf, delta))
					break;

				// scan the buffer
				result = findpattern(buf, signature, delta);

				if (result != nullptr)
				{
					// get the delta and add to the real base address
					result = result.delta<uintptr_t>((uintptr_t)buf);
					result = result.get<uintptr_t>(ptr);

					return result;
				}
			
				// continue to the next chunk
				adr = adr.get<size_t>(delta);

				// check if we reached the total length of the scan
				if (adr.as<uintptr_t>() >= end)
					break;
			}
		}

		return nullptr;
	}

	// copy from another instance
	xaddress &xaddress::operator= (const xaddress &a)
	{
		m_ptr = a.m_ptr;
		m_key = a.m_key;
		m_offset = a.m_offset;

		return *this;
	}
		
	// decode
	address xaddress::get() const
	{
		return !m_ptr ? nullptr : address((m_ptr - m_offset) ^ m_key);
	}

	// encode
	void xaddress::set (const void *ptr)
	{
		if (ptr == nullptr)
			return;

		m_ptr = ((uintptr_t)ptr ^ m_key) + m_offset;
	}

	// generate random key and offset
	void xaddress::reset()
	{
		m_ptr = 0;
		m_key = mt19937::get();
		m_offset = mt19937::get();
	}

	address realloc (address data, size_t size)
	{
		size_t newsize;
		address newdata;
		
		if (data != nullptr)
		{
			newdata = new byte[size];
			newsize = data.as<int*>()[-1] & ~3;

			if (newsize > size)
				newsize = (size + 3) & ~3;

			memmove(newdata, data, newsize);
			delete data;
		}

		return newdata;
	}
};