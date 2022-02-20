#pragma once

#include "baseentity.h"

namespace css
{
	enum CSWeaponType
	{
		WEAPONTYPE_KNIFE = 0,	
		WEAPONTYPE_PISTOL,
		WEAPONTYPE_SUBMACHINEGUN,
		WEAPONTYPE_RIFLE,
		WEAPONTYPE_SHOTGUN,
		WEAPONTYPE_SNIPER_RIFLE,
		WEAPONTYPE_MACHINEGUN,
		WEAPONTYPE_C4,
		WEAPONTYPE_GRENADE,
		WEAPONTYPE_UNKNOWN
	};

	enum CSWeaponID
	{
		WEAPON_NONE = 0,
		
		WEAPON_P228,
		WEAPON_GLOCK,
		WEAPON_SCOUT,
		WEAPON_HEGRENADE,
		WEAPON_XM1014,
		WEAPON_C4,
		WEAPON_MAC10,
		WEAPON_AUG,
		WEAPON_SMOKEGRENADE,
		WEAPON_ELITE,
		WEAPON_FIVESEVEN,
		WEAPON_UMP45,
		WEAPON_SG550,
		
		WEAPON_GALIL,
		WEAPON_FAMAS,
		WEAPON_USP,
		WEAPON_AWP,
		WEAPON_MP5NAVY,
		WEAPON_M249,
		WEAPON_M3,
		WEAPON_M4A1,
		WEAPON_TMP,
		WEAPON_G3SG1,
		WEAPON_FLASHBANG,
		WEAPON_DEAGLE,
		WEAPON_SG552,
		WEAPON_AK47,
		WEAPON_KNIFE,
		WEAPON_P90,
		
		WEAPON_SHIELDGUN,
		
		WEAPON_KEVLAR,
		WEAPON_ASSAULTSUIT,
		WEAPON_NVG,
		
		WEAPON_MAX
	};

	// wrapper
	class CSWeaponInfo : public address {
	public:
		CSWeaponInfo();
		CSWeaponInfo (const void *ptr);

		
		int GetBulletType();
		int GetPenetration();
		float GetDamage();
		float GetRange();
		float GetRangeModifier();
		int GetBulletsPerShot();
	};
	
	// caliber
	inline int CSWeaponInfo::GetBulletType()
	{
		return get<size_t>(0x6c0).to<int>();
	}

	// number of penetrable surfaces
	inline int CSWeaponInfo::GetPenetration()
	{
		return get<size_t>(0x884).to<int>();
	}

	// base damage
	inline float CSWeaponInfo::GetDamage()
	{
		return (float)get<size_t>(0x888).to<int>();
	}

	// maximum range
	inline float CSWeaponInfo::GetRange()
	{
		return get<size_t>(0x88c).to<float>();
	}

	// damage reduces over distance
	inline float CSWeaponInfo::GetRangeModifier()
	{
		return get<size_t>(0x890).to<float>();
	}

	// for shotguns
	inline int CSWeaponInfo::GetBulletsPerShot()
	{
		return get<size_t>(0x894).to<int>();
	}

	class CSWeapon : public CBaseEntity {
	public:
		CSWeapon() : CBaseEntity() {}
		CSWeapon (const void *ptr) : CBaseEntity(ptr) {}
		
		void GetWeaponInfo (int &penetration, int &bulletType, float &range, float &rangeModifier, float &damage);
		
		int GetClip1();
		int GetState();
		float GetNextPrimaryAttack();
		float GetNextSecondaryAttack();
		CSWeaponInfo GetWpnData();
		string GetWeaponAlias();

		bool IsValid();
		bool IsHitscan();
		bool IsPistol();
		bool IsSemiAuto();
		bool IsShotgun();
		bool ShouldReload();
		bool IsBurstMode();
		bool IsSecondaryMode();
		float GetBurstInterval();
		float GetNextBurstShot();
		int GetBurstShotsRemaining();

		bool IsKnife();
		int GetWeaponID();
		float GetInaccuracy();
		float GetSpread();
		void UpdateAccuracyPenalty();
	};

	inline bool CSWeapon::IsKnife()
	{
		return GetWeaponID() == WEAPON_KNIFE;
	}

	inline int CSWeapon::GetWeaponID()
	{
		return method<int (__thiscall*)(void*)>(360)(ptr());
	}

	inline float CSWeapon::GetInaccuracy()
	{
		return method<float (__thiscall*)(void*)>(371)(ptr());
	}

	inline float CSWeapon::GetSpread()
	{
		return method<float (__thiscall*)(void*)>(372)(ptr());
	}

	inline void CSWeapon::UpdateAccuracyPenalty()
	{
		method<void (__thiscall*)(void*)>(373)(ptr());
	}
};