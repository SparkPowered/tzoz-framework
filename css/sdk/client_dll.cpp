#include "../../stdafx.h"
#include "client_dll.h"

#include "../../css/hook/netvars.h"

namespace css
{
	bool CGameRules::IsFreezePeriod()
	{
		if (inst() == nullptr)
			return false;

		return inst().get<word>(netvars->offsets.m_bFreezePeriod).to<bool>();
	}

	bool CGameRules::ShouldCollide (int collisionGroup0, int collisionGroup1)
	{
		if (inst() == nullptr)
			return false;

		return get_virtual_method<bool (__thiscall*)(void*, int, int)>(inst(), 29)(inst(), collisionGroup0, collisionGroup1);
	}
	
	const CViewVectors *CGameRules::GetViewVectors()
	{
		if (inst() == nullptr)
			return nullptr;
			
		return get_virtual_method<const CViewVectors *(__thiscall*)(void*)>(inst(), 31)(inst());
	}
};