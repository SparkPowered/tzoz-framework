#include "../../stdafx.h"
#include "drawpanel.h"
#include "client.h"
#include "gui.h"
#include "settings.h"

#include "../../win/imports.h"
#include "../../hash/hash.h"

namespace css
{
	static drawpanel _draw;
	drawpanel *draw = &_draw;

	bool drawpanel::init()
	{
		static xstring<16> fname(/*Lucida Console*/ 0x0E, 0x93, 0xDFE1F6FF, 0xF3F9B9D9, 0xF4F2EEF1, 0xF3C50000);
		
		if (!m_fontName.init(fname.decrypt(), 13, FONTFLAG_OUTLINE))
			return false;

		if (!m_fontData.init(fname.decrypt(), 10, FONTFLAG_OUTLINE))
			return false;

		if (!m_fontUi.init(fname.decrypt(), 12, FONTFLAG_OUTLINE))
			return false;

		return true;
	}

	void drawpanel::frame (dword handle)
	{
		core->setLastGameNameUsed(true);
		core->fillFriendIdList();		
		
		if (hash_t(vguipanel->GetName(handle)) == 0xd219bd05/*FocusOverlayPanel*/)
		{
			paint();
		}
	}

	void drawpanel::paint()
	{
		if (engine->IsInGame() && settings->esp.enable->m_value)
		{
			if (settings->esp.world->m_value);
				worldESP();
			
			playerESP();
		}

		gui->m_menu.baseDraw(&m_fontUi, 30, 12);
	}

	void drawpanel::playerESP()
	{
		static xstring<4> hstr(/*%dhp*/ 0x04, 0xD5, 0xF0B2BFA8);

		color32 col, hcol;
		
		CSPlayer player;
		CSLocalPlayer local;
		CSWeapon weapon;
		player_info_t pi;

		studiohdr_t *hdr;
		mstudiobone_t *bone;
		
		vec3 origin, head, pbone, cbone, s_origin, s_head, s_pbone, s_cbone;

		if (!getLocalPlayer(&local))
			return;

		for (int i = 1; i <= gpGlobals->GetMaxClients(); ++i)
		{
			if (i == engine->GetLocalPlayer())
				continue;

			if (!entlist->GetPlayer(i, &player))
				continue;

			if (!engine->GetPlayerInfo(i, &pi))
				continue;

			if (player.IsDormant() || !player.IsAlive())
				continue;

			if (settings->esp.team->m_value)
			{
				if (player.GetTeamNum() == local.GetTeamNum())
					continue;
			}

			if (!player.SetupBones(CBaseEntity::s_boneCache))
				continue;

			origin = player.GetAbsOrigin();

			if (!player.GetHitboxPosition(CBaseEntity::s_boneCache, HITBOX_HEAD, head))
				continue;

			if (debugoverlay->ScreenPosition(origin, s_origin) != 1 &&
				debugoverlay->ScreenPosition(head, s_head) != 1)
			{
				// team color
				if (!core->isfriend(i))
				{
					if (player.GetTeamNum() == 2)
						col.set(255, 128, 128, 255);
					else
						col.set(156, 192, 255, 255);
				}
				else
				{
					col.set(128, 255, 128, 255);
				}

				s_origin.y += 4.0f;

				// name esp
				if (settings->esp.name->m_value)
				{
					m_fontName.drawstr(s_origin.x, s_origin.y, true, col, pi.name);
					s_origin.y += vguisurface->GetFontTall(m_fontName);
				}

				if (settings->esp.health->m_value)
				{
					// get health color
					int health = player.GetHealth();
					
					if (health <= 100)
						hcol.set(255,
							(byte)((float)health * 2.55f),
							(byte)((float)health * 2.55f),
							255);
					else
						hcol.set(-1);

					// health esp
					m_fontData.drawstrfmt(s_origin.x, s_origin.y, true, hcol, hstr.decrypt(), health);
					s_origin.y += vguisurface->GetFontTall(m_fontData);
				}

				// weapon esp
				if (settings->esp.weapon->m_value && player.GetActiveWeapon(&weapon))
				{
					m_fontData.drawstr(s_origin.x, s_origin.y, true, color32(-1), core->weaponIDToAlias(weapon.GetWeaponID()));
				}

				// do skeleton
				if (settings->esp.skeleton->m_value)
				{
					hdr = modelinfoclient->GetStudioModel(player.GetModel());

					if (hdr != nullptr)
					{
						for (int j = 0; j < hdr->numbones; j++)
						{
							bone = hdr->pBone(j);

							if (bone == nullptr)
								continue;

							if (!(bone->flags & 256)) // BONE_USED_BY_HITBOX
								continue;

							if (bone->parent == -1)
								continue;

							player.GetBonePosition(CBaseEntity::s_boneCache, j, pbone);
							player.GetBonePosition(CBaseEntity::s_boneCache, bone->parent, cbone);

							if (debugoverlay->ScreenPosition(pbone, s_pbone) != 1 &&
								debugoverlay->ScreenPosition(cbone, s_cbone) != 1)
							{
								vguisurface->DrawLine(s_pbone.x, s_pbone.y, s_cbone.x, s_cbone.y, color32(-1));
							}
						}
					}
				}

				// head hitbox center
				vguisurface->DrawOutlinedRect(s_head.x - 3, s_head.y - 3, 6, 6, color32(255));
				vguisurface->DrawOutlinedRect(s_head.x - 2, s_head.y - 2, 4, 4, col);
				vguisurface->DrawOutlinedRect(s_head.x - 1, s_head.y - 1, 2, 2, color32(255));
			}
		}
	}

	void drawpanel::worldESP()
	{
		static xstring<16> fb(/*_eq_flashbang*/ 0x0D, 0x75, 0x2A130627, 0x1F161A0F, 0x151C1EEE, 0xE6000000);
		static xstring<16> frag(/*_eq_fraggrenade*/ 0x0F, 0xBD, 0xE2DBCE9F, 0xA7B0A2A3, 0xA2B4A2A6, 0xA8AEAE00);
		static xstring<16> smoke(/*_eq_smokegrenade*/ 0x10, 0xF3, 0xAC9184A9, 0x84959691, 0x9E9B8F9B, 0x91616567);
		static xstring<16> c4(/*c4_planted*/ 0x0A, 0xF0, 0x93C5AD83, 0x98949883, 0x9D9D0000);

		dword hash;

		string tmp, modelname;
		staticstring<256> buf;

		vec3 center, s_center;

		CSWeapon entity;		

		for (int i = 1; i <= entlist->GetHighestEntityIndex(); i++)
		{
			if (i == engine->GetLocalPlayer())
				continue;

			if (!entlist->GetClientEntity(i, &entity))
				continue;

			if (entity.IsDormant())
				continue;

			center = entity.WorldSpaceCenter();

			if (debugoverlay->ScreenPosition(center, s_center) == 1)
				continue;

			modelname = modelinfoclient->GetModelName(entity.GetModel());
			hash = entity.GetClassHash();

			// thrown grenade
			if (hash == 0x8ce3dbfd/*CBaseCSGrenade*/ || hash == 0x6bb2e048/*CBaseCSGrenadeProjectile*/)
			{
				// verify which type of grenade this is
				tmp = modelname.find(fb.decrypt());
				if (tmp.str() == nullptr) {
					tmp = modelname.find(frag.decrypt());
					if (tmp.str() == nullptr) {
						tmp = modelname.find(smoke.decrypt());
						if (tmp.str() == nullptr)
							continue;
					}
				}

				// strip modelname
				buf.copy(tmp + 4);
				buf.str()[buf.length() - 11] = '\0';
			}
			else
			{
				// planted c4? or just a weapon
				tmp = modelname.find(c4.decrypt());

				if (tmp.str() == nullptr)
				{
					if (entity.IsWeapon())
					{
						// must be a dropped weapon
						if (entity.GetState() == 0)
							buf.copy(entity.GetWeaponAlias());
						else
							continue;
					}
					else
					{
						continue;
					}
				} 
				else
				{
					// strip modelname
					buf.copy(tmp);
					buf.str()[2] = '\0';
				}
			}

			m_fontData.drawstr(s_center.x, s_center.y, true, color32(-1), buf.str());
		}
	}

	void modelparams::init (void *ecx, void *state, const ModelRenderInfo_t &info, matrix3x4_t *matrix)
	{
		m_ecx = ecx;
		m_state = state;
		m_info = info;
		m_matrix = matrix;
	}
	
	bool modelparams::doChams()
	{		
		CBaseEntity entity;		

		CMaterial mat;
		staticlist<void*, 32> mats;

		fcolor32 newclr, oldclr;

		// not all models have 32 (maximum) textures
		mats.zero();

		if (!entlist->GetClientEntity(m_info.entity_index, &entity))
			return false;
			
		if (!entity.IsPlayer())
			return false;
		
		// we need this for numtextures
		studiohdr_t *hdr = modelinfoclient->GetStudioModel(m_info.model);
		
		if (hdr == nullptr)
			return false;
			
		if (hdr->numtextures <= 0)
			return false;

		int team = entity.GetTeamNum();
		
		// must be a valid player team
		if (team != 2 && team != 3)
			return false;
		
		// get all materials for this model
		modelinfoclient->GetModelMaterials(m_info.model, hdr->numtextures, mats.begin());
		
		for (int i = 0; i < hdr->numtextures; i++)
		{
			if (mats[i] == nullptr)
				continue;
			
			renderview->GetColorModulation(oldclr);
			
			if (team == 2)
				newclr.set(255.0f, 0.2784313f, 0.2784313f); // terrorist
			else
				newclr.set(0.611764f, 0.752941f, 255.0f); // counter-terrorist

			// set team color
			renderview->SetColorModulation(newclr);

			mat.set(mats[i]);						
			
			mat.SetMaterialVarFlag(MATERIAL_VAR_ALPHATEST, true);
			mat.SetMaterialVarFlag(MATERIAL_VAR_FLAT, true);
			mat.SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);	
			mat.SetMaterialVarFlag(MATERIAL_VAR_NOFOG, true);
			mat.SetMaterialVarFlag(MATERIAL_VAR_NOCULL, true);
			mat.SetMaterialVarFlag(MATERIAL_VAR_ZNEARER, true);

			// draw the hacked model
			modelrender->DrawModelExecute(m_ecx, m_state, m_info, m_matrix);

			// set original color
			renderview->SetColorModulation(oldclr);

			mat.SetMaterialVarFlag(MATERIAL_VAR_ALPHATEST, false);
			mat.SetMaterialVarFlag(MATERIAL_VAR_FLAT, false);
			mat.SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);	
			mat.SetMaterialVarFlag(MATERIAL_VAR_NOFOG, false);
			mat.SetMaterialVarFlag(MATERIAL_VAR_NOCULL, false);
			mat.SetMaterialVarFlag(MATERIAL_VAR_ZNEARER, false);

			// draw the original model
			modelrender->DrawModelExecute(m_ecx, m_state, m_info, m_matrix);
		}
		
		return true;
	}
};