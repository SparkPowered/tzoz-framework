#pragma once

#include "core.h"

namespace css
{
	// player tick data for prediction
	// credits: Badster
	class movedata {
	public:
		movedata()
		{
			reset();
		}

		void reset();
		void cache (CSLocalPlayer &localPlayer);
		void apply (CSLocalPlayer &localPlayer);
		
		// commmand data
		int command_number;
		bool firstSpeed;

		// player data
		struct
		{
			vec3 punchangle;
			vec3 punchanglevel;
			vec3 viewoffset;
			float stamina;
			float fallvelocity;
		} player;
	};
};