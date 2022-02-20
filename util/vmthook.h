#pragma once

#include "address.h"
#include "../win/syscall_w7.h"

namespace mu
{
	template <typename T>
	inline T get_virtual_method (address inst, word fn, word tbl = 0)
	{
		// get the virtual method table and return the desired function
		return inst.get<size_t>(tbl * PTR_SIZE).to<T*>()[fn];
	}

	// stripped vmthook class
	class vmtobject : public address {
	public:
		vmtobject()
		{
			reset();
		}

		vmtobject (const void *ptr)
		{
			set(ptr);
		}

		template <typename T>
		T method (word fn, word tbl = 0) const
		{
			return get_virtual_method<T>(m_ptr, fn, tbl);
		}
	};

	// stripped vmthook class with pseudo-encryption
	class xvmtobject : public xaddress {
	public:
		xvmtobject()
		{
			reset();
		}

		xvmtobject (const void *ptr)
		{
			reset();
			set(ptr);
		}

		template <typename T>
		T method (word fn, word tbl = 0) const
		{
			return get_virtual_method<T>(get(), fn, tbl);
		}
	};
	
	// vmt hook/container class
	// TODO: add support for inherited tables
	class vmthook {
	public:
		enum
		{
			k_null = 0,
			k_hooked = (1 << 0),
			k_wrap = (1 << 1),
			k_init = (1 << 2),
		};

		vmthook()
		{
			reset();
		}

		vmthook (address inst)
		{
			reset();
			fill(inst);
		}

		virtual ~vmthook()
		{
			clear();
		}
		
		virtual void reset();
		virtual bool fill (address inst, bool shouldcpy = false);
		virtual bool cpytable();
		virtual bool hooktable (bool state);
		virtual bool hookmethod (address fn, word index, bool state);
		virtual void free();
		
		void clear()
		{
			free();
			reset();
		}

		// number of virtual methods
		word count() const
		{
			return m_count;
		}

		// current state
		word flags() const
		{
			return m_flags;
		}

		// size of table in bytes
		size_t size() const
		{
			return m_count * PTR_SIZE;
		}

		// the base class
		address inst() const
		{
			return m_inst.get();
		}

		// table pointer replaced
		bool is_hooked() const
		{
			return m_flags & k_hooked;
		}

		// just a container for a class instance
		bool is_wrapper() const
		{
			return m_flags & k_wrap;
		}

		// uninitialized
		bool is_null() const
		{
			return !(m_flags & k_init);
		}

		// we need to set up before we can hook anything
		bool can_hook() const
		{
			return !is_null() && !is_wrapper() && m_count > 0;
		}
		
		// convert to wrapper
		vmtobject object() const
		{
			return (vmtobject&)m_inst.get();
		}

		// convert to pseudo-encrypted wrapper	
		xvmtobject xobject() const
		{
			return (xvmtobject&)m_inst;
		}

		// access the original method table
		template <typename T>
		T old_method (word fn) const
		{
			return m_oldtbl.as<T*>()[fn];
		}

		// count the methods in the table
		static word getTableCount (void **table);

	protected:
		xaddress m_inst, m_oldtbl, m_newtbl;
		word m_count, m_flags;
	};
};