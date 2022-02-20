#pragma once

#include "core.h"
#include "client.h"

#define CS_MASK_SHOOT (MASK_SHOT_HULL|CONTENTS_HITBOX)

namespace css
{
	extern bool isValidTarget (CSPlayer &player);
	extern bool isVisible (clientmove *client, CSPlayer &player, vec3 &dest);
	extern bool pathFree (clientmove *client, const vec3 *dest, const vec3 *dir, bool shouldPenetrate = true, int targetNum = -1);
	extern int pathFreeMelee (clientmove *client, const vec3 *dest, const vec3 *dir, int targetNum = -1);
	extern int checkTrigger (clientmove *client, int seed);
	extern void getMaterialParameters (int iMaterial, float &flPenetrationModifier, float &flDamageModifier);
	extern bool traceToExit (const vec3 &src, const vec3 &dir, vec3 &end, float stepSize, float maxDistance);
};