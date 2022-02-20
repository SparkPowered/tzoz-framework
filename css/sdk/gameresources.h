#pragma once

#include "../../util/color.h"

using namespace mu;

namespace css
{
	class IGameResources {
	public:
		virtual	~IGameResources() {};	
		virtual char *GetTeamName (int index) = 0;
		virtual int	GetTeamScore (int index) = 0;
		virtual color32 GetTeamColor (int index) = 0;
		virtual bool IsConnected (int index) = 0;
		virtual bool IsAlive (int index) = 0;
		virtual bool IsFakePlayer (int index) = 0;
		virtual bool IsLocalPlayer (int index) = 0;
		virtual char *GetPlayerName (int index) = 0;
		virtual int GetPlayerScore (int index) = 0;
		virtual int	GetPing (int index) = 0;
		virtual int	GetDeaths (int index) = 0;
		virtual int	GetFrags (int index) = 0;
		virtual int	GetTeam (int index) = 0;
		virtual int GetHealth (int index) = 0;
	};
};