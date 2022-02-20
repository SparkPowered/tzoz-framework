#pragma once

#include "stdafx.h"
#include "util\rand.h"

namespace mu
{
	class address;

	// count the amount of dwords necessary to fit a string
	inline dword strdwordcount (dword stringLength)
	{
		dword r, c;
				
		r = stringLength % sizeof dword; // remainder
		c = (stringLength - r) / sizeof dword; // initial count
		
		return !r ? c : c + 1; // add an extra dword if we have a remainder
	}

	// wrap a multi byte string pointer
	class string {
	public:
		string()
		{
			reset();
		}
		
		string (const char *str)
		{
			*this = str;
		}

		explicit string (const string &s)
		{
			*this = s;
		}

		~string()
		{
			reset();
		}

		// for use in findpattern
		bool sigcmp (address ptr) const;
		
		size_t length() const
		{
			return strlen(m_str);
		}
	
		void reset()
		{
			m_str = nullptr;
		}

		char *lower()
		{
			return strlwr(m_str);
		}

		char *upper()
		{
			return strupr(m_str);
		}

		char *append (const char *str)
		{
			return strcat(m_str, str);
		}

		char *snip (const char *str)
		{
			char *p = find(str);

			if (p != nullptr)
				*p = '\0';

			return m_str;
		}

		char *copy (const char *str)
		{
			return strcpy(m_str, str);
		}

		char *copyto (char *str)
		{
			return strcpy(str, m_str);
		}
	
		char *to (dword i) const
		{
			return &m_str[i];
		}

		char *chr (char c)
		{
			return strchr(m_str, c);
		}

		const char *chr (char c) const
		{
			return strchr(m_str, c);
		}

		char *find (const char *str)
		{
			return strstr(m_str, str);
		}

		const char *find (const char *str) const
		{
			return strstr(m_str, str);
		}

		char *findto (const char *str)
		{
			return strstr(str, m_str);
		}

		const char *findto (const char *str) const
		{
			return strstr(str, m_str);
		}

		int cmp (const char *str) const
		{
			return strcmp(m_str, str, length());
		}

		char *str()
		{
			return m_str;
		}

		const char *str() const
		{
			return m_str;
		}

		operator char*()
		{
			return str();
		}

		operator const char*() const
		{
			return str();
		}

		bool operator== (const string &s) const
		{
			return cmp(s);
		}

		char &at (dword i)
		{
			return m_str[i];
		}

		const char &at (dword i) const
		{
			return m_str[i];
		}

		char &operator[] (dword i)
		{
			return at(i);
		}

		const char &operator[] (dword i) const
		{
			return at(i);
		}

		string &operator= (const char *s)
		{
			m_str = (char*)s;
			return *this;
		}

		string &operator+= (const char *s)
		{
			append(s);
			return *this;
		}

		string &operator-= (const char *s)
		{
			snip(s);
			return *this;
		}
	
	private:
		char *m_str;
	};
	
	// c-style string array of fixed size
	template <dword N>
	class staticstring {
	public:
		staticstring()
		{
			reset();
		}

		staticstring (const char *str)
		{
			*this = str;
		}

		explicit staticstring (const staticstring<N> &s)
		{
			*this = s;
		}

		~staticstring()
		{
			reset();
		}
		
		bool sigcmp (address ptr) const
		{
			return string(m_str).sigcmp(ptr);
		}

		size_t length() const
		{
			return strlen(m_str);
		}
	
		void reset()
		{
			memset(m_str, 0, sizeof m_str);
		}

		char *lower()
		{
			return strlwr(m_str);
		}

		char *upper()
		{
			return strupr(m_str);
		}

		char *append (const char *str)
		{
			return strcat(m_str, str);
		}

		char *snip (const char *str)
		{
			char *p = find(str);

			if (p != nullptr)
				*p = '\0';

			return m_str;
		}

		char *copy (const char *str)
		{
			return strcpy(m_str, str);
		}

		char *copyto (char *str)
		{
			return strcpy(str, m_str);
		}
	
		char *to (dword i) const
		{
			return &m_str[i];
		}

		char *chr (char c)
		{
			return strchr(m_str, c);
		}

		const char *chr (char c) const
		{
			return strchr(m_str, c);
		}

		char *find (const char *str)
		{
			return strstr(m_str, str);
		}

		const char *find (const char *str) const
		{
			return strstr(m_str, str);
		}

		char *findto (const char *str)
		{
			return strstr(str, m_str);
		}

		const char *findto (const char *str) const
		{
			return strstr(str, m_str);
		}

		int cmp (const char *str) const
		{
			return strcmp(m_str, str, length());
		}

		char *str()
		{
			return m_str;
		}

		const char *str() const
		{
			return m_str;
		}

		operator char*()
		{
			return str();
		}

		operator const char*() const
		{
			return str();
		}

		bool operator== (const staticstring<N> &s) const
		{
			return cmp(s);
		}

		char &operator[] (dword i)
		{
			return m_str[i];
		}

		const char &operator[] (dword i) const
		{
			return m_str[i];
		}

		staticstring<N> &operator= (const char *s)
		{
			copy(s);
			return *this;
		}

		staticstring<N> &operator+= (const char *s)
		{
			append(s);
			return *this;
		}

		staticstring<N> &operator-= (const char *s)
		{
			snip(s);
			return *this;
		}
	
	private:
		char m_str[N];
	};

	// VERY UGLY pseudo-encrypted string (maximum length of 256)
	// credits: Badster, Chod
	template <dword N>
	class xstring {
	public:
		xstring()
		{
			reset();
		}

		xstring (string str)
		{
			init(str);
		}

		// init with pre-generated hashes
		xstring (dword length, dword key, ...);

		explicit xstring (const xstring<N> &s)
		{
			*this = s;
		}

		~xstring()
		{
			reset();
		}

		// zero
		void reset();
	
		// perform encrpytion
		void init (string str);

		// perform dectyption
		string decrypt (bool clearstr = false);

		// zero the c-string buffer
		void sreset()
		{
			memset(m_str, 0, sizeof m_str);
		}

		// zero the hash table
		void creset()
		{
			memset(m_table, 0, sizeof m_table);
		}
		
		// length of string
		size_t strlen() const
		{
			return (m_data >> 16) & 0xff;
		}

		// length of hash
		size_t cryptlen() const
		{
			return (m_data >> 8) & 0xff;
		}

		// encryption key
		dword key() const
		{
			return m_data & 0xff;
		}
	
		// copy another instance
		xstring<N> &operator= (const xstring<N> &s)
		{
			m_data = s.m_data;

			memcpy(m_str, s.m_str, N);
			memcpy(m_table, s.m_table, N * sizeof dword);
		}

	private:
		// number of hashes to store
		static dword dwordcount (dword stringLength);

		// each of these args are translated to an 8-bit value
		void setdata (dword length, dword count, dword key);

		// the decrypted buffer
		staticstring<N> m_str;

		// string data (flags)
		dword m_data;

		// encoded hash table
		dword m_table[N / 4];
	};

	template <dword N>
	xstring<N>::xstring<N> (dword length, dword key, ...)
	{
		dword count;
		va_list valist;

		reset();

		va_start(valist, key);

		// set flags
		setdata(length, count = strdwordcount(length), key);

		// fill the hash table
		for (dword i = 0; i < count; m_table[i++] = va_arg(valist, dword));

		va_end(valist);		
	}

	template <dword N>
	void xstring<N>::reset()
	{
		m_data = 0;

		sreset();
		creset();
	}

	template <dword N>
	void xstring<N>::init (string str)
	{
		char c;

		dword length, count, i, j, k, charCount;
		
		reset();

		// setup string data
		count = strdwordcount(length = str.length());
		setdata(length, count, k = mt19937::get8());

		// iterate the table
		for (i = 0, charCount = 0; i < count; i++)
		{
			// in x86, each hash can hold up to 4 chars
			for (j = 0; j < sizeof dword; j++)
			{
				// current char
				c = charCount < length ? str[charCount++] : '\0';

				// encrypt, shift, and shove this char into the hash
				m_table[i] |= (((c ^ k) & 0xff) << (24 - (j * 8)));

				k++;
				k %= 256;
			}
		}
	}

	// get the ANSI string
	template <dword N>
	string xstring<N>::decrypt (bool clearstr /*= false*/)
	{
		dword ch, i, k, h, c, dw;

		// zero the string
		if (clearstr)
			sreset();

		// decrypt once
		if (!m_str.length())
		{
			// iterate hash table
			for (ch = 0, dw = 0, k = key(); dw < cryptlen(); dw++)
			{
				// get each char in this hash
				for (i = 0, h = m_table[dw]; i < sizeof dword; i++)
				{
					// shift and decrypt
					c = h >> (24 - (i * 8));
					c = (c & 0xff) ^ k;

					// store this char
					m_str[ch++] = (char)c;

					// iterate the key
					k++;
					k %= 256;
				}
			}
		
			// zero-terminator
			m_str[strlen()] = '\0';
		}
		
		return m_str;
	}

	template <dword N>
	void xstring<N>::setdata (dword length, dword count, dword key)
	{
		m_data |= ((length & 0xff) << 16);
		m_data |= ((count & 0xff) << 8);
		m_data |= (key & 0xff);
	}
};