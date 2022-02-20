#pragma once

#include "../stdafx.h"

namespace mu
{
	// wrap a wide char string pointer
	class wstring {
	public:
		wstring()
		{
			reset();
		}

		wstring (const wchar_t *str)
		{
			*this = str;
		}

		explicit wstring (const wstring &s)
		{
			*this = s;
		}

		~wstring()
		{
			reset();
		}

		size_t length() const
		{
			return wcslen(m_str);
		}
	
		void reset()
		{
			m_str = nullptr;
		}

		wchar_t *lower()
		{
			return wcslwr(m_str);
		}

		wchar_t *upper()
		{
			return wcsupr(m_str);
		}

		wchar_t *append (const wchar_t *str)
		{
			return wcscat(m_str, str);
		}

		wchar_t *snip (const wchar_t *str)
		{
			wchar_t *p = find(str);

			if (p != nullptr)
				*p = L'\0';

			return m_str;
		}

		wchar_t *copy (const wchar_t *str)
		{
			return wcscpy(m_str, str);
		}

		wchar_t *copyto (wchar_t *str)
		{
			return wcscpy(str, m_str);
		}
	
		wchar_t *to (dword i) const
		{
			return &m_str[i];
		}

		wchar_t *chr (wchar_t c)
		{
			return wcschr(m_str, c);
		}

		const wchar_t *chr (wchar_t c) const
		{
			return wcschr(m_str, c);
		}

		wchar_t *find (const wchar_t *str)
		{
			return wcsstr(m_str, str);
		}

		const wchar_t *find (const wchar_t *str) const
		{
			return wcsstr(m_str, str);
		}

		wchar_t *findto (const wchar_t *str)
		{
			return wcsstr(str, m_str);
		}

		const wchar_t *findto (const wchar_t *str) const
		{
			return wcsstr(str, m_str);
		}

		int cmp (const wchar_t *str) const
		{
			return wcscmp(m_str, str, length());
		}

		wchar_t *str()
		{
			return m_str;
		}

		const wchar_t *str() const
		{
			return m_str;
		}

		operator wchar_t*()
		{
			return str();
		}

		operator const wchar_t*() const
		{
			return str();
		}

		bool operator== (const wstring &s) const
		{
			return cmp(s);
		}

		wchar_t &operator[] (dword i)
		{
			return m_str[i];
		}

		const wchar_t &operator[] (dword i) const
		{
			return m_str[i];
		}

		wstring &operator= (const wchar_t *s)
		{
			m_str = (wchar_t*)s;
			return *this;
		}

		wstring &operator+= (const wchar_t *s)
		{
			append(s);
			return *this;
		}

		wstring &operator-= (const wchar_t *s)
		{
			snip(s);
			return *this;
		}
	
	private:
		wchar_t *m_str;
	};
	
	// unicode string array of fixed size
	template<dword N>
	class staticwstring {
	public:
		staticwstring()
		{
			reset();
		}

		staticwstring (const wchar_t *str)
		{
			copy(str);
		}

		explicit staticwstring (const staticwstring<N> &s)
		{
			*this = s;
		}

		~staticwstring()
		{
			reset();
		}

		size_t length() const
		{
			return wcslen(m_str);
		}
	
		void reset()
		{
			memset(m_str, 0, sizeof m_str);
		}

		wchar_t *lower()
		{
			return wcslwr(m_str);
		}

		wchar_t *upper()
		{
			return wcsupr(m_str);
		}

		wchar_t *append (const wchar_t *str)
		{
			return wcscat(m_str, str);
		}

		wchar_t *snip (const wchar_t *str)
		{
			wchar_t *p = find(str);

			if (p != nullptr)
				*p = L'\0';

			return m_str;
		}

		wchar_t *copy (const wchar_t *str)
		{
			return wcscpy(m_str, str);
		}

		wchar_t *copyto (wchar_t *str)
		{
			return wcscpy(str, m_str);
		}
	
		wchar_t *to (dword i) const
		{
			return &m_str[i];
		}

		wchar_t *chr (wchar_t c)
		{
			return wcschr(m_str, c);
		}

		const wchar_t *chr (wchar_t c) const
		{
			return wcschr(m_str, c);
		}

		wchar_t *find (const wchar_t *str)
		{
			return wcsstr(m_str, str);
		}

		const wchar_t *find (const wchar_t *str) const
		{
			return wcsstr(m_str, str);
		}

		wchar_t *findto (const wchar_t *str)
		{
			return wcsstr(str, m_str);
		}

		const wchar_t *findto (const wchar_t *str) const
		{
			return wcsstr(str, m_str);
		}

		int cmp (const wchar_t *str) const
		{
			return wcscmp(m_str, str, length());
		}

		wchar_t *str()
		{
			return m_str;
		}

		const wchar_t *str() const
		{
			return m_str;
		}

		operator wchar_t*()
		{
			return str();
		}

		operator const wchar_t*() const
		{
			return str();
		}

		bool operator== (const staticwstring<N> &s) const
		{
			return cmp(s);
		}

		wchar_t &operator[] (dword i)
		{
			return m_str[i];
		}

		const wchar_t &operator[] (dword i) const
		{
			return m_str[i];
		}

		operator wstring()
		{
			return (wstring&)m_str;
		}

		staticwstring<N> &operator= (const wchar_t *s)
		{
			copy(s);
			return *this;
		}

		staticwstring<N> &operator+= (const wchar_t *s)
		{
			append(s);
			return *this;
		}

		staticwstring<N> &operator-= (const wchar_t *s)
		{
			snip(s);
			return *this;
		}
	
	private:
		wchar_t m_str[N];
	};
};