#include "../../stdafx.h"
#include "trace.h"
#include "client.h"
#include "settings.h"
#include "../../win/imports.h"
#include "../../css/sdk/util_shared.h"

namespace css
{
	bool isValidTarget (CSPlayer &player)
	{
		CSLocalPlayer local;

		if (player == nullptr || !getLocalPlayer(&local))
			return false;

		// ignore ourselves
		if (player.entindex() == engine->GetLocalPlayer())
			return false;

		// alive player within PVS
		if (player.IsDormant() || !player.IsPlayer() || !player.IsAlive())
			return false;

		// friendly fire
		if (!settings->aim.team->m_value)
		{
			if (local.GetTeamNum() == player.GetTeamNum())
				return false;
		}
		
		// Steam friends
		if (settings->aim.friends->m_value)
		{
			if (core->isfriend(player.entindex()))
				return false;
		}

		// spawn protection for deathmatch servers
		// this may not work for every plugin
		if (settings->aim.deathmatch->m_value)
		{
			if (player.GetRenderMode() != kRenderTransColor)
			{
				if (player.GetHealth() >= 500 || player.GetRenderColor()->a < 255)
					return false;
			}
			else
			{
				return false;
			}
		}

		return true;
	}

	// prioritized hitboxes
	static int s_hitboxes[HITBOX_COUNT] =
	{
		HITBOX_HEAD,
		HITBOX_NECK,
		HITBOX_SPINE1,
		HITBOX_SPINE2,
		HITBOX_PELVIS,
		HITBOX_LEFTUPPERARM,
		HITBOX_LEFTFOREARM,
		HITBOX_LEFTHAND,
		HITBOX_RIGHTUPPERARM,
		HITBOX_RIGHTFOREARM,
		HITBOX_RIGHTHAND,
		HITBOX_LEFTTHIGH,
		HITBOX_LEFTCALF,
		HITBOX_LEFTFOOT,
		HITBOX_LEFTTOE,
		HITBOX_RIGHTTHIGH,
		HITBOX_RIGHTCALF,
		HITBOX_RIGHTFOOT,
		HITBOX_RIGHTTOE
	};
	
	// proxy bullet trace for multipoint aiming
	bool isVisible (clientmove *client, CSPlayer &player, vec3 &dest)
	{
		vec3 pos;
		
		int maxCount;

		bool shouldPenetrate;

		if (player != nullptr && player.SetupBones(CBaseEntity::s_boneCache))
		{
			shouldPenetrate = settings->aim.autowall->m_value == 1;
			maxCount = settings->aim.body->m_value == 1 ? HITBOX_COUNT : 1;

			for (int i = 0; i < maxCount; i++)
			{
				if (player.GetHitboxPosition(CBaseEntity::s_boneCache, s_hitboxes[i], pos))
				{
					if (pathFree(client, &pos, nullptr, shouldPenetrate, player.entindex()))
					{
						dest = pos;
						return true;
					}
				}
			}
		}

		return false;
	}

	// perform a full trace for our bullet
	bool pathFree (clientmove *client, const vec3 *dest, const vec3 *dir, bool shouldPenetrate /*= true*/, int targetNum /*= -1*/)
	{
		trace_t tr, exitTr;		

		int iPenetration, bulletType;
		
		CTraceFilterSkipTwoEntities filter;
		
		CSPlayer hitEntity, ignoreEntity;
		
		vec3 start, end, forward, penetrationExit;		
		
		float traceLength, currentLength, penetrationModifier, damageModifier,
			maxRange, rangeModifier, damage, penetrationPower, penetrationDist;
		
		// we need to set up this data first
		if (!client->isActive() || !client->isWeaponValid())
			return false;

		client->m_weapon.GetWeaponInfo(iPenetration, bulletType, maxRange, rangeModifier, damage);
		client->m_player.GetBulletTypeParameters(bulletType, penetrationPower, penetrationDist);
		
		start = client->m_vieworg;
				
		// determine the shot direction
		if (dest != nullptr)
		{
			forward = *dest - start;
			traceLength = forward.vectorNormalize();

			// too far
			if (traceLength >= maxRange)
				return false;
		}
		else
		{
			// we dont have a direction to shoot in
			if (dir == nullptr)
				return false;
			
			forward = *dir;
			traceLength = maxRange;
		}
		
		while ((int)damage > 0)
		{
			// ignore ourselves and the last player we hit
			filter.SetPassEntity(client->m_player.ptr());
			filter.SetPassEntity2(ignoreEntity.ptr());

			end = start + forward * maxRange;

			// trace and shave off for player bounds
			UTIL_TraceLine(start, end, CS_MASK_SHOOT, &filter, &tr);
			UTIL_ClipTraceToPlayers(start, end + forward * 40.0f, CS_MASK_SHOOT, &filter, &tr);
			
			if (dir == nullptr)
			{
				// ghetto way to check if we reached our target
				if (client->m_vieworg.dist(tr.endpos) >= traceLength)
					return true;
			}
			
			if (tr.m_pEnt != nullptr)
			{
				hitEntity.set(tr.m_pEnt);
			
				// only players
				if (hitEntity.IsPlayer())
				{
					if ((targetNum == -1 && isValidTarget(hitEntity)) ||
						(targetNum != -1 && hitEntity.entindex() == targetNum))
					{
						return true;
					}

					// ignore the player for next trace
					ignoreEntity.set(tr.m_pEnt);
				}
			}	

			// we didn't hit anything
			if (tr.fraction == 1.0f)
				break;
			
			bool hitGrate = (tr.contents & CONTENTS_GRATE) != 0;

			// allow exception for grate materials
			if (!shouldPenetrate && !hitGrate)
				break;

			currentLength += tr.fraction * maxRange;
			damage *= imports->pow(rangeModifier, currentLength * 0.002);
			
			if (currentLength > penetrationDist && iPenetration > 0)
				iPenetration = 0;

			// grate will allow an extra penetration
			if (!iPenetration && !hitGrate)
				break;

			if (iPenetration < 0)
				break;			
			
			surfacedata_t *surface = physprops->GetSurfaceData(tr.surfaceProps);

			if (surface == nullptr)
				break;

			int iMaterial = (int)surface->material;

			// some railings in de_inferno are CONTENTS_GRATE but CHAR_TEX_CONCRETE
			// we'll trust the CONTENTS_GRATE and use a high damage modifier
			if (hitGrate)
			{
				// if we're a concrete grate (TOOLS/TOOLSINVISIBLE texture) allow more penetrating power
				penetrationModifier = 1.0f;
				damageModifier = 0.99f;
			}
			else
			{
				getMaterialParameters(iMaterial, penetrationModifier, damageModifier);
			}
			
			// try to penetrate object, maximum penetration is 128 inch
			if (!traceToExit(tr.endpos, forward, penetrationExit, 24.0f, 128.0f))
				break;

			// find exact penetration exit
			UTIL_TraceLine(penetrationExit, tr.endpos, CS_MASK_SHOOT, nullptr, &exitTr);
			
			// something was blocking, trace again
			if (exitTr.m_pEnt != nullptr && exitTr.m_pEnt != tr.m_pEnt)
				UTIL_TraceLine(penetrationExit, tr.endpos, CS_MASK_SHOOT, exitTr.m_pEnt, COLLISION_GROUP_NONE, &exitTr);
			
			hitGrate = hitGrate && (exitTr.contents & CONTENTS_GRATE) != 0;

			// get material at exit point
			surface = physprops->GetSurfaceData(exitTr.surfaceProps);

			if (surface == nullptr)
				continue;

			// if enter & exit point is wood or metal we assume this is 
			// a hollow crate or barrel and give a penetration bonus
			if (iMaterial == (int)surface->material)
			{
				if (iMaterial == CHAR_TEX_WOOD || iMaterial == CHAR_TEX_METAL)
					penetrationModifier += penetrationModifier;
			}

			float thickness = tr.endpos.dist(exitTr.endpos);

			// check if bullet has enough power to penetrate this distance for this material			
			if (thickness > (penetrationPower * penetrationModifier))
				break;

			// save the exit point of the bullet
			start = exitTr.endpos;

			// iterate
			penetrationPower -= thickness / penetrationModifier;
			currentLength += thickness;
			maxRange = (maxRange - currentLength) * 0.5f;
			damage *= damageModifier;
			iPenetration--;
		}

		return false;
	}

	int pathFreeMelee (clientmove *client, const vec3 *dest, const vec3 *dir, int targetNum /*= -1*/)
	{
		int dmg;
		bool first;
		float range;

		trace_t tr;		
		vec3 fwd, end;

		CSPlayer entity;		
		
		// collision bounds
		static vec3 maxs(16.0f, 16.0f, 18.0f), mins(-16.0f, -16.0f, -18.0f);

		if (!client->isActive() || !client->isWeaponValid())
			return 0;
		
		if (dest != nullptr)
		{
			// get the direction to our target loc
			fwd = *dest - client->m_vieworg;
			fwd.vectorNormalize();
		}
		else
		{
			// we need a direction to trace in
			if (dir == nullptr)
				return 0;

			fwd = *dir;
		}
		
		// first swing
		first = client->m_weapon.GetNextPrimaryAttack() + 0.4f < client->getCurtime();
		
		for (int i = 0; i < 2; i++)
		{
			// stab first, then slash
			range = i > 0 ? 48.0f : 32.0f;
			end = client->m_vieworg + fwd * range;
			
			// trace a line first
			UTIL_TraceLine(client->m_vieworg, end, MASK_SOLID, client->m_player.ptr(), COLLISION_GROUP_NONE, &tr);
			
			// no hit
			if (tr.fraction == 1.0f)
			{			
				// trace forward with respect to collision bounds
				UTIL_TraceHull(client->m_vieworg, end, mins, maxs, MASK_SOLID, client->m_player.ptr(), COLLISION_GROUP_NONE, &tr);
				
				// no hit
				if (tr.fraction == 1.0f)
					continue;
			}
			
			if (tr.m_pEnt == nullptr)
				continue;
			
			entity.set(tr.m_pEnt);

			if (targetNum != -1)
			{
				// is this our target?
				if (targetNum != entity.entindex())
					continue;
			}
			else
			{
				// okay, make sure that we can attack them
				if (!isValidTarget(entity))
					continue;
			}

			// are we doing a swing?
			if (i > 0)
			{
				// get swing damage
				dmg = entity.GetArmorValue() > 0 ? 17 : 20;

				// subsequent swings do less damage
				if (first)
					dmg -= 5;

				// does the player have enough health to constitute a swing?
				// we want to prioritize stabs as our primary attack
				if (entity.GetHealth() > dmg)
					continue;
			}

			// return the proper attack flags
			if (i == 0)
				return IN_ATTACK2;
			else if (i == 1)
				return IN_ATTACK;
		}

		return 0;
	}

	// nospread/norecoil triggerbot w/autowall
	int checkTrigger (clientmove *client, int seed)
	{
		float x, y;

		vec3 f, r, u, dir, va, punch;

		if (!client->isActive() || !client->isWeaponValid())
			return 0;
		
		client->getSpread(seed, x, y);
		client->m_ucmd->viewangles.angleVectors(&f, &r, &u);

		if ((x != 0.0f || y != 0.0f) && !settings->aim.antispread->m_value)
		{
			// apply spread
			dir = f + (r * x) + (u * y);
			dir.vectorNormalize();
		}
		else
		{
			// spread is zero, get the forward vector
			dir = f;
		}

		if (!client->m_weapon.IsKnife())
		{
			dir.vectorAngles(va);
			va.angleNormalize();

			// apply recoil
			if (!settings->aim.antirecoil->m_value)
			{
				va += client->m_player.GetPunchAngle() * 2.0f;
				va.angleNormalize();
			}

			va.angleVectors(&dir, nullptr, nullptr);

			// hitscan check
			if (pathFree(client, nullptr, &dir, settings->aim.autowall->m_value == 1))
				return IN_ATTACK;
		}
		else
		{
			return pathFreeMelee(client, nullptr, &dir);
		}

		return 0;
	}

	void getMaterialParameters (int iMaterial, float& flPenetrationModifier, float& flDamageModifier)
	{
		switch (iMaterial) {
		case CHAR_TEX_DIRT:
		case CHAR_TEX_METAL:
			flPenetrationModifier = 0.5f;
			flDamageModifier = 0.3f;
			break;
		case CHAR_TEX_CONCRETE:
			flPenetrationModifier = 0.4f;
			flDamageModifier = 0.25f;
			break;
		case CHAR_TEX_GRATE:
			flPenetrationModifier = 1.0f;
			flDamageModifier = 0.99f;
			break;
		case CHAR_TEX_VENT:
			flPenetrationModifier = 0.5f;
			flDamageModifier = 0.45f;
			break;
		case CHAR_TEX_TILE:
			flPenetrationModifier = 0.65f;
			flDamageModifier = 0.3f;
			break;
		case CHAR_TEX_COMPUTER:
			flPenetrationModifier = 0.4f;
			flDamageModifier = 0.45f;
			break;
		case CHAR_TEX_WOOD:
			flPenetrationModifier = 1.0f;
			flDamageModifier = 0.6f;
			break;
		default:
			flPenetrationModifier = 1.0f;
			flDamageModifier = 0.5f;
			break;
		}
	}

	bool traceToExit (const vec3 &src, const vec3 &dir, vec3 &end, float stepSize, float maxDistance)
	{
		float distance = 0.0f;

		while (distance <= maxDistance)
		{
			distance += stepSize;
			end = src + dir * distance;

			if (!(enginetrace->GetPointContents(end) & MASK_SOLID))
				return true;
		}

		return false;
	}
};