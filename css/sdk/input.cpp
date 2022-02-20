#include "../../stdafx.h"
#include "input.h"
#include "../../hash/crc32.h"

namespace css
{
	CUserCmd &CUserCmd::operator= (const CUserCmd &cmd)
	{
		command_number = cmd.command_number;
		tick_count = cmd.tick_count;
		viewangles = cmd.viewangles;
		forwardmove = cmd.forwardmove;
		sidemove = cmd.sidemove;
		upmove = cmd.upmove;
		buttons = cmd.buttons;
		impulse = cmd.impulse;
		weaponselect = cmd.weaponselect;
		weaponsubtype = cmd.weaponsubtype;
		random_seed = cmd.random_seed;
		mousedx = cmd.mousedx;
		mousedy = cmd.mousedy;
		hasbeenpredicted = cmd.hasbeenpredicted;

		return *this;
	}

	ulong CUserCmd::GetChecksum()
	{
		crc32 crc;

		crc.init();

		crc.process(&command_number, sizeof command_number);
		crc.process(&tick_count, sizeof tick_count);
		crc.process(&viewangles, sizeof viewangles);
		crc.process(&forwardmove, sizeof forwardmove);
		crc.process(&sidemove, sizeof sidemove);
		crc.process(&upmove, sizeof upmove);
		crc.process(&buttons, sizeof buttons);
		crc.process(&impulse, sizeof impulse);
		crc.process(&weaponselect, sizeof weaponselect);
		crc.process(&weaponsubtype, sizeof weaponsubtype);
		crc.process(&random_seed, sizeof random_seed);
		crc.process(&mousedx, sizeof mousedx);
		crc.process(&mousedy, sizeof mousedy);
		
		return crc.finish();
	}

	CUserCmd *CInput::GetUserCmd (int sequence_number)
	{
		CUserCmd *commands, *ucmd;

		commands = get().get<size_t>(0xc4).to<CUserCmd*>();
		ucmd = &commands[sequence_number % MULTIPLAYER_BACKUP];

		// command_number must be synced
		return ucmd->command_number != sequence_number ? nullptr : ucmd;
	}

	void CInput::VerifyUserCmd (int sequence_number, CUserCmd *ucmd)
	{
		CVerifiedUserCmd *vucmd, *vcommands;

		if (ucmd == nullptr)
			return;
		
		vcommands = get().get<size_t>(0xc8).to<CVerifiedUserCmd*>();
		vucmd = &vcommands[sequence_number % MULTIPLAYER_BACKUP];
		
		// copy that shit
		vucmd->m_cmd = *ucmd;
		vucmd->m_crc = ucmd->GetChecksum();
	}
};