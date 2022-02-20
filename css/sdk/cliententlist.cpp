#include "../../stdafx.h"
#include "cliententlist.h"

namespace css
{
	bool CClientEntityList::GetClientEntity (int entnum, CBaseEntity *entity)
	{
		if (entity == nullptr)
			return false;

		HandleEntity ptr = GetClientEntity(entnum);

		if (ptr == nullptr)
			return false;

		entity->set(ptr);
		return true;
	}
		
	bool CClientEntityList::GetPlayer (int entnum, CSPlayer *entity)
	{
		if (!GetClientEntity(entnum, entity))
			return false;
				
		return entity->IsPlayer();
	}

	bool CClientEntityList::GetClientEntityFromHandle (CBaseHandle handle, CBaseEntity *entity)
	{
		if (entity == nullptr)
			return false;

		HandleEntity ptr = GetClientEntityFromHandle(handle);

		if (ptr == nullptr)
			return false;

		entity->set(ptr);
		return true;
	}
};