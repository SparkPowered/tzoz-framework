#include "../../stdafx.h"
#include "settings.h"

namespace css
{
	static ctrlsettings _settings;
	ctrlsettings *settings = &_settings;

	void ctrlsettings::reset()
	{
		memset(&aim, 0, sizeof aim);
		memset(&esp, 0, sizeof esp);
		memset(&hvh, 0, sizeof hvh);
		memset(&misc, 0, sizeof misc);
		memset(&gui, 0, sizeof gui);
	}

	void ctrlsettings::init()
	{
		static xstring<8> _aimbot(/*aimbot*/ 0x06, 0xD1, 0xB0BBBEB6, 0xBAA20000);
		static xstring<12> _triggerbot(/*triggerbot*/ 0x0A, 0x5E, 0x2A2D0906, 0x05061607, 0x09130000);
		static xstring<8> _priority(/*priority*/ 0x08, 0x56, 0x26253136, 0x28322824);
		static xstring<4> _fov(/*fov*/ 0x03, 0xC4, 0xA2AAB000);
		static xstring<8> _silent(/*silent*/ 0x06, 0x29, 0x5A434749, 0x435A0000);
		static xstring<4> _body(/*body*/ 0x04, 0xB8, 0xDAD6DEC2);
		static xstring<8> _autowall(/*autowall*/ 0x08, 0x33, 0x52414159, 0x40595556);
		static xstring<12> _deathmatch(/*deathmatch*/ 0x0A, 0x6C, 0x08080F1B, 0x181C1307, 0x171D0000);
		static xstring<8> _friends(/*friends*/ 0x07, 0x1A, 0x7C697578, 0x707B5300);
		static xstring<16> _friendlyfire(/*friendly fire*/ 0x0D, 0x49, 0x2F382229, 0x232A2329, 0x71343A26, 0x30000000);
		static xstring<12> _antispread(/*anti spread*/ 0x0B, 0x80, 0xE1EFF6EA, 0xA4F6F6F5, 0xEDE8EE00);
		static xstring<12> _antirecoil(/*anti recoil*/ 0x0B, 0xAC, 0xCDC3DAC6, 0x90C3D7D0, 0xDBDCDA00);

		static xstring<4> _esp(/*esp*/ 0x03, 0x85, 0xE0F5F700);
		static xstring<4> _name(/*name*/ 0x04, 0xB0, 0xDED0DFD6);
		static xstring<8> _health(/*health*/ 0x06, 0x3E, 0x565A212D, 0x362B0000);
		static xstring<8> _weapon(/*weapon*/ 0x06, 0xDA, 0xADBEBDAD, 0xB1B10000);
		static xstring<8> _skeleton(/*skeleton*/ 0x08, 0x9B, 0xE8F7F8F2, 0xFAD4CECC);
		static xstring<12> _enemyonly(/*enemy only*/ 0x0A, 0x95, 0xF0F8F2F5, 0xE0BAF4F2, 0xF1E70000);
		static xstring<8> _world(/*world*/ 0x05, 0x42, 0x352C3629, 0x22000000);
		static xstring<8> _chams(/*chams*/ 0x05, 0xD8, 0xBBB1BBB6, 0xAF000000);

		static xstring<8> _antiaim(/*anti aim*/ 0x08, 0xCE, 0xAFA1A4B8, 0xF2B2BDB8);
		static xstring<8> _fakeduck(/*fakeduck*/ 0x08, 0xF4, 0x92949D92, 0x9C8C9990);
		static xstring<8> _fakelag(/*fakelag*/ 0x07, 0xFF, 0x99616A67, 0x6F656200);
		static xstring<12> _lagfactor(/*lag factor*/ 0x0A, 0x4B, 0x272D2A6E, 0x29313226, 0x3C260000);

		static xstring<12> _antismac(/*anti smac*/ 0x09, 0xF3, 0x929A819F, 0xD78B949B, 0x98000000);
		static xstring<12> _speedhack(/*speedhack*/ 0x09, 0x52, 0x21233130, 0x323F393A, 0x31000000);
		static xstring<12> _speedfactor(/*speed factor*/ 0x0C, 0xAA, 0xD9DBC9C8, 0xCA8FD6D0, 0xD1C7DBC7);
		static xstring<12> _namestealer(/*namestealer*/ 0x0B, 0x89, 0xE7EBE6E9, 0xFEFAEAF1, 0xFDF7E100);

		static xstring<4> _r(/*r*/ 0x01, 0xBB, 0xC9000000);
		static xstring<4> _g(/*g*/ 0x01, 0xD7, 0xB0000000);
		static xstring<4> _b(/*b*/ 0x01, 0xCC, 0xAE000000);

		aim.enable = new ctrlvar(_aimbot.decrypt());
		aim.triggerbot = new ctrlvar(_triggerbot.decrypt());
		aim.priority = new ctrlvar(_priority.decrypt(), 1, 1, 2);
		aim.fov = new ctrlvar(_fov.decrypt(), 360, 5, 360, 5);
		aim.silent = new ctrlvar(_silent.decrypt(), 1);
		aim.body = new ctrlvar(_body.decrypt());
		aim.autowall = new ctrlvar(_autowall.decrypt());
		aim.deathmatch = new ctrlvar(_deathmatch.decrypt());
		aim.friends = new ctrlvar(_friends.decrypt());
		aim.team = new ctrlvar(_friendlyfire.decrypt());
		aim.antispread = new ctrlvar(_antispread.decrypt());
		aim.antirecoil = new ctrlvar(_antirecoil.decrypt());
		
		esp.enable = new ctrlvar(_esp.decrypt(), 1);
		esp.name = new ctrlvar(_name.decrypt(), 1);
		esp.health = new ctrlvar(_health.decrypt(), 1);
		esp.weapon = new ctrlvar(_weapon.decrypt());
		esp.skeleton = new ctrlvar(_skeleton.decrypt());
		esp.team = new ctrlvar(_enemyonly.decrypt(), 1);
		esp.world = new ctrlvar(_world.decrypt());
		esp.chams = new ctrlvar(_chams.decrypt());

		hvh.antiaim = new ctrlvar(_antiaim.decrypt());
		hvh.fakeduck = new ctrlvar(_fakeduck.decrypt());
		hvh.fakelag = new ctrlvar(_fakelag.decrypt());
		hvh.lagfactor = new ctrlvar(_lagfactor.decrypt(), 8, 2, 16);
		
		misc.antismac = new ctrlvar(_antismac.decrypt());
		misc.speedhack = new ctrlvar(_speedhack.decrypt());
		misc.speedfactor = new ctrlvar(_speedfactor.decrypt(), 8, 2, 16);
		misc.namesteal = new ctrlvar(_namestealer.decrypt());

		gui.r = new ctrlvar(_r.decrypt(), 15, 0, 255, 5);
		gui.g = new ctrlvar(_g.decrypt(), 20, 0, 255, 5);
		gui.b = new ctrlvar(_b.decrypt(), 25, 0, 255, 5);
	}
};