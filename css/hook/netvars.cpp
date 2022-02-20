#include "../../stdafx.h"
#include "netvars.h"
#include "../../hash/hash.h"
#include "../../css/hook/core.h"

namespace css
{
	static networkedvars _netvars;
	networkedvars *netvars = &_netvars;
	
	void ParticleSmokeGrenade_SpawnTime (const CRecvProxyData *pData, void *pStruct, void *pOut)
	{
		*(float*)pOut = 0.0f;
	}

	void FuncOccluder_Active (const CRecvProxyData *pData, void *pStruct, void *pOut)
	{
		*(bool*)pOut = false;
	}

	void FogController_FogEnable (const CRecvProxyData *pData, void *pStruct, void *pOut)
	{
		*(bool*)pOut = false;
	}

	void CSPlayer_FlashDuration (const CRecvProxyData *pData, void *pStruct, void *pOut)
	{
		*(float*)pOut = 0.0f;
	}

	void CSPlayer_FlashMaxAlpha (const CRecvProxyData *pData, void *pStruct, void *pOut)
	{
		*(float*)pOut = 0.0f;
	}

	void CSPlayer_EyeAnglesX (const CRecvProxyData *pData, void *pStruct, void *pOut)
	{
		float x = pData->m_Value.m_Float;
	
		// normalize first
		if (x > 180.0f)
			x -= 360.0f;
		else if (x < -180.0f)
			x += 360.0f;

		// clamp value to fix animation bug
		if (x > 90.0f || x == -179.912064f)
			x = 90.0f; // they're looking down
		else if (x < -90.0f || x == -179.576702f)
			x = -90.0f; // they're looking up
	
		*(float*)pOut = x;
	}

	void networkedvars::init()
	{
		// DT_BaseEntity		
		offsets.m_flSimulationTime = getPropOffset(0x4db1913a);
		offsets.m_vecOrigin = getPropOffset(0x3d4698f2);
		offsets.m_nRenderMode = getPropOffset(0x6f76dc11);
		offsets.m_nRenderFX = getPropOffset(0xa15daacc);
		offsets.m_clrRender = getPropOffset(0xd6796f01);		
		offsets.m_iTeamNum = getPropOffset(0xabb44074);
		offsets.m_CollisionGroup = getPropOffset(0x6198e177);
		offsets.m_hOwnerEntity = getPropOffset(0x9a60a904);
		offsets.m_iParentAttachment = getPropOffset(0x89df20fa);
		offsets.m_Collision = getPropOffset(0xb3c73388);

		// DT_CollisionProperty
		offsets.m_nSolidType = getPropOffset(0xd7a16fea);
		offsets.m_usSolidFlags = getPropOffset(0xe35785c5);
		
		// DT_BaseAnimating
		offsets.m_nSequence = getPropOffset(0x4c4c89c2);
		offsets.m_nHitboxSet = getPropOffset(0x28474a7f);
		offsets.m_flModelScale = getPropOffset(0x4fe4f16e);

		// DT_BaseCombatCharacter
		offsets.m_hActiveWeapon = getPropOffset(0xad32d424);

		// DT_BCCLocalPlayerExclusive
		offsets.m_flNextAttack = getPropOffset(0xff16338f);

		// DT_BasePlayer
		offsets.pl = getPropOffset(0xb8a10d90);
		offsets.m_hUseEntity = getPropOffset(0xd29b05ce);
		offsets.m_iHealth = getPropOffset(0x4613776b);
		offsets.m_lifeState = getPropOffset(0x49514e4f);
		offsets.m_flMaxspeed = getPropOffset(0x0ba60967);
		offsets.m_fFlags = getPropOffset(0x425c4407);

		// DT_CSPlayer
		offsets.m_ArmorValue = getPropOffset(0xe720f733);
		offsets.m_angEyeAngles = getPropOffset(0x5b93d30e);

		// DT_LocalPlayerExclusive
		offsets.m_Local = getPropOffset(0xfa0c2d25);
		offsets.m_vecViewOffset = getPropOffset(0x8e8f362e);
		offsets.m_nTickBase = getPropOffset(0x28aab0c6);
		offsets.m_hGroundEntity = getPropOffset(0xf13cab3e);
		offsets.m_vecVelocity = getPropOffset(0x2deb3439);
		offsets.m_vecBaseVelocity = getPropOffset(0x3262fcba);
		
		// DT_Local
		offsets.m_bDucked = getPropOffset(0xd60bbc0f);
		offsets.m_bDucking = getPropOffset(0x97832cce);
		offsets.m_flDucktime = getPropOffset(0x07964713);
		offsets.m_flFallVelocity = getPropOffset(0x5f87c68f);
		offsets.m_vecPunchAngle = getPropOffset(0x283b9f44);
		offsets.m_vecPunchAngleVel = getPropOffset(0xd9afb0fb);

		// DT_CSLocalPlayerExclusive
		offsets.m_flStamina = getPropOffset(0xf131ee3f);

		// DT_BaseCombatWeapon
		offsets.m_iState = getPropOffset(0x65204fe7);

		// DT_LocalWeaponData
		offsets.m_iClip1 = getPropOffset(0x1e3a14c9);

		// DT_LocalActiveWeaponData
		offsets.m_flNextPrimaryAttack = getPropOffset(0x36f28722);
		offsets.m_flNextSecondaryAttack = getPropOffset(0x5b820714);		
		
		// DT_CSGameRules		
		offsets.m_bFreezePeriod = getPropOffset(0xaa9687f7);
		
		// DT_WeaponFamas
		offsets.m_bBurstMode = getPropOffset(0x0d35b5c1);
		offsets.m_iBurstShotsRemaining = getPropOffset(0x4c75a2c6);

		// DT_WeaponM4A1
		offsets.m_bSilencerOn = getPropOffset(0xd7d06019);
		
		// C_BaseEntity
		offsets.m_vecAbsVelocity = 0x12c;
		offsets.m_flSpawnTime = 0x14c;
		offsets.m_flLastMessageTime = 0x150;
		offsets.m_flGravity = 0x160;
		offsets.m_MoveType = 0x174;
		offsets.m_vecNetworkOrigin = 0x334;

		// C_BasePlayer
		offsets.m_pCurrentCommand = 0x100c;		
		offsets.m_surfaceFriction = 0x1268;		

		// anti-smoke (first smoke grenade will not count)
		hookProp(0x8aeeebc9, ParticleSmokeGrenade_SpawnTime);
		hookProp(0x3408206d, FuncOccluder_Active);
		hookProp(0xa9694f0b, FogController_FogEnable);

		// anti-flash
		hookProp(0x3c4eea81, CSPlayer_FlashMaxAlpha);
		hookProp(0x87d1abff, CSPlayer_FlashDuration);

		// anti-anti-aim
		hookProp(0x5b93d30e, CSPlayer_EyeAnglesX);
	}

	void networkedvars::hookProp (dword hash, RecvVarProxyFn fn)
	{
		RecvProp *prop = getProp(hash);
			
		if (prop == nullptr)
			return;

		prop->m_ProxyFn = fn;
	}

	RecvProp *networkedvars::getProp (dword hash)
	{
		RecvProp *prop = nullptr;

		// iterate all client classes
		for (ClientClass *cc = g_ClientDLL->GetAllClasses(); cc != nullptr; cc = cc->m_pNext)
		{
			// recursively search the table and its children for the RecvProp
			crawlTableForProp(hash, cc->m_pRecvTable, &prop);
				
			// found it
			if (prop != nullptr)
				break;
		}

		if (prop == nullptr)
		{
			LOG("networkedvars::getProp() failed for hash 0x%x\n", hash);
		}

		return prop;
	}

	void networkedvars::crawlTableForProp (dword hash, RecvTable *table, RecvProp **out)
	{
		hash_t h;
		staticstring<256> buf;

		if (out == nullptr || table == nullptr)
			return;

		for (int i = 0; i < table->m_nProps; i++)
		{
			RecvProp *prop = &table->m_pProps[i];

			// skip this entry
			if (h.getstr(prop->m_pVarName) == 0x349f833e/*baseclass*/)
				continue;

			// use recursion for child tables
			crawlTableForProp(hash, prop->m_pDataTable, out);
				
			if (*out != nullptr)
				break;

			// combine table and prop name so we can use a single hash
			buf = table->m_pNetTableName;
			buf += prop->m_pVarName;

			if (h.getstr(buf.str()) == hash)
			{
				*out = prop;
				LOG("%s -> %s = 0x%x\n", table->m_pNetTableName, prop->m_pVarName, prop->m_Offset);
				break;
			}
		}
	}
};