#pragma once

#include "../../type.h"

using namespace mu;

namespace css
{
	typedef void *HandleEntity;

	class CBaseHandle {
	public:
		CBaseHandle() : m_Index(-1) {}

		bool IsValid() const;
		int ToInt() const;
		int GetEntryIndex() const;
		int GetSerialNumber() const;

		operator ulong();

		CBaseHandle &operator= (const CBaseHandle &h);

	private:
		ulong m_Index;
	};

	inline bool CBaseHandle::IsValid() const
	{
		return m_Index != -1;
	}

	inline int CBaseHandle::ToInt() const
	{
		return (int)m_Index;
	}

	inline int CBaseHandle::GetEntryIndex() const
	{
		return m_Index & 0xffff;
	}

	inline int CBaseHandle::GetSerialNumber() const
	{
		return m_Index >> 12;
	}

	inline CBaseHandle::operator ulong()
	{
		return m_Index;
	}

	inline CBaseHandle &CBaseHandle::operator= (const CBaseHandle &h)
	{
		m_Index = h.m_Index;
		return (*this);
	}
};