#pragma once

#include "../../util/vmthook.h"

using namespace mu;

namespace css
{
	class CSteamID {
	public:
		bool IsValid()
		{
			return m_EAccountType != 0 && m_EUniverse != 0;
		}

		dword m_unAccountID : 32;
		dword m_unAccountInstance : 20;
		dword m_EAccountType : 4;
		dword m_EUniverse : 8;
		qword m_unAll64Bits;
	};

	class CSteamFriends : public vmthook {
	public:
		CSteamFriends()
		{
			reset();
		}

		string GetPersonaName()
		{
			return old_method<char *(__thiscall*)(void*)>(0)(inst());
		}

		void SetPersonaName (string name)
		{
			old_method<void (__thiscall*)(void*, char*)>(1)(inst(), name);
		}

		int GetFriendCount (int flags)
		{
			return old_method<int (__thiscall*)(void*, int)>(3)(inst(), flags);
		}

		void GetFriendByIndex (CSteamID &id, int index, int flags)
		{
			old_method<void (__thiscall*)(void*, CSteamID&, int, int)>(4)(inst(), id, index, flags);
		}
	};

	class CSteamClient : public vmthook {
	public:
		CSteamClient()
		{
			reset();
		}

		int CreateSteamPipe()
		{
			return old_method<int (__thiscall*)(void*)>(0)(inst());
		}

		int ConnectToGlobalUser (int pipe)
		{
			return old_method<ulong (__thiscall*)(void*, int)>(2)(inst(), pipe);
		}

		void *GetISteamFriends (int user, int pipe, string version)
		{
			return old_method<void *(__thiscall*)(void*, int, int, char*)>(8)(inst(), user, pipe, version);
		}
	};
};