#include "../../stdafx.h"
#include "csweapon.h"

#include "../../css/hook/core.h"
#include "../../css/hook/netvars.h"

namespace css
{
	CSWeaponInfo::CSWeaponInfo()
	{
		reset();
	}

	CSWeaponInfo::CSWeaponInfo (const void *ptr)
	{
		set(ptr);
	}

	void CSWeapon::GetWeaponInfo (int &penetration, int &bulletType, float &range, float &rangeModifier, float &damage)
	{
		// GetFileWeaponInfoFromHandle(m_hWeaponFileInfo)
		CSWeaponInfo wpnData = GetWpnData();
	
		// initial values
		penetration = wpnData.GetPenetration();
		bulletType = wpnData.GetBulletType();
		range = wpnData.GetRange();
		rangeModifier = wpnData.GetRangeModifier();
		damage = wpnData.GetDamage();
		
		// m4a1, glock18, usp45, famas
		if (IsSecondaryMode())
		{
			switch (GetWeaponID()) {
			case WEAPON_GLOCK:
				rangeModifier = 0.9f;
				damage = 18.0f;
				break;
			case WEAPON_USP:
				damage = 30.0f;
				break;
			case WEAPON_M4A1:
				rangeModifier = 0.95f;
				break;
			}
		}
	}

	int CSWeapon::GetClip1()
	{
		return get<word>(netvars->offsets.m_iClip1).to<int>();
	}

	int CSWeapon::GetState()
	{
		return get<word>(netvars->offsets.m_iState).to<int>();
	}

	float CSWeapon::GetNextPrimaryAttack()
	{
		return get<word>(netvars->offsets.m_flNextPrimaryAttack).to<float>();
	}
	
	float CSWeapon::GetNextSecondaryAttack()
	{
		return get<word>(netvars->offsets.m_flNextSecondaryAttack).to<float>();
	}
	
	CSWeaponInfo CSWeapon::GetWpnData()
	{
		return CSWeaponInfo(core->getWpnData(ptr()));
	}

	string CSWeapon::GetWeaponAlias()
	{
		return core->weaponIDToAlias(GetWeaponID());
	}

	bool CSWeapon::IsValid()
	{
		switch (GetWeaponID()) {
		case WEAPON_HEGRENADE:
		case WEAPON_SMOKEGRENADE:
		case WEAPON_FLASHBANG:
		case WEAPON_C4:
			return false;
		default:
			return true;
		}
	}

	bool CSWeapon::IsHitscan()
	{
		switch (GetWeaponID()) {
		case WEAPON_HEGRENADE:
		case WEAPON_SMOKEGRENADE:
		case WEAPON_FLASHBANG:
		case WEAPON_C4:
		case WEAPON_KNIFE:
			return false;
		default:
			return true;
		}
	}

	bool CSWeapon::IsPistol()
	{
		switch (GetWeaponID()) {
		case WEAPON_DEAGLE:
		case WEAPON_ELITE:
		case WEAPON_FIVESEVEN:
		case WEAPON_GLOCK:
		case WEAPON_USP:
		case WEAPON_P228:
			return true;
		default:
			return false;
		}
	}

	bool CSWeapon::IsSemiAuto()
	{
		switch (GetWeaponID()) {
		case WEAPON_DEAGLE:
		case WEAPON_ELITE:
		case WEAPON_FIVESEVEN:
		case WEAPON_GLOCK:
		case WEAPON_AWP:
		case WEAPON_USP:
		case WEAPON_P228:
		case WEAPON_SCOUT:
		case WEAPON_G3SG1:
		case WEAPON_SG550:
		case WEAPON_M3:
			return true;
		default:
			return false;
		}
	}

	bool CSWeapon::IsShotgun()
	{
		switch (GetWeaponID()) {
		case WEAPON_M3:
		case WEAPON_XM1014:
			return true;
		default:
			return false;
		}
	}

	bool CSWeapon::ShouldReload()
	{
		return IsHitscan() ? GetClip1() < 1 : false;
	}

	bool CSWeapon::IsBurstMode()
	{
		int weaponId = GetWeaponID();

		if (weaponId == WEAPON_FAMAS)
			return get<word>(netvars->offsets.m_bBurstMode).to<bool>();
		else if (weaponId == WEAPON_GLOCK)
			return get<word>(netvars->offsets.m_bBurstMode - 8).to<bool>();

		return false;
	}

	bool CSWeapon::IsSecondaryMode()
	{
		int weaponId = GetWeaponID();

		if (IsBurstMode())
			return true;

		if (weaponId == WEAPON_M4A1)
			return get<word>(netvars->offsets.m_bSilencerOn).to<bool>();
		else if (weaponId == WEAPON_USP)
			return get<word>(netvars->offsets.m_bSilencerOn - 8).to<bool>();

		return false;
	}

	float CSWeapon::GetBurstInterval()
	{
		switch (GetWeaponID()) {
		case WEAPON_GLOCK:
			return 0.06f;
		case WEAPON_FAMAS:
			return 0.075f;
		default:
			return 0.0f;
		}
	}

	float CSWeapon::GetNextBurstShot()
	{
		int weaponId = GetWeaponID();

		if (weaponId == WEAPON_FAMAS)
			return get<word>(netvars->offsets.m_iBurstShotsRemaining + 4).to<float>();
		else if (weaponId == WEAPON_GLOCK)
			return get<word>(netvars->offsets.m_iBurstShotsRemaining - 4).to<float>();

		return 0.0f;
	}

	int CSWeapon::GetBurstShotsRemaining()
	{
		int weaponId = GetWeaponID();

		if (weaponId == WEAPON_FAMAS)
			return get<word>(netvars->offsets.m_iBurstShotsRemaining).to<int>();
		else if (weaponId == WEAPON_GLOCK)
			return get<word>(netvars->offsets.m_iBurstShotsRemaining - 8).to<int>();

		return 0.0f;
	}
};