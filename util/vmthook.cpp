#include "../stdafx.h"
#include "vmthook.h"

namespace mu
{
	// set default values
	void vmthook::reset()
	{
		m_inst.reset();
		m_oldtbl.reset();
		m_newtbl.reset();

		m_count = m_flags = 0;
	}

	// perform initialization
	bool vmthook::fill (address inst, bool shouldcpy /*= false*/)
	{
		if (inst == nullptr)
			return false;

		m_inst.set(inst);

		// virtual method table
		void **tbl = inst.to<void**>();
		
		if (tbl == nullptr)
			return false;

		m_oldtbl.set(tbl);
		m_flags |= (k_init|k_wrap);

		if ((m_count = getTableCount(tbl)) > 0)
		{
			if (shouldcpy)
				return cpytable(); // allocate new table
		}

		return true;
	}

	// clone the vmt
	bool vmthook::cpytable()
	{
		if (!count())
			return false;

		// allocate and copy
		void **tbl = new void*[m_count];
		m_newtbl.set(tbl);
		memcpy(tbl, m_oldtbl.get(), size());

		if (is_wrapper())
			m_flags &= ~k_wrap;

		return true;
	}

	bool vmthook::hooktable (bool state)
	{
		// make sure we're set up
		if (!can_hook())
			return false;
	
		if (state)
		{
			if (!is_hooked())
			{
				// use our allocated table
				*m_inst.as<void**>() = m_newtbl.get();
				m_flags |= k_hooked;
			}
		}
		else
		{
			if (is_hooked())
			{
				// restore the old pointer
				*m_inst.as<void**>() = m_oldtbl.get();
				m_flags &= ~k_hooked;
			}
		}
	
		return true;
	}

	bool vmthook::hookmethod (address hookFn, WORD index, bool state)
	{
		// valid check
		if (index < 0 || index >= m_count || !can_hook())
			return false;
	
		if (state)
		{
			if (hookFn == nullptr)
				return false;
		
			// replace with our hook
			m_newtbl.as<void**>()[index] = hookFn;
		}
		else
		{
			// restore the original
			m_newtbl.as<void**>()[index] = m_oldtbl.as<void**>()[index];
		}

		return true;
	}

	void vmthook::free()
	{
		if (is_wrapper() || m_newtbl.get() == nullptr)
			return;

		// restore the old table ptr
		hooktable(false);

		// free our table and zero
		delete m_newtbl.get();
		m_newtbl.reset();

		// this is now a function wrapper
		m_flags |= k_wrap;
	}

	word vmthook::getTableCount (void **table)
	{
		word count = 0;

		if (table != nullptr)
		{
			// perform nullptr and page flag check
			for (address pfn; syscall->canReadPointer(pfn = table[count]); count++);
		}

		return count;
	}
};