#include "../stdafx.h"
#include "hash.h"

#include "../util/staticlist.h"

#include "../win/process.h"

namespace mu
{
	// process single set of data
	void hash_t::process (address buffer, size_t length)
	{
		if (buffer == nullptr)
			return;
			
		for (size_t i = 0; i < length; i++)
			m_hash = (m_hash * 33) ^ buffer.as<byte*>()[i];
	}

	// hash a single buffer according to a signature mask
	dword hash_t::getsig (address buffer, dword mask)
	{
		byte data[32];

		if (buffer != nullptr)
		{
			// fill the array according to the mask
			for (size_t i = 0; i < sizeof data; i++)
				data[i] = mask & (1 << i) ? buffer.as<byte*>()[i] : 0;
			
			return get(data, sizeof data);
		}

		return 0;
	}

	address findpattern (address ptr, size_t length, dword signature, dword mask)
	{
		hash_t h;

		if (ptr != nullptr)
		{		
			// iterate the region
			// we don't need to clip the length; our mask will handle that for us
			for (size_t i = 0; i < length; i++)
			{
				// current loc
				address a = ptr.get<size_t>(i);
				
				if (h.getsig(a, mask) == signature)
					return a;
			}
		}

		return nullptr;
	}

	address remotefindpattern (const mu::process &p, address ptr, size_t length, dword signature, dword mask)
	{
		address end, result, adr;

		static byte buf[4096];

		if (ptr != nullptr && length && p.isvalid())
		{
			memset(buf, 0, sizeof buf);

			adr = ptr;
			end = ptr.get<size_t>(length);

			for (;;)
			{
				// remaining bytes to be calculated (for this particular page)
				size_t delta = min(end - adr, sizeof buf);

				if (!p.read(adr, buf, delta))
					break;

				// scan the buffer
				result = findpattern(buf, delta, signature, mask);

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
};