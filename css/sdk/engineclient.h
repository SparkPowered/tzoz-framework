#pragma once

#include "../../util/vmthook.h"
#include "../../util/vec3.h"

#include "const.h"

using namespace mu;

namespace css
{
	struct player_info_t
	{
		char name[MAX_PLAYER_NAME_LENGTH];
		int	userID;
		char guid[SIGNED_GUID_LEN + 1];
		dword friendsID;
		char friendsName[MAX_PLAYER_NAME_LENGTH];
		bool fakeplayer;
		bool ishltv;
		bool isreplay;
		ulong customFiles[MAX_CUSTOM_FILES];
		byte filesDownloaded;
	};

	class CEngineClient : public vmthook {
	public:
		CEngineClient()
		{
			reset();
		}
		
		void GetScreenSize (int &w, int &h)
		{
			old_method<void (__thiscall*)(void*, int&, int&)>(5)(inst(), w, h);
		}

		void ClientCmd (string args)
		{
			old_method<void (__thiscall*)(void*, char*)>(7)(inst(), args.str());
		}

		bool GetPlayerInfo (int entnum, player_info_t *pi)
		{
			return old_method<bool (__thiscall*)(void*, int, void*)>(8)(inst(), entnum, pi);
		}

		int GetLocalPlayer()
		{
			return old_method<int (__thiscall*)(void*)>(12)(inst());
		}

		void GetViewAngles (vec3 &angles)
		{
			old_method<void (__thiscall*)(void*, vec3&)>(19)(inst(), angles);
		}

		void SetViewAngles (vec3 &angles)
		{
			old_method<void (__thiscall*)(void*, vec3&)>(20)(inst(), angles);
		}

		bool IsInGame()
		{
			return old_method<bool (__thiscall*)(void*)>(26)(inst());
		}

		bool IsConnected()
		{
			return old_method<bool (__thiscall*)(void*)>(27)(inst());
		}

		const VMatrix &WorldToScreenMatrix()
		{
			return old_method<const VMatrix &(__thiscall*)(void*)>(36)(inst());
		}
	};
};