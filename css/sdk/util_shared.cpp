#include "../../stdafx.h"
#include "util_shared.h"

#include "../../css/hook/core.h"

namespace css
{
	IterationRetval_t CFlaggedEntitiesEnum::EnumElement (void *entity)
	{
		CSPlayer ent;

		if (entity != nullptr)
		{
			ent.set(entity);

			if (m_flagMask && ent.GetFlags() & m_flagMask)
				return ITERATION_CONTINUE;

			if (m_count >= m_listMax)
				return ITERATION_STOP;

			m_entityList[m_count++] = entity;
		}
		
		return ITERATION_CONTINUE;
	}

	bool CTraceFilterNoPlayers::ShouldHitEntity (void *pHandleEntity, int contentsMask)
	{
		CBaseEntity ent;

		if (pHandleEntity == nullptr)
			return true;

		ent.set(pHandleEntity);

		return !ent.IsPlayer();
	}

	bool CTraceFilterSimple::ShouldHitEntity (void *pHandleEntity, int contentsMask)
	{
		CBaseEntity ent;

		if (!StandardFilterRules(pHandleEntity, contentsMask))
			return false;

		if (m_pPassEnt != nullptr && !PassServerEntityFilter(pHandleEntity, m_pPassEnt))
			return false;

		ent.set(pHandleEntity);
		
		if (!ent.ShouldCollide(m_collisionGroup, contentsMask))
			return false;
		
		if (!g_pGameRules->ShouldCollide(m_collisionGroup, ent.GetCollisionGroup()))
			return false;

		return true;
	}

	bool CTraceFilterSkipTwoEntities::ShouldHitEntity (void *pHandleEntity, int contentsMask)
	{
		if (!PassServerEntityFilter(pHandleEntity, m_pPassEnt2))
			return false;

		return CTraceFilterSimple::ShouldHitEntity(pHandleEntity, contentsMask);
	}

	bool CTraceFilterSimpleList::ShouldHitEntity (void *pHandleEntity, int contentsMask)
	{
		for (auto it = m_passEnts.begin(); it != m_passEnts.end(); ++it)
		{
			if (*it == pHandleEntity)
				return false;
		}

		return CTraceFilterSimple::ShouldHitEntity(pHandleEntity, contentsMask);
	}

	bool CTraceFilterChain::ShouldHitEntity (void *pHandleEntity, int contentsMask)
	{
		bool ret1 = m_filter1 != nullptr
			? m_filter1->ShouldHitEntity(pHandleEntity, contentsMask)
			: true;

		bool ret2 = m_filter2 != nullptr
			? m_filter2->ShouldHitEntity(pHandleEntity, contentsMask)
			: true;

		return ret1 && ret2;
	}

	bool PassServerEntityFilter (void *touchHandle, void *passHandle)
	{
		CBaseEntity touch, pass, touchOwner, passOwner;

		if (passHandle == nullptr)
			return true;

		if (touchHandle == passHandle)
			return false;

		touch.set(touchHandle);
		pass.set(passHandle);

		// don't clip against own missiles
		if (touch.GetOwnerEntity(&touchOwner))
		{
			if (touchOwner.ptr() == passHandle)
				return false;
		}

		// don't clip against owner
		if (pass.GetOwnerEntity(&passOwner))
		{
			if (passOwner.ptr() == touchHandle)
				return false;
		}

		return true;
	}

	bool StandardFilterRules (void *entity, int contentsMask)
	{
		CBaseEntity collide;

		if (entity == nullptr)
			return true;

		collide.set(entity);
		
		char movetype = collide.GetMoveType();
		char solidtype = collide.GetSolidType();
		
		int modtype = modelinfoclient->GetModelType(collide.GetModel());

		if (modtype != mod_brush || (solidtype != SOLID_BSP && solidtype != SOLID_VPHYSICS))
		{
			if (!(contentsMask & CONTENTS_MONSTER))
				return false;
		}

		// cull out tests against see-thru entities
		if (!(contentsMask & CONTENTS_WINDOW) && collide.GetRenderMode() == kRenderTransColor)
			return false;

		// skip entities that can be potentially moved/deleted		
		if (!(contentsMask & CONTENTS_MOVEABLE) && movetype == MOVETYPE_PUSH)
			return false;

		return true;
	}

	void UTIL_TraceLine (const vec3 &src, const vec3 &dest, dword mask, void *ignore, int collisionGroup, trace_t *tr)
	{
		CTraceFilterSimple filter(ignore, collisionGroup);
		UTIL_TraceLine(src, dest, mask, &filter, tr);
	}

	void UTIL_TraceLine (const vec3 &src, const vec3 &dest, dword mask, ITraceFilter *filter, trace_t *tr)
	{
		Ray_t ray;
		ray.Init(src, dest);
		enginetrace->TraceRay(ray, mask, filter, tr);
	}

	void UTIL_TraceHull (const vec3 &src, const vec3 &dest, const vec3 &mins, const vec3 &maxs, dword mask, void *ignore, int collisionGroup, trace_t *tr)
	{
		CTraceFilterSimple filter(ignore, collisionGroup);
		UTIL_TraceHull(src, dest, mins, maxs, mask, &filter, tr);
	}

	void UTIL_TraceHull (const vec3 &src, const vec3 &dest, const vec3 &mins, const vec3 &maxs, dword mask, ITraceFilter *filter, trace_t *tr)
	{
		Ray_t ray;
		ray.Init(src, dest, mins, maxs);
		enginetrace->TraceRay(ray, mask, filter, tr);
	}
	
	// given a position and a ray, return the shortest distance between the two
	// if 'pos' is beyond either end of the ray, the returned distance is negated
	float DistanceToRay (const vec3 &pos, const vec3 &src, const vec3 &dest, float *along /*= nullptr*/, vec3 *pointOnRay /*= nullptr*/)
	{
		vec3 to, dir, onRay;
		
		float length, range, rangeAlong;
		
		to = pos - src;
		dir = dest - src;
		
		length = dir.vectorNormalize();
		rangeAlong = dir.dot(to);

		if (rangeAlong < 0.0f)
		{
			range = -to.length();
			
			if (pointOnRay != nullptr)
				*pointOnRay = src;
		}
		else if (rangeAlong > length)
		{
			range = -(pos - dest).length();
			
			if (pointOnRay != nullptr)
				*pointOnRay = dest;
		}
		else
		{
			onRay = src + dir * rangeAlong;
			range = (pos - onRay).length();
			
			if (pointOnRay != nullptr)
				*pointOnRay = onRay;
		}
		
		return range;
	}
	
	void UTIL_ClipTraceToPlayers (const vec3 &src, const vec3 &dest, dword mask, ITraceFilter *filter, trace_t *tr)
	{
		Ray_t ray;
		
		CBaseEntity player;
		
		trace_t playerTrace;		
		
		float range, smallestFraction = tr->fraction;
		
		ray.Init(src, dest);
		
		for (int i = 1; i < gpGlobals->GetMaxClients(); ++i)
		{
			if (!entlist->GetClientEntity(i, &player))
				continue;
			
			if (player.IsDormant() || !player.IsPlayer() || !player.IsAlive())
				continue;
			
			if (filter != nullptr && !filter->ShouldHitEntity(player.ptr(), mask))
				continue;
				
			range = DistanceToRay(player.WorldSpaceCenter(), src, dest);
			
			if (range < 0.0f || range > 60.0f)
				continue;

			enginetrace->ClipRayToEntity(ray, mask|CONTENTS_HITBOX, player.ptr(), &playerTrace);

			if (playerTrace.fraction >= smallestFraction)
				continue;
			
			memcpy(tr, &playerTrace, sizeof playerTrace);
			smallestFraction = playerTrace.fraction;
		}
	}
};