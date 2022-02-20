#pragma once

#include "../../util/vmthook.h"
#include "../../util/vec3.h"
#include "../../util/color.h"

#include "basehandle.h"
#include "client_class.h"

using namespace mu;

namespace css
{
	enum csplayer_hitbox_t
	{
		HITBOX_PELVIS,
		HITBOX_LEFTTHIGH,
		HITBOX_LEFTCALF,
		HITBOX_LEFTFOOT,
		HITBOX_LEFTTOE,
		HITBOX_RIGHTTHIGH,
		HITBOX_RIGHTCALF,
		HITBOX_RIGHTFOOT,
		HITBOX_RIGHTTOE,
		HITBOX_SPINE1,
		HITBOX_SPINE2,
		HITBOX_NECK,
		HITBOX_HEAD,
		HITBOX_LEFTUPPERARM,
		HITBOX_LEFTFOREARM,
		HITBOX_LEFTHAND,
		HITBOX_RIGHTUPPERARM,
		HITBOX_RIGHTFOREARM,
		HITBOX_RIGHTHAND,
		HITBOX_COUNT
	};

	typedef struct model_t;
		
	class CBaseEntity : public vmtobject {
	public:
		CBaseEntity() : vmtobject() {}
		CBaseEntity (const void *ptr) : vmtobject(ptr) {}

		bool IsBehind (CBaseEntity &entity);
				
		dword GetClassHash();
		float GetSimulationTime();		
		const vec3 &GetLocalOrigin();
		const vec3 &GetNetworkOrigin();
		char GetRenderMode();
		char GetRenderFX();
		color32 *GetRenderColor();
		int GetTeamNum();
		int GetCollisionGroup();
		bool GetOwnerEntity (CBaseEntity *entity);
		int GetParentAttachment();
		char GetSolidType();
		word GetSolidFlags();
		int	GetSequence();
		int GetHitboxSet();
		float GetModelScale();
		const vec3 &GetVelocity();
		const vec3 &GetBaseVelocity();
		const vec3 &GetAbsVelocity();
		void EstimateAbsVelocity (vec3 &vel);
		float GetSpawnTime();
		float GetLastMessageTime();
		float GetGravity();		
		char GetMoveType();		

		void SetSimulationTime (float simtime);
		void SetLocalOrigin (const vec3 &origin);
		void SetNetworkOrigin (const vec3 &origin);
		void SetMoveType (char type);
		void SetVelocity (const vec3 &vel);
		void SetAbsVelocity (const vec3 &vel);
		void SetBaseVelocity (const vec3 &vel);		
		
		bool IsWeapon();	

		const CBaseHandle &GetRefEHandle();
		const vec3 &GetAbsOrigin();
		const vec3 &GetAbsAngles();

		const model_t *GetModel();
		bool SetupBones (matrix3x4_t *matrix, int maxBones, int mask, float curtime);
		bool SetupBones (matrix3x4_t *cache);
		void GetRenderBoundsWorldSpace (vec3 &mins, vec3 &maxs);

		ClientClass *GetClientClass();
		bool IsDormant();
		int entindex();

		const vec3 &WorldSpaceCenter();
		bool IsAlive();
		bool IsPlayer();
		bool IsBaseCombatCharacter();
		HandleEntity MyCombatCharacterPointer();
		bool IsBaseCombatWeapon();
		HandleEntity MyCombatWeaponPointer();
		const vec3 &EyePosition();
		const vec3 &EyeAngles();
		const vec3 &LocalEyeAngles();
		bool ShouldCollide (int collisionGroup, int contentsMask);

		bool GetBonePosition (int bone, vec3 &out);
		bool GetHitboxPosition (int hitbox, vec3 &out);
		
		virtual bool GetHitboxPosition (matrix3x4_t *cache, int hitbox, vec3 &out);

		static matrix3x4_t s_boneCache[128];
		static bool GetBonePosition (matrix3x4_t *cache, int bone, vec3 &out);
	};

	inline const CBaseHandle &CBaseEntity::GetRefEHandle()
	{
		return method<const CBaseHandle &(__thiscall*)(void*)>(2)(m_ptr);
	}

	inline const vec3 &CBaseEntity::GetAbsOrigin()
	{
		return method<const vec3 &(__thiscall*)(void*)>(9)(m_ptr);
	}
		
	inline const vec3 &CBaseEntity::GetAbsAngles()
	{
		return method<const vec3 &(__thiscall*)(void*)>(10)(m_ptr);
	}

	inline const model_t *CBaseEntity::GetModel()
	{
		return method<const model_t *(__thiscall*)(void*)>(9, 1)(get<size_t>(4));
	}

	inline bool CBaseEntity::SetupBones (matrix3x4_t *matrix, int maxBones, int mask, float curtime)
	{
		return method<bool (__thiscall*)(void*, matrix3x4_t*, int, int, float)>(16, 1)(get<size_t>(4), matrix, maxBones, mask, curtime);
	}

	inline bool CBaseEntity::SetupBones (matrix3x4_t *cache)
	{
		return SetupBones(cache, 128, 0x100, GetSimulationTime());
	}

	inline void CBaseEntity::GetRenderBoundsWorldSpace (vec3 &mins, vec3 &maxs)
	{
		method<void (__thiscall*)(void*, vec3&, vec3&)>(21, 1)(get<size_t>(4), mins, maxs);
	}	

	inline ClientClass *CBaseEntity::GetClientClass()
	{
		return method<ClientClass *(__thiscall*)(void*)>(2, 2)(get<size_t>(8));
	}

	inline bool CBaseEntity::IsDormant()
	{
		return method<bool (__thiscall*)(void*)>(8, 2)(get<size_t>(8));
	}

	inline int CBaseEntity::entindex()
	{
		return method<int (__thiscall*)(void*)>(9, 2)(get<size_t>(8));
	}

	inline const vec3 &CBaseEntity::WorldSpaceCenter()
	{
		return method<const vec3 &(__thiscall*)(void*)>(66)(m_ptr);
	}

	inline bool CBaseEntity::IsAlive()
	{
		return method<bool (__thiscall*)(void*)>(127)(m_ptr);
	}

	inline bool CBaseEntity::IsPlayer()
	{
		return method<bool (__thiscall*)(void*)>(128)(m_ptr);
	}

	inline bool CBaseEntity::IsBaseCombatCharacter()
	{
		return method<bool (__thiscall*)(void*)>(129)(m_ptr);
	}

	inline HandleEntity CBaseEntity::MyCombatCharacterPointer()
	{
		return method<HandleEntity (__thiscall*)(void*)>(130)(m_ptr);
	}

	inline bool CBaseEntity::IsBaseCombatWeapon()
	{
		return method<bool (__thiscall*)(void*)>(134)(m_ptr);
	}

	inline HandleEntity CBaseEntity::MyCombatWeaponPointer()
	{
		return method<HandleEntity (__thiscall*)(void*)>(135)(m_ptr);
	}

	inline const vec3 &CBaseEntity::EyePosition()
	{
		return method<const vec3 &(__thiscall*)(void*)>(138)(m_ptr);
	}
		
	inline const vec3 &CBaseEntity::EyeAngles()
	{
		return method<const vec3 &(__thiscall*)(void*)>(139)(m_ptr);
	}

	inline const vec3 &CBaseEntity::LocalEyeAngles()
	{
		return method<const vec3&(__thiscall*)(void*)>(140)(m_ptr);
	}
		
	inline bool CBaseEntity::ShouldCollide (int collisionGroup, int contentsMask)
	{
		return method<bool (__thiscall*)(void*, int, int)>(142)(m_ptr, collisionGroup, contentsMask);
	}
};