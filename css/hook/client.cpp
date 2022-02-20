#include "../../hash/hash.h"
#include "client.h"
#include "trace.h"
#include "gui.h"
#include "settings.h"

#include "../../win/imports.h"

#include "../../css/sdk/util_shared.h"

namespace css
{
	static clientmove _move;
	clientmove *move = &_move;

	void clientmove::reset()
	{
		m_name = nullptr;

		m_fFlags = 0;
		m_player.reset();
		m_weapon.reset();

		m_vieworg.zero();
		m_viewangles.zero();
		m_oldviewangles.zero();

		m_ucmd = m_prevcmd = nullptr;

		m_ebp.reset();

		m_state = k_null;

		m_speedCounter = 0;
		m_fakeLagCounter = 0;

		command_number = 0;
		m_tickBase = 0;
		m_curtime = 0.0f;

		m_target = -1;
		m_lastTarget = -1;
		m_knifeResult = 0;
		m_targetPos.zero();

		resetMoveData();
	}
	
	void clientmove::resetMoveData()
	{
		m_data.reset();
		
		for (int i = 0; i < m_data.max_count(); i++)
		{
			m_data[i].reset();
			m_data.push();
		}
	}

	static bool nameChangeAllowed (const char *pOldValue)
	{	
		static xstring<8> defaultValue(/*unnamed*/ 0x07, 0x31, 0x445C5D55, 0x58535300);
		
		// allow engine to set default name
		if (!defaultValue.decrypt().cmp(pOldValue))
			return true;

		uintptr_t retnadr = core->cvarCallbackReturnAddress().as<uintptr_t>();

		// check if the console is calling our callback
		if (offset_fwd<uintptr_t>(_AddressOfReturnAddress(), retnadr, 100) != -1)
			return true;

		return false;
	}

	// credits: Casual_Hacker
	void clientmove::OnChangeName (IConVar *var, const char *pOldValue, float flOldValue)
	{
		if (nameChangeAllowed(pOldValue))
			return;

		// otherwise, prevent the name change
		strcpy(((ConVar*)var)->m_pszString, pOldValue);
	}

	void clientmove::init()
	{
		static xstring<4> _name(/*name*/ 0x04, 0xB5, 0xDBD7DADD);

		if (m_name == nullptr)
			m_name = cvar->FindVar(_name.decrypt());

		if (m_name != nullptr)
			m_name->m_fnChangeCallback = &OnChangeName;
	}

	void clientmove::stealName()
	{
		int index;

		CSPlayer ent;
		
		if (m_name != nullptr)
		{
			// get a random player that isn't us
			do index = mt19937::get() % gpGlobals->GetMaxClients();
			while (index == engine->GetLocalPlayer());

			// make sure they are a player that isn't on our Steam friends list
			if (!core->isfriend(index) && entlist->GetPlayer(index, &ent))
			{
				IGameResources *gr = core->getGameResources();

				// ignore bots and unconnected players
				if (!gr->IsFakePlayer(index) && gr->IsConnected(index))
				{
					staticstring<MAX_PLAYER_NAME_LENGTH>
						playerName(gr->GetPlayerName(index));
					
					// we need space for our invisible unicode string
					if (playerName.length() <= (MAX_PLAYER_NAME_LENGTH - 3))
						m_name->SetValue(playerName.append("\xe2\x81\xa3"));
				}
			}
		}
	}

	// initialize data prior to CreateMove call
	bool clientmove::preMove (address ebp, int sequence_number, bool active)
	{
		m_state = k_null;

		// SMAC will kick for any invisible unicode characters
		if (!settings->misc.antismac->m_value && settings->misc.namesteal->m_value)
			stealName();

		if (!active || g_pGameRules->IsFreezePeriod())
			return false;
		
		// update vars
		command_number = sequence_number;
		m_ebp = ebp;
		m_curtime = gpGlobals->GetCurtime();
		m_oldviewangles = m_viewangles;
		engine->GetViewAngles(m_viewangles);
		m_target = -1;
		m_knifeResult = 0;

		setSendPacket(true);

		// update player
		if (!getLocalPlayer(&m_player))
			return false;
		
		// save m_fFlags for when we predict
		m_fFlags = m_player.GetFlags();

		// we need to be alive...
		if (!m_player.IsAlive())
			return false;

		m_state |= k_active;

		return true;
	}

	// perform user command manipulation	
	void clientmove::postMove()
	{
		if (!isActive())
			return;
		
		m_prevcmd = input->GetUserCmd(command_number - 1);//m_ucmd;
		m_ucmd = input->GetUserCmd(command_number);

		// command_number not synced?
		if (m_ucmd == nullptr)
			return;

		getMoveData(m_ucmd->command_number).firstSpeed = false;

		if (settings->misc.speedhack->m_value && inputsystem->IsButtonDown(KEY_LSHIFT))
			doSpeed();

		doBhop();
		
		if (settings->hvh.fakeduck->m_value)
			fakeDuck();
		
		// weapon will be validated during our own prediction
		if (m_player.GetActiveWeapon(&m_weapon))
		{
			if (m_weapon.IsValid())			
				predict();
		}

		// now we can do our hack shit
		if (isWeaponValid())
		{
			if (settings->aim.enable->m_value)
				doAim();
			
			if (isFiring())
			{
				if (m_weapon.IsHitscan())
				{
					// silent aim
					setSendPacket(!canFire());
					
					if (settings->aim.antispread->m_value)
						antiSpread();
						
					if (settings->aim.antirecoil->m_value)
						antiRecoil();

					fixMove(m_viewangles, m_ucmd->viewangles);
				}
			}
			else
			{
				if (settings->hvh.antiaim->m_value && m_player.GetMoveType() != MOVETYPE_LADDER)
					antiAim(settings->misc.antismac->m_value == 1);
			}
		}

		if (settings->hvh.fakelag->m_value)
			fakeLag();
		
		// track tickbase manually
		++m_tickBase;

		// do checksum
		input->VerifyUserCmd(command_number, m_ucmd);
	}
	
	// set values needed for ProcessMovement
	void clientmove::prePrediction (float &frametime, float &curtime)
	{
		frametime = gpGlobals->GetFrametime();
		curtime = gpGlobals->GetCurtime();
		
		m_curtime = (float)m_tickBase * gpGlobals->GetIntervalPerTick();
		
		gpGlobals->SetFrametime(gpGlobals->GetIntervalPerTick());
		gpGlobals->SetCurtime(m_curtime);
		
		m_player.SetCurrentCommand(m_ucmd);
		
		if (!(m_fFlags & FL_ONGROUND))
			m_player.SetFallVelocity(-m_player.GetAbsVelocity().z);
			
		core->setPredictionRandomSeed(m_ucmd->random_seed);
		
		movehelper->SetHost(m_player.ptr());
	}
	
	// restore values after ProcessMovement
	void clientmove::postPrediction (float frametime, float curtime)
	{
		m_player.SetCurrentCommand(nullptr);
		m_player.SetAbsVelocity(m_player.GetVelocity());
		
		core->setPredictionRandomSeed(-1);		
		
		gpGlobals->SetFrametime(frametime);
		gpGlobals->SetCurtime(curtime);
		
		m_vieworg = m_player.EyePosition();
		
		movehelper->SetHost(nullptr);
	}
	
	// update player data in CreateMove
	void clientmove::predict()
	{
		static byte data[256];

		float frametime, curtime;		
		
		if (m_prevcmd != nullptr && m_prevcmd->hasbeenpredicted)
		{
			// tickbase has been iterated by RunCommand
			// now we can sync with the game
			m_tickBase = m_player.GetTickBase();

			// apply the predicted data
			preCommand(m_player.ptr(), m_ucmd, true);
		}
		
		// setup for prediction
		prePrediction(frametime, curtime);

		memset(data, 0, sizeof data);
		
		// predict
		prediction->SetupMove(m_player.ptr(), m_ucmd, movehelper->inst(), data);
		gamemovement->ProcessMovement(m_player.ptr(), data);
		prediction->FinishMove(m_player.ptr(), m_ucmd, data);
		
		// finish prediction
		postPrediction(frametime, curtime);
		
		// update weapon params
		itemPostFrame();
	}
	
	void clientmove::itemPostFrame()
	{
		if (!isActive() || m_weapon == nullptr)
			return;

		// disable attacking while our menu is open
		if (gui->m_menu.m_active)
		{
			if (m_ucmd->buttons & IN_ATTACK)
				m_ucmd->buttons &= ~IN_ATTACK;

			if (m_ucmd->buttons & IN_ATTACK2)
				m_ucmd->buttons &= ~IN_ATTACK2;
		}

		if (!m_weapon.ShouldReload())
		{
			// if we're speedhacking then only fire on the initial tick
			bool canShoot = m_state & k_speed ? getMoveData(m_ucmd->command_number).firstSpeed : true;
			
			// now determine if we can shoot during this tick
			if (canShoot)
				verifyShot();

			// update spread per first shot
			if (!isBursting())
				m_weapon.UpdateAccuracyPenalty();
			
			// autopistol
			if (isFiring() && !m_weapon.IsKnife() && !canFire())
				m_ucmd->buttons &= ~IN_ATTACK;

			m_state |= k_validWeapon;
		}
		else
		{
			// stop attacking
			if (m_ucmd->buttons & IN_ATTACK)
				m_ucmd->buttons &= ~IN_ATTACK;

			// reload if we haven't started already
			if (m_curtime >= m_player.GetNextAttack())
				m_ucmd->buttons |= IN_RELOAD;
		}
	}
	
	bool clientmove::getSendPacket()
	{
		if (m_ebp == nullptr)
			return false;

		return *(bool*)(m_ebp.to<byte*>() - 1);
	}

	void clientmove::setSendPacket (bool state)
	{
		if (m_ebp == nullptr)
			return;
			
		*(bool*)(m_ebp.to<byte*>() - 1) = state;
	}
	
	// this must be called directly in InPrediction hook
	bool clientmove::inPrediction()
	{
		volatile void *ebp = getEBP();

		vec3 *angles;

		CSLocalPlayer player;

		uintptr_t retn = core->inPredictionReturnAddress().as<uintptr_t>();

		// are we being called in C_BasePlayer::CalcPlayerView?
		if (offset_fwd<uintptr_t>(_AddressOfReturnAddress(), retn, 5) == -1)
			return false;

		if (!getLocalPlayer(&player))
			return false;

		if (!player.IsAlive())
			return false;
		
		// remove visual recoil (Vector &eyeAngles)
		**(vec3**)(**(size_t**)ebp + 12) -= player.GetPunchAngle();

		// remove view shake
		return true;
	}

	// restore player data before RunCommand is called
	void clientmove::preCommand (void *player, CUserCmd *ucmd, bool isClient)
	{
		CSLocalPlayer localPlayer(player);

		if (player == nullptr || ucmd == nullptr)
			return;
		
		localPlayer.SetAbsVelocity(localPlayer.GetVelocity());		
		getMoveData(ucmd->command_number - 1).apply(localPlayer);

		// did we start speedhacking on this command?
		// sync tickbase with the amount of commands that we're sending
		if (getMoveData(ucmd->command_number).firstSpeed)
		{
			int tickCount = settings->misc.speedfactor->m_value;
		
			if (!isClient)
				localPlayer.SetTickBase(localPlayer.GetTickBase() - tickCount);
			else
				m_tickBase -= tickCount;
		}
	}

	// store predicted player data for next tick
	void clientmove::postCommand (void *player, CUserCmd *ucmd)
	{
		if (player == nullptr || ucmd == nullptr)
			return;
		
		getMoveData(ucmd->command_number).cache(CSLocalPlayer(player));
	}

	bool clientmove::isFiring()
	{
		if (!isActive() || m_weapon == nullptr || m_ucmd == nullptr)
			return false;
		
		// reloading or switching weapon
		if (m_curtime < m_player.GetNextAttack())
			return false;
		
		if (m_ucmd->buttons & IN_ATTACK2)
			return m_weapon.IsKnife() ? true : false;

		return m_ucmd->buttons & IN_ATTACK || isBursting();
	}

	// check if we're able to shoot for this tick
	bool clientmove::verifyShot()
	{
		if (canFire())
			m_state &= ~k_canFire;

		if (isBursting())
			m_state &= ~k_isBursting;

		// verify ptrs
		if (m_player != nullptr && m_weapon != nullptr)
		{
			// check for burst firing
			if (m_weapon.IsBurstMode())
			{
				if (m_weapon.GetBurstShotsRemaining() > 0)
				{
					float nextBurst = m_weapon.GetNextBurstShot();

					if (nextBurst > 0.0f && m_curtime >= nextBurst)
						m_state |= k_isBursting;
				}
			}

			if (!isBursting())
			{
				// do the standard checks against fire rate and reload/switch time
				if (m_curtime >= m_player.GetNextAttack() && m_curtime >= m_weapon.GetNextPrimaryAttack())
					m_state |= k_canFire;
			}
			else
			{
				m_state |= k_canFire;
			}
		}

		return canFire();
	}

	void clientmove::doBhop()
	{
		// wait one tick before setting IN_JUMP while in air
		static bool waitOne = false;
		// track the last time we hit the ground
		static bool wasOnGround = false;		

		if (!isActive())
			return;
			
		if (m_player.GetMoveType() == MOVETYPE_LADDER)
			return;
		
		if (!(m_fFlags & FL_ONGROUND))
		{
			if (wasOnGround)
			{
				if (waitOne)
				{
					// attempt a jump while in midair
					// this should fool SMAC autotrigger detection
					m_ucmd->buttons |= IN_JUMP;
					waitOne = false;
					wasOnGround = false;
					return;
				}
				
				// for next tick...
				waitOne = true;
			}

			// do our bunnyhop
			if (m_ucmd->buttons & IN_JUMP)
				m_ucmd->buttons &= ~IN_JUMP;
		}
		else
		{
			wasOnGround = true;
		}
	}

	static bool isSpeedhackDisabled()
	{
		static ConVar *sv_maxusrcmdprocessticks = nullptr;
		static xstring<24> name(/*sv_maxusrcmdprocessticks*/ 0x18, 0x1C, 0x6F6B4172, 0x41595750, 0x56464B43, 0x585B4548, 0x495E5D5B, 0x59525940);

		if (sv_maxusrcmdprocessticks == nullptr)
		{
			sv_maxusrcmdprocessticks = cvar->FindVar(name.decrypt());

			// does it exist?
			if (sv_maxusrcmdprocessticks == nullptr)
				return false;
		}
		
		return sv_maxusrcmdprocessticks->m_nValue > 0;
	}

	void clientmove::doSpeed()
	{
		if (isSpeedhackDisabled())
			return;
		
		// we need the stack frame for this
		if (m_ebp == nullptr)
			return;
		
		if (m_speedCounter > 0)
		{
			// initial speedhack tick
			if (m_speedCounter == settings->misc.speedfactor->m_value)
				getMoveData(m_ucmd->command_number).firstSpeed = true;
			
			setSendPacket(false);

			// iterate
			--m_speedCounter;
			
			// call CL_Move again
			*(uintptr_t*)(m_ebp.to<byte*>() + 4) -= 5;
		}
		else
		{
			m_speedCounter = settings->misc.speedfactor->m_value;

			// now we can send the packet
			setSendPacket(true);
		}

		m_state |= k_speed;
	}

	void clientmove::getSpread (int seed, float &outX, float &outY)
	{
		int numShots, bestShot;

		float length, bestLength;

		float randomInaccuracy, randomInaccuracyRadians, inaccuracyX, inaccuracyY;
		float randomSpread, randomSpreadRadians, spreadX[10], spreadY[10];

		outX = 0.0f;
		outY = 0.0f;

		if (!isActive() || !isWeaponValid())
			return;

		if (!m_weapon.IsHitscan())
			return;
		
		numShots = m_weapon.GetWpnData().GetBulletsPerShot();

		// 256 seeds
		core->randomSeed((seed & 255) + 1);

		// movement/stance penalty
		randomInaccuracyRadians = core->randomFloat(0.0f, m_pi * 2.0);
		randomInaccuracy = core->randomFloat(0.0f, m_weapon.GetInaccuracy());

		inaccuracyX = mu::cos(randomInaccuracyRadians) * randomInaccuracy;
		inaccuracyY = mu::sin(randomInaccuracyRadians) * randomInaccuracy;

		// base weapon spread
		// shotguns have more than one bullet
		for (int i = 0; i < numShots; i++)
		{
			randomSpreadRadians = core->randomFloat(0.0f, m_pi * 2.0);
			randomSpread = core->randomFloat(0.0f, m_weapon.GetSpread());

			spreadX[i] = mu::cos(randomSpreadRadians) * randomSpread;
			spreadY[i] = mu::sin(randomSpreadRadians) * randomSpread;
		}

		if (numShots > 1)
		{
			// get the closest to center
			for (int i = 0, bestLength = 8192.0f; i < numShots; i++)
			{
				length = (spreadX[i] * spreadX[i]) + (spreadY[i] * spreadY[i]);

				if (length >= bestLength)
					continue;

				bestShot = i;
				bestLength = length;
			}
		}
		else
		{
			bestShot = 0;
		}

		outX = inaccuracyX + spreadX[bestShot];
		outY = inaccuracyY + spreadY[bestShot];
	}

	// credits: wav
	void clientmove::antiSpread()
	{
		float x, y;

		vec3 dir, f, r, u, angles;

		if (!isActive() || !isWeaponValid())
			return;

		// random spread
		getSpread(m_ucmd->random_seed, x, y);

		// nothing to compensate
		if (x == 0.0f && y == 0.0f)
			return;

		// origin angles
		vec3().angleVectors(&f, &r, &u);

		// spread direction to spread angles
		// note that this is still relative to origin
		dir = f + (r * x) + (u * y);
		dir.vectorNormalize();
		dir.vectorAngles(angles);

		// since we are transposing, we need to adjust according to our (negative) pitch
		// otherwise, we would use the positive value
		angles.x -= m_ucmd->viewangles.x;
		angles.angleNormalize().angleVectorsTranspose(&f, &r, &u);

		// now we use reference up to get the compensated angles
		f.vectorAngles(angles, &u);
		angles.y += m_ucmd->viewangles.y;		
		m_ucmd->viewangles = angles.angleNormalize();
	}

	void clientmove::antiRecoil()
	{
		m_ucmd->viewangles = (m_ucmd->viewangles - m_player.GetPunchAngle() * 2.0f).angleNormalize();
	}

	// run our aimbot
	void clientmove::doAim()
	{
		int fire;
		
		vec3 tmp, dir;
		
		if (!isActive() || !isWeaponValid())
		{
		AIM_STOP:
			return;
		}

		bool isKnife = m_weapon.IsKnife();
		
		if ((!isKnife && !canFire()) || (isKnife && m_curtime < m_player.GetNextAttack()))
			goto AIM_STOP;
		
		if (!settings->aim.triggerbot->m_value)
		{
			bool shouldSkip = false;

			if (findTarget() != -1)
			{
				bool smacOn = settings->misc.antismac->m_value == 1;

				// SMAC aimbot detection bypass
				if (smacOn && !isKnife)
				{
					if (m_lastTarget == -1)
					{
						// null our viewangles to invalidate our angle history
						// make this silent to spectators
						m_ucmd->viewangles.zero();
						setSendPacket(false);
						
						// save target for next tick
						m_lastTarget = m_target;
						
						// keep us moving straight
						fixMove(m_viewangles, m_ucmd->viewangles);
						
						// wait another tick
						goto AIM_STOP;
					}
					else
					{
						CSPlayer prev;
						
						// get the last player target
						if (entlist->GetPlayer(m_lastTarget, &prev))
						{
							// reset
							m_lastTarget = -1;
							shouldSkip = true;

							// we need to check their visibility again
							if (!isVisible(this, prev, tmp))
								goto AIM_STOP;
							
							// save their new position
							m_targetPos = tmp;
						}
						else
						{
							goto AIM_STOP;
						}
					}
				}

				// set attack flags
				fire = isKnife ? m_knifeResult : IN_ATTACK;
				m_ucmd->buttons |= fire;

				// angles to target
				dir = m_targetPos - m_vieworg;
				dir.vectorNormalize();
				dir.vectorAngles(m_ucmd->viewangles);
				
				m_ucmd->viewangles.angleNormalize();
		
				// set client angles or fix movement
				if (!settings->aim.silent->m_value)
					engine->SetViewAngles(m_ucmd->viewangles);
				else
					fixMove(m_viewangles, m_ucmd->viewangles);

				// save new angles
				m_viewangles = m_ucmd->viewangles;
			}

			// save last target (for anti-SMAC)
			if (!shouldSkip)
				m_lastTarget = m_target;
		}
		else
		{
			// get (suggested) attack flags
			fire = checkTrigger(this, m_ucmd->random_seed);

			if (fire)
				m_ucmd->buttons |= fire;
		}	
	}
	
	int clientmove::findTarget()
	{
		vec3 pos;

		CSPlayer player;

		float dist, sortDist;
		
		bool isKnife = m_weapon.IsKnife();

		m_target = -1;

		// cycle all clients on the server
		for (int i = 1, sortDist = 8192.0f; i <= gpGlobals->GetMaxClients(); ++i)
		{
			if (!entlist->GetPlayer(i, &player))
				continue;
			
			// do we want to shoot this nigga?
			if (!isValidTarget(player))
				continue;
			
			if (!isKnife)
			{
				dist = m_vieworg.dist(player.GetAbsOrigin());
				
				if (dist >= m_weapon.GetWpnData().GetRange())
					continue;

				if (settings->aim.priority->m_value == 2)
				{
					dist = m_viewangles.angleFOV(m_vieworg, player.GetAbsOrigin());

					if (dist > settings->aim.fov->m_value)
						continue;
				}
				
				if (dist >= sortDist)
					continue;

				// hitscan trace
				if (!isVisible(this, player, pos))
					continue;
				
				// cache dat shit
				sortDist = dist;
				m_target = i;
				m_targetPos = pos;
			}
			else
			{
				if (!player.GetHitboxPosition(HITBOX_SPINE2, pos))
					continue;
			
				// knife trace
				if (m_knifeResult = pathFreeMelee(this, &pos, nullptr, i))
				{
					m_target = i;
					m_targetPos = pos;
					break;
				}
			}
		}
		
		return m_target;
	}

	// this is old CS 1.6 code by Tabris
	void clientmove::fixMove (const vec3 &prevang, const vec3 &newang)
	{
		vec3 f1, r1, u1, f2, r2, u2, move;

		// current velocity
		move.init(m_ucmd->forwardmove, m_ucmd->sidemove, m_ucmd->upmove);

		// check if we're moving
		if (move.length() <= 0.0f)
			return;

		// get the normalized vecs
		prevang.angleVectors(&f1, &r1, &u1);
		newang.angleVectors(&f2, &r2, &u2);

		// credits: Tetsuo
		m_ucmd->forwardmove = f2.dot(f1 * move.x) + f2.dot(r1 * move.y) + f2.dot(u1 * move.z);
		m_ucmd->sidemove = r2.dot(f1 * move.x) + r2.dot(r1 * move.y) + r2.dot(u1 * move.z);
		m_ucmd->upmove = u2.dot(f1 * move.x) + u2.dot(r1 * move.y) + u2.dot(u1 * move.z);
	}

	void clientmove::antiAim (bool safe /*= false*/)
	{
		static bool flip = false;

		bool wall = false;

		vec3 va, angles, normal;

		if (!isActive() || m_ucmd == nullptr)
			return;

		// save old viewangles
		va = m_ucmd->viewangles.angleNormalize();

		// upside down
		m_ucmd->viewangles.x = safe ? 70.0f : 180.0f;
		
		// look for a wall to hide our head behind
		for (float i = 0.0f; i < 360.0f; i += 15.0f)
		{
			if (isFacingWall(m_ucmd->viewangles.y + i, &normal))
			{
				wall = true;
				break;
			}
		}

		if (wall)
		{
			// face the wall
			normal.negate().vectorAngles(angles);

			if (!flip)
				angles.y += 180.0f;
		}
		else
		{
			// screw up hitboxes via bSendPacket
			angles.y = va.y + (!flip ? -90.0f : 30.0f);
		}

		m_ucmd->viewangles.y = angles.angleNormalize().y;
		
		fixMove(va, m_ucmd->viewangles);
		
		if (flip)
			setSendPacket(false);

		flip = !flip;
	}

	bool clientmove::isFacingWall (float yaw, vec3 *normal /*= nullptr*/)
	{
		vec3 va, f;

		trace_t tr;	

		CTraceFilterNoPlayers filter;

		if (!isActive() || m_ucmd == nullptr)
			return false;

		// look slightly down
		va.init(10.0f, yaw, 0.0f);
		va.angleNormalize();
		va.angleVectors(&f, nullptr, nullptr);

		// trace forward, ignoring players
		UTIL_TraceLine(m_vieworg, m_vieworg + f * 32.0f, (MASK_SOLID & ~CONTENTS_GRATE), &filter, &tr);
		
		// no hit
		if (tr.fraction == 1.0f)
			return false;

		// orthogonal to the surface we hit
		if (normal != nullptr)
			*normal = tr.normal;
		
		return true;
	}

	void clientmove::fakeDuck()
	{
		static bool flip = false;

		if (isActive() && m_ucmd->buttons & IN_DUCK)
		{
			if (!flip)
			{
				// undo buttons
				m_ucmd->buttons &= ~IN_DUCK;

				// set player data
				m_player.SetDucked(false);

				// choke the packet
				if (!(m_state & k_speed))
					setSendPacket(false);
			}

			flip = !flip;
		}
	}

	void clientmove::fakeLag()
	{
		if (m_state & k_speed)
			return;

		if (m_fakeLagCounter <= 1)
		{
			if (!isFiring())
			{
				// reset the counter
				m_fakeLagCounter = settings->hvh.lagfactor->m_value;
				setSendPacket(true);
			}
		}
		else
		{
			// keep choking the packet
			--m_fakeLagCounter;
			setSendPacket(false);
		}
	}
};