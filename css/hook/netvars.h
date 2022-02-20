#pragma once

#include "../../css/sdk/client_dll.h"
#include "../../css/sdk/input.h"
namespace css
{
	class networkedvars {
	public:
		networkedvars()
		{
			memset(&offsets, 0, sizeof offsets);
		}

		void init();
		void hookProp (dword hash, RecvVarProxyFn fn);
		void crawlTableForProp (dword hash, RecvTable *tbl, RecvProp **out);

		RecvProp *getProp (dword hash);
		int getPropOffset (dword hash);
		
		struct
		{
			// DT_BaseEntity
			word m_flSimulationTime;
			word m_vecOrigin;
			word m_nRenderMode;
			word m_nRenderFX;
			word m_clrRender;
			word m_iTeamNum;
			word m_CollisionGroup;
			word m_hOwnerEntity;
			word m_iParentAttachment;
			word m_Collision;

			// DT_CollisionPropery
			word m_nSolidType;
			word m_usSolidFlags;

			// DT_BaseAnimating
			word m_nSequence;
			word m_nHitboxSet;
			word m_flModelScale;

			// DT_BaseCombatCharacter
			word m_hActiveWeapon;

			// DT_BasePlayer
			word pl;
			word m_hUseEntity;
			word m_iHealth;
			word m_lifeState;
			word m_flMaxspeed;
			word m_fFlags;

			// DT_CSPlayer
			word m_ArmorValue;
			word m_angEyeAngles;

			// DT_BCCLocalPlayerExclusive
			word m_flNextAttack;

			// DT_LocalPlayerExclusive
			word m_Local;
			word m_vecViewOffset;
			word m_nTickBase;
			word m_hGroundEntity;
			word m_vecVelocity;
			word m_vecBaseVelocity;

			// DT_CSLocalPlayerExclusive
			word m_flStamina;
			
			// DT_Local
			word m_bDucked;
			word m_bDucking;
			word m_flDucktime;
			word m_flFallVelocity;
			word m_vecPunchAngle;
			word m_vecPunchAngleVel;

			// DT_BaseCombatWeapon
			word m_iState;

			// DT_LocalWeaponData
			word m_iClip1;

			// DT_LocalActiveWeaponData
			word m_flNextPrimaryAttack;
			word m_flNextSecondaryAttack;

			// DT_CSGameRules
			word m_bFreezePeriod;
			
			// DT_WeaponFamas
			word m_bBurstMode;
			word m_iBurstShotsRemaining;

			// DT_WeaponM4A1
			word m_bSilencerOn;

			// C_BaseEntity
			word m_vecAbsVelocity;
			word m_flSpawnTime;
			word m_flLastMessageTime;
			word m_flGravity;
			word m_MoveType;
			word m_vecNetworkOrigin;

			// C_BasePlayer
			word m_pCurrentCommand;			
			word m_surfaceFriction;			

		} offsets;
	};

	inline int networkedvars::getPropOffset (dword hash)
	{
		RecvProp *prop = getProp(hash);
		return prop == nullptr ? 0 : prop->m_Offset;
	}

	extern networkedvars *netvars;
};