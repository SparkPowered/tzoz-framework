#include "../stdafx.h"
#include "cstring.h"
#include "address.h"

namespace mu
{
	// for use in findpattern
	// this is essentially memcmp modified for use with Tabris-style bytesig/mask
	bool string::sigcmp (address ptr) const
	{
		if (ptr == nullptr)
			return false;
		
		for (size_t i = 0; i < length(); i++)
		{
			// check wildcard before comparison
			if (at(i) != '?' && at(i) != ptr.as<char*>()[i])
				return false;
		}
		
		return true;
	}
};