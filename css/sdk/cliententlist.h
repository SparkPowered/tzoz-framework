#pragma once

#include "basehandle.h"
#include "baseentity.h"
#include "csplayer.h"

namespace css
{
	class CClientEntityList : public vmthook {
	public:
		CClientEntityList() : vmthook() {}

		HandleEntity GetClientEntity (int entnum);
		HandleEntity GetClientEntityFromHandle (CBaseHandle handle);
		int GetHighestEntityIndex();

		bool GetClientEntity (int entnum, CBaseEntity *entity);
		bool GetPlayer (int entnum, CSPlayer *entity);
		bool GetClientEntityFromHandle (CBaseHandle handle, CBaseEntity *entity);
	};

	inline HandleEntity CClientEntityList::GetClientEntity (int entnum)
	{
		return old_method<HandleEntity (__thiscall*)(void*, int)>(3)(inst(), entnum);
	}

	inline HandleEntity CClientEntityList::GetClientEntityFromHandle (CBaseHandle handle)
	{
		return old_method<HandleEntity (__thiscall*)(void*, CBaseHandle)>(4)(inst(), handle);
	}
	
	inline int CClientEntityList::GetHighestEntityIndex()
	{
		return old_method<int (__thiscall*)(void*)>(6)(inst());
	}
};