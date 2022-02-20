#include "../../stdafx.h"

#include "../../css/hook/core.h"
#include "../../css/hook/client.h"
#include "../../css/hook/netvars.h"

#include "../../hash/hash.h"

namespace css
{
	bool CBaseEntity::IsBehind (CBaseEntity &entity)
	{
		vec3 f, los;

		entity.GetAbsAngles().angleVectors(&f, nullptr, nullptr);

		los = entity.GetAbsOrigin() - GetAbsOrigin();
		los.vectorNormalize2D();

		return los.dot2d(f) > 0.8f;
	}
	
	dword CBaseEntity::GetClassHash()
	{
		return hash_t(GetClientClass()->m_pNetworkName);
	}

	float CBaseEntity::GetSimulationTime()
	{
		return get<word>(netvars->offsets.m_flSimulationTime).to<float>();
	}

	const vec3 &CBaseEntity::GetLocalOrigin()
	{
		vec3 *v = get<word>(netvars->offsets.m_vecOrigin).as<vec3*>();
		return *v;
	}

	const vec3 &CBaseEntity::GetNetworkOrigin()
	{
		vec3 *v = get<word>(netvars->offsets.m_vecNetworkOrigin).as<vec3*>();
		return *v;
	}

	char CBaseEntity::GetRenderMode()
	{
		return get<word>(netvars->offsets.m_nRenderMode).to<char>();
	}

	char CBaseEntity::GetRenderFX()
	{
		return get<word>(netvars->offsets.m_nRenderFX).to<char>();
	}

	color32 *CBaseEntity::GetRenderColor()
	{
		return get<word>(netvars->offsets.m_clrRender).as<color32*>();
	}
	
	int CBaseEntity::GetTeamNum()
	{
		return get<word>(netvars->offsets.m_iTeamNum).to<int>();
	}

	int CBaseEntity::GetCollisionGroup()
	{
		return get<word>(netvars->offsets.m_CollisionGroup).to<int>();
	}

	bool CBaseEntity::GetOwnerEntity (CBaseEntity *entity)
	{
		CBaseHandle handle = get<word>(netvars->offsets.m_hOwnerEntity).to<CBaseHandle>();
		return entlist->GetClientEntityFromHandle(handle, entity);
	}

	int CBaseEntity::GetParentAttachment()
	{
		return get<word>(netvars->offsets.m_iParentAttachment).to<int>();
	}
	
	word CBaseEntity::GetSolidFlags()
	{
		return get<word>(netvars->offsets.m_Collision + netvars->offsets.m_usSolidFlags).to<word>();
	}

	char CBaseEntity::GetSolidType()
	{
		return get<word>(netvars->offsets.m_Collision + netvars->offsets.m_nSolidType).to<char>();
	}

	int CBaseEntity::GetSequence()
	{
		return get<word>(netvars->offsets.m_nSequence).to<int>();
	}

	int CBaseEntity::GetHitboxSet()
	{
		return get<word>(netvars->offsets.m_nHitboxSet).to<int>();
	}

	float CBaseEntity::GetModelScale()
	{
		return get<word>(netvars->offsets.m_flModelScale).to<float>();
	}

	const vec3 &CBaseEntity::GetVelocity()
	{
		vec3 *v = get<word>(netvars->offsets.m_vecVelocity).as<vec3*>();
		return *v;
	}

	const vec3 &CBaseEntity::GetAbsVelocity()
	{
		vec3 *v = get<word>(netvars->offsets.m_vecAbsVelocity).as<vec3*>();
		return *v;
	}
	
	const vec3 &CBaseEntity::GetBaseVelocity()
	{
		vec3 *v = get<word>(netvars->offsets.m_vecBaseVelocity).as<vec3*>();
		return *v;
	}

	void CBaseEntity::EstimateAbsVelocity (vec3 &vel)
	{
		core->estimateAbsVelocity(m_ptr, vel);
	}

	float CBaseEntity::GetSpawnTime()
	{
		return get<word>(netvars->offsets.m_flSpawnTime).to<float>();
	}

	float CBaseEntity::GetLastMessageTime()
	{
		return get<word>(netvars->offsets.m_flLastMessageTime).to<float>();
	}

	float CBaseEntity::GetGravity()
	{
		float gravity = get<word>(netvars->offsets.m_flGravity).to<float>();
		return gravity != 0.0f ? gravity : 1.0f;		
	}

	char CBaseEntity::GetMoveType()
	{
		return get<word>(netvars->offsets.m_MoveType).to<char>();
	}
	
	void CBaseEntity::SetSimulationTime (float simtime)
	{
		*get<word>(netvars->offsets.m_flSimulationTime).as<float*>() = simtime;
	}

	void CBaseEntity::SetLocalOrigin (const vec3 &origin)
	{
		*get<word>(netvars->offsets.m_vecOrigin).as<vec3*>() = origin;		
		SetSimulationTime(move->getCurtime());
	}
	
	void CBaseEntity::SetNetworkOrigin (const vec3 &origin)
	{
		*get<word>(netvars->offsets.m_vecNetworkOrigin).as<vec3*>() = origin;
	}

	void CBaseEntity::SetVelocity (const vec3 &vel)
	{
		*get<word>(netvars->offsets.m_vecVelocity).as<vec3*>() = vel;
	}

	void CBaseEntity::SetAbsVelocity (const vec3 &vel)
	{
		*get<word>(netvars->offsets.m_vecAbsVelocity).as<vec3*>() = vel;
	}
	
	void CBaseEntity::SetBaseVelocity (const vec3 &vel)
	{
		*get<word>(netvars->offsets.m_vecBaseVelocity).as<vec3*>() = vel;
	}
	
	void CBaseEntity::SetMoveType (char type)
	{
		*get<word>(netvars->offsets.m_MoveType).as<char*>() = type;
	}
	
	bool CBaseEntity::IsWeapon()
	{
		hash_t h;

		// we need the RecvTable from this
		ClientClass *cc = GetClientClass();

		if (cc == nullptr)
			return false;

		if (cc->m_pRecvTable == nullptr)
			return false;

		// table name starts with "DT_Weapon"
		if (h.get(cc->m_pRecvTable->m_pNetTableName, 9) == 0xcc0ba568/*DT_Weapon*/)
			return true;

		// grenade check
		switch (h.getstr(cc->m_pRecvTable->m_pNetTableName)) {
		case 0x124da10b/*DT_SmokeGrenade*/:
		case 0x2b4150d9/*DT_HEGrenade*/:
		case 0x286b9090/*DT_Flashbang*/:
			return true;
		default:
			return false;
		}
	}

	bool CBaseEntity::GetBonePosition (int bone, vec3 &out)
	{
		return SetupBones(s_boneCache)
			? GetBonePosition(s_boneCache, bone, out)
			: false;
	}

	bool CBaseEntity::GetHitboxPosition (int hitbox, vec3 &out)
	{
		return SetupBones(s_boneCache)
			? GetHitboxPosition(s_boneCache, hitbox, out)
			: false;
	}

	// credits: Tetsuo
	bool CBaseEntity::GetHitboxPosition (matrix3x4_t *cache, int hitbox, vec3 &out)
	{
		vec3 min, max;
		
		if (cache == nullptr)
			return false;
			
		mstudiobbox_t *box = modelinfoclient->GetStudioBBox(GetModel(), GetHitboxSet(), hitbox);

		if (box == nullptr)
			return false;

		// get the bounds
		min.transform(box->bbmin, cache[box->bone]);
		max.transform(box->bbmax, cache[box->bone]);

		// get the center
		out = (min + max) * 0.5f;

		return true;
	}

	matrix3x4_t CBaseEntity::s_boneCache[128];

	bool CBaseEntity::GetBonePosition (matrix3x4_t *cache, int bone, vec3 &out)
	{
		if (cache == nullptr || bone < 0 || bone >= 128)
			return false;

		out.x = cache[bone][0][3];
		out.y = cache[bone][1][3];
		out.z = cache[bone][2][3];

		return true;
	}
};