#pragma once

#include "enginetrace.h"
#include "../../util/vector.h"

namespace css
{
	typedef bool (*ShouldHitFunc_t)(void *pHandleEntity, int contentsMask);

	class CFlaggedEntitiesEnum : public IPartitionEnumerator {
	public:
		CFlaggedEntitiesEnum (void **entityList, int listMax, int flagMask)
			: m_entityList(entityList), m_listMax(listMax), m_flagMask(flagMask), m_count(0)
		{}

		virtual IterationRetval_t EnumElement (void *entity);

	private:
		// entity pointer list
		void **m_entityList;

		// list info
		int m_listMax, m_flagMask, m_count;
	};

	class CTraceFilterNoPlayers : public ITraceFilter {
	public:
		virtual bool ShouldHitEntity (void *pHandleEntity, int contentsMask);

		virtual TraceType_t GetTraceType() const
		{
			return TRACE_EVERYTHING;
		}
	};

	class CTraceFilterSimple : public CTraceFilter {
	public:	
		CTraceFilterSimple (void *passentity, int collisionGroup)
		{
			m_pPassEnt = passentity;
			m_collisionGroup = collisionGroup;
		}

		virtual bool ShouldHitEntity (void *pHandleEntity, int contentsMask);

		virtual void SetPassEntity (void *pPassEntity)
		{
			m_pPassEnt = pPassEntity;
		}

		virtual void SetCollisionGroup (int iCollisionGroup)
		{
			m_collisionGroup = iCollisionGroup;
		}

	protected:
		// the entity to skip
		void *m_pPassEnt;

		// for the ShouldCollide call
		int	m_collisionGroup;
	};

	class CTraceFilterSkipTwoEntities : public CTraceFilterSimple {
	public:
		CTraceFilterSkipTwoEntities() : CTraceFilterSimple(nullptr, 0)
		{
			m_pPassEnt2 = nullptr;
		}

		CTraceFilterSkipTwoEntities (void *passentity, void *passentity2, int collisionGroup)
			: CTraceFilterSimple(passentity, collisionGroup)
		{
			m_pPassEnt2 = passentity2;
		}

		virtual bool ShouldHitEntity (void *pHandleEntity, int contentsMask);
		
		virtual void SetPassEntity2 (void *pHandleEntity)
		{
			m_pPassEnt2 = pHandleEntity;
		}

	protected:
		// the other entity to skip
		void *m_pPassEnt2;
	};

	class CTraceFilterSimpleList : public CTraceFilterSimple {
	public:
		CTraceFilterSimpleList (int collisionGroup)
			: CTraceFilterSimple(nullptr, collisionGroup)
		{
			m_passEnts.reset();
		}

		virtual bool ShouldHitEntity (void *pHandleEntity, int contentsMask);

		void AddEntityToIgnore (void *pHandleEntity)
		{
			m_passEnts.append(pHandleEntity);
		}

	protected:
		vector<void*> m_passEnts;
	};

	class CTraceFilterChain : public CTraceFilter {
	public:
		CTraceFilterChain (ITraceFilter *filter1, ITraceFilter *filter2)
			: m_filter1(filter1), m_filter2(filter2)
		{}

		virtual bool ShouldHitEntity (void *pHandleEntity, int contentsMask);

		void SetFilter1 (ITraceFilter *filter)
		{
			m_filter1 = filter;
		}

		void SetFilter2 (ITraceFilter *filter)
		{
			m_filter2 = filter;
		}

	protected:
		ITraceFilter *m_filter1;
		ITraceFilter *m_filter2;
	};

	extern bool PassServerEntityFilter (void *touchHandle, void *passHandle);
	extern bool StandardFilterRules (void *entity, int contentsMask);
	
	extern void UTIL_TraceLine (const vec3 &src, const vec3 &dest, dword mask, void *ignore, int collisionGroup, trace_t *tr);
	extern void UTIL_TraceLine (const vec3 &src, const vec3 &dest, dword mask, ITraceFilter *filter, trace_t *tr);
	extern void UTIL_TraceHull (const vec3 &src, const vec3 &dest, const vec3 &mins, const vec3 &maxs, dword mask, void *ignore, int collisionGroup, trace_t *tr);
	extern void UTIL_TraceHull (const vec3 &src, const vec3 &dest, const vec3 &mins, const vec3 &maxs, dword mask, ITraceFilter *filter, trace_t *tr);
	
	extern float DistanceToRay (const vec3 &pos, const vec3 &src, const vec3 &dest, float *along = nullptr, vec3 *pointOnRay = nullptr);
	
	extern void UTIL_ClipTraceToPlayers (const vec3 &src, const vec3 &dest, dword mask, ITraceFilter *filter, trace_t *tr);
};