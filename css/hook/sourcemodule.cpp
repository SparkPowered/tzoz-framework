#include "../../stdafx.h"
#include "sourcemodule.h"
#include "../../hash/hash.h"

namespace css
{
	bool sourcemodule::fill (address base)
	{
		if (!ntmodule::fill(base))
			return false;

		LOG("%s = 0x%x\n", m_name.str(), m_base.get());
		return getList() != nullptr;
	}

	void sourcemodule::reset()
	{
		ntmodule::reset();
		m_list.reset();
	}

	interfacereg_t *sourcemodule::getList()
	{
		if (m_list.get() == nullptr)
		{
			// CreateInterface
			address ci = proc(0x5bba0a84);

			if (ci != nullptr)
			{				
				address jmp = findpattern(ci, 0x10, 0xab376bcc, 0x1);

				// check if this is a jmp gate
				if (jmp != nullptr)
					ci = jmpaddress<void*>(jmp);

				// mov esi, s_pInterfaceRegs
				address mov = findpattern(ci, 0x10, 0x9780941b, 0x3);

				if (mov != nullptr)
				{
					address list = *(void**)(mov.get<size_t>(2).to<void*>());

					if (list != nullptr)
					{
						LOG("%s -> s_pInterfaceRegs = 0x%x\n", m_name.str(), list.ptr());
						m_list.set(list);
					}
				}
			}
		}

		return m_list.as<interfacereg_t*>();
	}

	address sourcemodule::createInterface (dword hash, vmthook *iface, bool shouldcpy /*= false*/, bool includeversion /*= false*/)
	{
		for (interfacereg_t *reg = getList(); reg != nullptr; reg = reg->next)
		{
			size_t length = strlen(reg->name);

			if (includeversion == false)
				length -= 3;

			if (hash_t((byte*)reg->name, length) == hash)
			{
				address inst = reg->createFn();

				LOG("%s -> %s = 0x%x\n", m_name.str(), reg->name, inst.ptr());

				if (iface != nullptr)
					iface->fill(inst, shouldcpy);

				return inst;
			}
		}

		LOG("createInterface for hash 0x%x failed in %s\n", hash, m_name.str());
		return nullptr;
	}
};