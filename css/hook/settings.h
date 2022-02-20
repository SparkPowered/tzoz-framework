#pragma once

#include "config.h"

namespace css
{
	class ctrlsettings {
	public:
		ctrlsettings()
		{
			reset();
		}
		
		void reset();
		void init();

		struct
		{
			ctrlvar *enable;
			ctrlvar *triggerbot;
			ctrlvar *priority;
			ctrlvar *fov;
			ctrlvar *silent;
			ctrlvar *body;
			ctrlvar *autowall;
			ctrlvar *deathmatch;
			ctrlvar *friends;
			ctrlvar *team;
			ctrlvar *antispread;
			ctrlvar *antirecoil;
		} aim;

		struct
		{
			ctrlvar *enable;
			ctrlvar *name;
			ctrlvar *health;
			ctrlvar *weapon;
			ctrlvar *skeleton;
			ctrlvar *team;
			ctrlvar *world;
			ctrlvar *chams;
		} esp;

		struct
		{
			ctrlvar *antiaim;
			ctrlvar *fakeduck;
			ctrlvar *fakelag;
			ctrlvar *lagfactor;
		} hvh;

		struct
		{
			ctrlvar *speedhack;
			ctrlvar *speedfactor;
			ctrlvar *antismac;
			ctrlvar *namesteal;
		} misc;

		struct
		{
			ctrlvar *r;
			ctrlvar *g;
			ctrlvar *b;
		} gui;
	};

	extern ctrlsettings *settings;
};