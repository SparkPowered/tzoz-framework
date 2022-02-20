#pragma once

#include "../../util/address.h"
#include "../../util/vec3.h"

using namespace mu;

namespace css
{
	#define MULTIPLAYER_BACKUP 90

	enum
	{
		IN_ATTACK = (1 << 0),
		IN_JUMP = (1 << 1),
		IN_DUCK	= (1 << 2),
		IN_FORWARD = (1 << 3),
		IN_BACK = (1 << 4),
		IN_USE = (1 << 5),
		IN_CANCEL = (1 << 6),
		IN_LEFT = (1 << 7),
		IN_RIGHT = (1 << 8),
		IN_MOVELEFT = (1 << 9),
		IN_MOVERIGHT = (1 << 10),
		IN_ATTACK2 = (1 << 11),
		IN_RELOAD = (1 << 13),
		IN_WALK = (1 << 18),
	};

	class CUserCmd {
	public:
		CUserCmd()
		{
		}
	
		virtual ~CUserCmd()
		{
		}
		
		// copy inst
		CUserCmd &operator= (const CUserCmd &cmd);

		// crc32 of each member
		ulong GetChecksum();

		int command_number;
		int tick_count;
		vec3 viewangles;
		float forwardmove;
		float sidemove;
		float upmove;
		int buttons;
		byte impulse;
		int weaponselect;
		int weaponsubtype;
		int random_seed;
		short mousedx;
		short mousedy;
		bool hasbeenpredicted;
	};

	class CVerifiedUserCmd {
	public:
		CUserCmd m_cmd;
		ulong m_crc;
	};

	class CInput : public xaddress {
	public:
		CInput()
		{
			reset();
		}

		// access m_pCommands
		CUserCmd *GetUserCmd (int sequence_number);
		// copy data and crc32 of the command to the verified structure
		void VerifyUserCmd (int sequence_number, CUserCmd *ucmd);
	};
};