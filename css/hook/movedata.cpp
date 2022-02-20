#include "../../stdafx.h"
#include "movedata.h"

namespace css
{
	// zero
	void movedata::reset()
	{
		command_number = 0;
		firstSpeed = false;

		player.punchangle.zero();
		player.punchanglevel.zero();
		player.viewoffset.zero();
		player.stamina = 0.0f;
		player.fallvelocity = 0.0f;
	}
	
	// cache the currently predicted data
	void movedata::cache (CSLocalPlayer &localPlayer)
	{
		player.punchangle = localPlayer.GetPunchAngle();
		player.punchanglevel = localPlayer.GetPunchAngleVel();
		player.viewoffset = localPlayer.GetViewOffset();
		player.stamina = localPlayer.GetStamina();
		player.fallvelocity = localPlayer.GetFallVelocity();
	}
	
	// set the previously predicted data
	void movedata::apply (CSLocalPlayer &localPlayer)
	{
		localPlayer.SetPunchAngle(player.punchangle);
		localPlayer.SetPunchAngleVel(player.punchanglevel);
		//localPlayer.SetViewOffset(player.viewoffset);
		localPlayer.SetStamina(player.stamina);
		localPlayer.SetFallVelocity(player.fallvelocity);
	}
}