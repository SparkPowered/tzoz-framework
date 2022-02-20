#pragma once

#include "baseentity.h"

namespace css
{
	class CUserCmd;
	class CSWeapon;

	class CPlayerState {
	public:
		virtual ~CPlayerState() {}

		bool deadflag;
		vec3 v_angle;
	};

	class CSPlayer : public CBaseEntity {
	public:
		CSPlayer()
		{
			reset();
		}

		CSPlayer(const void *ptr)
		{
			set(ptr);
		}

		bool GetActiveWeapon (CSWeapon *weapon);
		CPlayerState *GetPlayerState();
		bool GetUseEntity (CBaseEntity *entity);
		int GetHealth();
		char GetLifeState();
		float GetPlayerMaxSpeed();
		int GetFlags();
		int GetArmorValue();
		const vec3 &GetEyeAngles();
		float GetSurfaceFriction();

		void SetFlags (int flags);
		void AddFlags (int flags);
		void RemoveFlags (int flags);

		bool GetHitboxPosition (int hitbox, vec3 &out);

		virtual bool GetHitboxPosition (matrix3x4_t *cache, int hitbox, vec3 &out);
	};

	class CSLocalPlayer : public CSPlayer {
	public:
		CSLocalPlayer()
		{
			reset();
		}

		CSLocalPlayer (const void *ptr)
		{
			set(ptr);
		}

		void GetBulletTypeParameters (int bullet, float &penetrationPower, float &penetrationDist);		

		float GetNextAttack();
		const vec3 &GetViewOffset();
		int GetTickBase();
		bool GetGroundEntity (CBaseEntity *entity);
		float GetStamina();
		bool DidDuck();
		bool IsDucking();
		float GetDucktime();
		float GetFallVelocity();
		const vec3 &GetPunchAngle();
		const vec3 &GetPunchAngleVel();
		CUserCmd *GetCurrentCommand();
		
		void SetViewOffset (const vec3 &off);
		void SetTickBase (int tickbase);
		void SetStamina (float stamina);
		void SetDucked (bool ducked);		
		void SetDucking (bool ducking);		
		void SetDucktime (float ducktime);
		void SetFallVelocity (float vel);
		void SetPunchAngle (const vec3 &punch);
		void SetPunchAngleVel (const vec3 &punch);
		void SetCurrentCommand (CUserCmd *ucmd);
	};

	extern bool getLocalPlayer (CSLocalPlayer *player);
};