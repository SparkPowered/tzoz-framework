#pragma once

#include "bspflags.h"
#include "basehandle.h"

#include "../../util/vec3.h"
#include "../../util/vmthook.h"

using namespace mu;

namespace css
{
	typedef float vecaligned_t[4];

	struct surfacedata_t
	{
		char _unk000[0x48];
		word material;
	};

	struct Ray_t
	{
		void Init (const vec3 &src, const vec3 &dest);
		void Init (const vec3 &src, const vec3 &dest, const vec3 &mins, const vec3 &maxs);

		vecaligned_t m_Start;
		vecaligned_t m_Delta;
		vecaligned_t m_Offset;
		vecaligned_t m_Extents;
		bool m_IsRay;
		bool m_IsSwept;
	};

	struct trace_t
	{
		vec3 startpos;
		vec3 endpos;
		vec3 normal;
		float planeDist;
		byte planeType;
		byte signBits;
		char _unk018[2];
		float fraction;
		int contents;
		word dispFlags;
		bool allsolid;
		bool startsolid;
		float fractionleftsolid;
		const char *surfaceName;
		short surfaceProps;
		word surfaceFlags;
		int hitgroup;
		short physicsBone;
		void *m_pEnt;
		int hitbox;
	};

	typedef enum
	{
		TRACE_EVERYTHING = 0,
		TRACE_WORLD_ONLY,				// NOTE: This does *not* test static props!!!
		TRACE_ENTITIES_ONLY,			// NOTE: This version will *not* test static props
		TRACE_EVERYTHING_FILTER_PROPS,	// NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
	} TraceType_t;

	class ITraceFilter {
	public:
		virtual bool ShouldHitEntity (void *entity, int mask) = 0;
		virtual TraceType_t GetTraceType() const = 0;
	};

	class CTraceFilter : public ITraceFilter {
	public:
		virtual TraceType_t GetTraceType() const
		{
			return TRACE_EVERYTHING;	
		}
	};

	class CTraceFilterWorldOnly : public ITraceFilter {
	public:
		virtual bool ShouldHitEntity (void *entity, int mask)
		{
			return false;
		}

		virtual TraceType_t GetTraceType() const
		{
			return TRACE_WORLD_ONLY;
		}
	};

	class CTraceFilterWorldAndPropsOnly : public ITraceFilter {
	public:
		virtual bool ShouldHitEntity (void *entity, int mask)
		{
			return false;
		}

		virtual TraceType_t GetTraceType() const
		{
			return TRACE_EVERYTHING;
		}
	};

	class CTraceFilterHitAll : public CTraceFilter {
	public:
		virtual bool ShouldHitEntity (void *entity, int mask)
		{
			return true;
		}
	};

	class CEngineTrace : public vmthook {
	public:
		CEngineTrace()
		{
			reset();
		}

		int GetPointContents (vec3 &point, void **ppEnt = 0)
		{
			return old_method<int (__thiscall*)(void*, vec3&, void**)>(0)(inst(), point, ppEnt);
		}

		void ClipRayToEntity (const Ray_t &ray, dword mask, void *entity, trace_t *tr)
		{
			old_method<void (__thiscall*)(void*, const Ray_t&, dword, void*, trace_t*)>(2)(inst(), ray, mask, entity, tr);
		}

		void ClipRayToCollideable (const Ray_t &ray, dword mask, void *collide, trace_t *tr)
		{
			old_method<void (__thiscall*)(void*, const Ray_t&, dword, void*, trace_t*)>(3)(inst(), ray, mask, collide, tr);
		}

		void TraceRay (const Ray_t &ray, dword mask, ITraceFilter *filter, trace_t *tr)
		{
			old_method<void (__thiscall*)(void*, const Ray_t&, dword, ITraceFilter*, trace_t*)>(4)(inst(), ray, mask, filter, tr);
		}

		void *GetCollideable (void *entity)
		{
			return old_method<void *(__thiscall*)(void*, void*)>(11)(inst(), entity);
		}
	};

	class CPhysicsSurfaceProps : public vmthook
	{
	public:
		CPhysicsSurfaceProps()
		{
			reset();
		}

		int GetSurfaceIndex (string name)
		{
			return old_method<int (__thiscall*)(void*, char*)>(3)(inst(), name.str());
		}

		surfacedata_t *GetSurfaceData (int index)
		{
			return old_method<surfacedata_t *(__thiscall*)(void*, int)>(5)(inst(), index);
		}
	};

	typedef enum
	{
		ITERATION_CONTINUE = 0,
		ITERATION_STOP,
	} IterationRetval_t;

	typedef enum
	{
		PARTITION_ENGINE_SOLID_EDICTS = (1 << 0), // every edict_t that isn't SOLID_TRIGGER or SOLID_NOT (and static props)
		PARTITION_ENGINE_TRIGGER_EDICTS = (1 << 1), // every edict_t that IS SOLID_TRIGGER
		PARTITION_CLIENT_SOLID_EDICTS = (1 << 2),
		PARTITION_CLIENT_RESPONSIVE_EDICTS = (1 << 3), // these are client-side only objects that respond to being forces, etc.
		PARTITION_ENGINE_NON_STATIC_EDICTS = (1 << 4), // everything in solid & trigger except the static props, includes SOLID_NOTs
		PARTITION_CLIENT_STATIC_PROPS = (1 << 5),
		PARTITION_ENGINE_STATIC_PROPS = (1 << 6),
		PARTITION_CLIENT_NON_STATIC_EDICTS = (1 << 7), // everything except the static props
	} SpatialPartitionListMask_t;

	typedef word SpatialPartitonHandle_t;

	class IPartitionEnumerator {
	public:
		virtual IterationRetval_t EnumElement (void *entity) = 0;
	};

	class CSpatialPartition : public vmthook {
	public:
		CSpatialPartition()
		{
			reset();
		}

		void EnumerateElementsInBox (int mask, vec3 &mins, vec3 &maxs, bool coarseTest, IPartitionEnumerator *iterator)
		{
			old_method<void (__thiscall*)(void*, int, vec3&, vec3&, bool, IPartitionEnumerator*)>(13)(inst(), mask, mins, maxs, coarseTest, iterator);
		}

		void EnumerateElementsAlongRay (int mask, const Ray_t &ray, bool coarseTest, IPartitionEnumerator *iterator)
		{
			old_method<void (__thiscall*)(void*, int, const Ray_t&, bool, IPartitionEnumerator*)>(14)(inst(), mask, ray, coarseTest, iterator);
		}
	};

	class CStaticPropMgr : public vmthook {
	public:
		CStaticPropMgr()
		{
			reset();
		}

		bool IsStaticProp (void *entity)
		{
			return old_method<bool (__thiscall*)(void*, void*)>(3)(inst(), entity);
		}

		bool IsStaticProp (CBaseHandle handle)
		{
			return old_method<bool (__thiscall*)(void*, CBaseHandle)>(4)(inst(), handle);
		}
	};
};