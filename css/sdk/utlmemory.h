#pragma once

namespace css
{
	template <class T>
	class CUtlMemory {
	public:
		T *m_pMemory;
		int m_nAllocationCount;
		int m_nGrowSize;
	};

	template <class T>
	class CUtlVector {
	public:		
		CUtlMemory<T> m_Memory;
		int m_Size;
		T *m_pElements;
	};
};