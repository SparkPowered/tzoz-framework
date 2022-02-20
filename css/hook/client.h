#pragma once

#include "movedata.h"
#include "../../util/staticlist.h"

namespace css
{
	// handles user commands and client prediction
	class clientmove {
	public:
		enum state_t
		{
			k_null = 0,
			k_active = (1 << 0),
			k_canFire = (1 << 1),
			k_validWeapon = (1 << 2),
			k_isBursting = (1 << 3),
			k_speed = (1 << 4),
		};

		clientmove()
		{
			reset();
		}

		// initialization
		void reset();
		void resetMoveData();
		void init();
		
		// CreateMove
		bool preMove (address ebp, int sequence_number, bool active);
		void postMove();

		// InPrediction
		static bool inPrediction();

		// RunCommand
		void preCommand (void *player, CUserCmd *ucmd, bool isClient);
		void postCommand (void *player, CUserCmd *ucmd);

		// weapon stuff
		bool isFiring();
		bool verifyShot();
		void getSpread (int seed, float &outX, float &outY);

		// correct movement from viewangles
		void fixMove (const vec3 &prevang, const vec3 &newang);	

		static void OnChangeName (IConVar *var, const char *pOldValue, float flOldValue);
		
		int getTickBase() const
		{
			return m_tickBase;
		}

		word getState() const
		{
			return m_state;
		}

		bool isActive() const
		{
			return m_state & k_active;
		}

		bool canFire() const
		{
			return m_state & k_canFire;
		}
		
		bool isWeaponValid() const
		{
			return m_state & k_validWeapon;
		}
		
		bool isBursting() const
		{
			return m_state & k_isBursting;
		}

		int aimTarget() const
		{
			return m_target;
		}

		float getCurtime() const
		{
			return m_curtime;
		}

		// player
		int m_fFlags;
		CSLocalPlayer m_player;
		CSWeapon m_weapon;		

		vec3 m_vieworg;
		vec3 m_viewangles;
		vec3 m_oldviewangles;

		// user command
		CUserCmd *m_ucmd, *m_prevcmd;

		// "name" convar
		ConVar *m_name;
		
	private:
		// RunCommand emulation
		void prePrediction (float &frametime, float &curtime);
		void postPrediction (float frametime, float curtime);
		void predict();
		void itemPostFrame();
	
		// CL_Move modification
		bool getSendPacket();
		void setSendPacket (bool state);		
		
		// features
		void stealName();
		void doSpeed();
		void doBhop();
		void doAim();
		int findTarget();
		void antiSpread();
		void antiRecoil();
		void antiAim (bool safe = false);
		bool isFacingWall (float yaw, vec3 *normal = nullptr);
		void fakeDuck();
		void fakeLag();
		
		movedata &getMoveData (int command_number)
		{
			return m_data[command_number % MULTIPLAYER_BACKUP];
		}
		
		// aimbot
		int m_target;
		int m_lastTarget;
		int m_knifeResult;
		vec3 m_targetPos;

		// stack frame
		address m_ebp;

		// current state
		word m_state;

		// speedhack
		int m_speedCounter;
		int m_fakeLagCounter;

		// tick info
		int command_number;
		int m_tickBase;
		float m_curtime;

		// field of predicted player data for applying correct precision
		staticlist<movedata, MULTIPLAYER_BACKUP> m_data;
	};

	extern clientmove *move;
};