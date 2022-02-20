#pragma once

#include "../../util/vmthook.h"

using namespace mu;

namespace css
{
	class CMoveHelper : public vmthook {
	public:
		CMoveHelper()
		{
			reset();
		}

		void SetHost (void *player)
		{
			old_method<void (__thiscall*)(void*, void*)>(1)(inst(), player);
		}
	};

	class CGameMovement : public vmthook {
	public:
		CGameMovement()
		{
			reset();
		}

		void ProcessMovement (void *player, void *movedata)
		{
			old_method<void (__thiscall*)(void*, void*, void*)>(1)(inst(), player, movedata);
		}
	};

	class CUserCmd;

	class CPrediction : public vmthook {
	public:
		CPrediction()
		{
			reset();
		}

		bool InPrediction (void *thisptr)
		{
			return old_method<bool (__thiscall*)(void*)>(14)(thisptr);
		}

		void RunCommand (void *thisptr, void *player, CUserCmd *ucmd, void *movehelper)
		{
			old_method<void (__thiscall*)(void*, void*, void*, void*)>(17)(thisptr, player, ucmd, movehelper);
		}

		void SetupMove (void *player, CUserCmd *ucmd, void *movehelper, void *movedata)
		{
			old_method<void (__thiscall*)(void*, void*, void*, void*, void*)>(18)(inst(), player, ucmd, movehelper, movedata);
		}

		void FinishMove (void *player, CUserCmd *ucmd, void *movedata)
		{
			old_method<void (__thiscall*)(void*, void*, void*, void*)>(19)(inst(), player, ucmd, movedata);
		}
	};
};