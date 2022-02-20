#include "../../stdafx.h"
#include "csplayer.h"

#include "../../css\hook/core.h"
#include "../../css\hook/netvars.h"

namespace css
{
	bool CSPlayer::GetActiveWeapon (CSWeapon *weapon)
	{
		CBaseHandle handle = get<word>(netvars->offsets.m_hActiveWeapon).to<CBaseHandle>();
		return entlist->GetClientEntityFromHandle(handle, weapon);
	}
	
	CPlayerState *CSPlayer::GetPlayerState()
	{
		return get<word>(netvars->offsets.pl).as<CPlayerState*>();
	}

	bool CSPlayer::GetUseEntity (CBaseEntity *entity)
	{
		CBaseHandle handle = get<word>(netvars->offsets.m_hUseEntity).to<CBaseHandle>();
		return entlist->GetClientEntityFromHandle(handle, entity);
	}

	int CSPlayer::GetHealth()
	{
		return get<word>(netvars->offsets.m_iHealth).to<int>();
	}

	char CSPlayer::GetLifeState()
	{
		return get<word>(netvars->offsets.m_lifeState).to<char>();
	}

	float CSPlayer::GetPlayerMaxSpeed()
	{
		return get<word>(netvars->offsets.m_flMaxspeed).to<float>();
	}

	int CSPlayer::GetFlags()
	{
		return get<word>(netvars->offsets.m_fFlags).to<int>();
	}

	int CSPlayer::GetArmorValue()
	{
		return get<word>(netvars->offsets.m_ArmorValue).to<int>();
	}

	const vec3 &CSPlayer::GetEyeAngles()
	{
		vec3 *v = get<word>(netvars->offsets.m_angEyeAngles).as<vec3*>();
		return *v;
	}

	float CSPlayer::GetSurfaceFriction()
	{
		return get<word>(netvars->offsets.m_surfaceFriction).to<float>();
	}

	void CSPlayer::SetFlags (int flags)
	{
		*get<word>(netvars->offsets.m_fFlags).as<int*>() = flags;
	}

	void CSPlayer::AddFlags (int flags)
	{
		*get<word>(netvars->offsets.m_fFlags).as<int*>() |= flags;
	}
	
	void CSPlayer::RemoveFlags (int flags)
	{
		*get<word>(netvars->offsets.m_fFlags).as<int*>() &= ~flags;
	}
	
	bool CSPlayer::GetHitboxPosition (int hitbox, vec3 &out)
	{
		return SetupBones(s_boneCache)
			? GetHitboxPosition(s_boneCache, hitbox, out)
			: false;
	}

	bool CSPlayer::GetHitboxPosition (matrix3x4_t *cache, int hitbox, vec3 &out)
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

		if (GetHitboxSet() == 0 && hitbox == HITBOX_HEAD)
		{
			CSLocalPlayer local;

			if (getLocalPlayer(&local) && local.IsAlive())
			{
				vec3 to, from;

				local.GetAbsAngles().angleVectors(&from, nullptr, nullptr);
				GetAbsAngles().angleVectors(&to, nullptr, nullptr);

				// check if they're looking away
				if (from.dot(to) > 0.7f)
				{
					float pitch = GetEyeAngles().x;

					// if they're looking down then aim for the top of their head hitbox
					if (pitch >= 70.0f || pitch <= -70.0f)
						out.z = max.z - 2.0f;
				}
			}
		}

		return true;
	}

	void CSLocalPlayer::GetBulletTypeParameters (int bullet, float &penetrationPower, float &penetrationDist)
	{
		return core->getBulletTypeParameters(m_ptr, bullet, penetrationPower, penetrationDist);
	}

	float CSLocalPlayer::GetNextAttack()
	{
		return get<word>(netvars->offsets.m_flNextAttack).to<float>();
	}

	const vec3 &CSLocalPlayer::GetViewOffset()
	{
		vec3 *v = get<word>(netvars->offsets.m_vecViewOffset).as<vec3*>();
		return *v;
	}

	int CSLocalPlayer::GetTickBase()
	{
		return get<word>(netvars->offsets.m_nTickBase).to<int>();
	}

	bool CSLocalPlayer::GetGroundEntity (CBaseEntity *entity)
	{
		CBaseHandle handle = get<word>(netvars->offsets.m_hGroundEntity).to<CBaseHandle>();
		return entlist->GetClientEntityFromHandle(handle, entity);
	}

	float CSLocalPlayer::GetStamina()
	{
		return get<word>(netvars->offsets.m_flStamina).to<float>();
	}

	bool CSLocalPlayer::DidDuck()
	{
		return get<word>(netvars->offsets.m_Local + netvars->offsets.m_bDucked).to<bool>();
	}

	bool CSLocalPlayer::IsDucking()
	{
		return get<word>(netvars->offsets.m_Local + netvars->offsets.m_bDucking).to<bool>();
	}

	float CSLocalPlayer::GetDucktime()
	{
		return get<word>(netvars->offsets.m_Local + netvars->offsets.m_flDucktime).to<float>();
	}

	float CSLocalPlayer::GetFallVelocity()
	{
		return get<word>(netvars->offsets.m_Local + netvars->offsets.m_flFallVelocity).to<float>();
	}

	const vec3 &CSLocalPlayer::GetPunchAngle()
	{
		vec3 *v = get<word>(netvars->offsets.m_Local + netvars->offsets.m_vecPunchAngle).as<vec3*>();
		return *v;
	}

	const vec3 &CSLocalPlayer::GetPunchAngleVel()
	{
		vec3 *v = get<word>(netvars->offsets.m_Local + netvars->offsets.m_vecPunchAngleVel).as<vec3*>();
		return *v;
	}

	CUserCmd *CSLocalPlayer::GetCurrentCommand()
	{
		return get<word>(netvars->offsets.m_pCurrentCommand).to<CUserCmd*>();
	}

	void CSLocalPlayer::SetViewOffset (const vec3 &off)
	{
		*get<word>(netvars->offsets.m_vecViewOffset).as<vec3*>() = off;
	}
	
	void CSLocalPlayer::SetTickBase (int tickbase)
	{
		*get<word>(netvars->offsets.m_nTickBase).as<int*>() = tickbase;
	}

	void CSLocalPlayer::SetStamina (float stamina)
	{
		*get<word>(netvars->offsets.m_flStamina).as<float*>() = stamina;
	}

	void CSLocalPlayer::SetDucked (bool ducked)
	{
		*get<word>(netvars->offsets.m_Local + netvars->offsets.m_bDucked).as<bool*>() = ducked;
	}

	void CSLocalPlayer::SetDucking (bool ducking)
	{
		*get<word>(netvars->offsets.m_Local + netvars->offsets.m_bDucking).as<bool*>() = ducking;
	}

	void CSLocalPlayer::SetDucktime (float ducktime)
	{
		*get<word>(netvars->offsets.m_Local + netvars->offsets.m_flDucktime).as<float*>() = ducktime;
	}

	void CSLocalPlayer::SetFallVelocity (float vel)
	{
		*get<word>(netvars->offsets.m_Local + netvars->offsets.m_flFallVelocity).as<float*>() = vel;
	}

	void CSLocalPlayer::SetPunchAngle (const vec3 &punch)
	{
		vec3 *v = get<word>(netvars->offsets.m_Local + netvars->offsets.m_vecPunchAngle).as<vec3*>();
		*v = punch;
	}

	void CSLocalPlayer::SetPunchAngleVel (const vec3 &punch)
	{
		vec3 *v = get<word>(netvars->offsets.m_Local + netvars->offsets.m_vecPunchAngleVel).as<vec3*>();
		*v = punch;
	}

	void CSLocalPlayer::SetCurrentCommand (CUserCmd *ucmd)
	{
		*get<word>(netvars->offsets.m_pCurrentCommand).as<CUserCmd**>() = ucmd;
	}

	bool getLocalPlayer (CSLocalPlayer *player)
	{
		return entlist->GetClientEntity(engine->GetLocalPlayer(), player);
	}
};