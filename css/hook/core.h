#pragma once

#include "sourcemodule.h"

#include "../../util/vec3.h"
#include "../../util/vector.h"

#include "../../css/sdk/cvar.h"
#include "../../css/sdk/debugoverlay.h"
#include "../../css/sdk/globalvarsbase.h"
#include "../../css/sdk/client_dll.h"
#include "../../css/sdk/engineclient.h"
#include "../../css/sdk/enginetrace.h"
#include "../../css/sdk/input.h"
#include "../../css/sdk/prediction.h"
#include "../../css/sdk/cliententlist.h"
#include "../../css/sdk/gameresources.h"
#include "../../css/sdk/inputsystem.h"
#include "../../css/sdk/vguipanel.h"
#include "../../css/sdk/vguisurface.h"
#include "../../css/sdk/modelinfo.h"
#include "../../css/sdk/modelrender.h"
#include "../../css/sdk/steamclient.h"
#include "../../css/sdk/utlmemory.h"

#include "../../css/sdk/csplayer.h"
#include "../../css/sdk/csweapon.h"

using namespace mu;

namespace css
{
	enum
	{
		k_clientDll = 0xb38f9d96,
		k_engineDll = 0xf57ae481,
		k_serverDll = 0xfba3454a,
		k_vguiMatSurfaceDll = 0xc9950f4f,
		k_inputSystemDll = 0x2459261c,
		k_vgui2Dll = 0xc3d98690,
		k_vstdlibDll = 0xa806f59d,
		k_vphysicsDll = 0xdd04fa12,
		k_materialSystemDll = 0x79865b61,
		k_steamClientDll = 0x9852a178,
	};

	class gamecore {
	public:
		gamecore()
		{
			reset();
		}

		void reset();
		bool init();
		void hook();
		bool isfriend (int entnum);
		void fillFriendIdList();
		int sharedRandomInt (int predictionSeed, string sharedName, int lo, int hi, int additionalSeed = 0);
		float sharedRandomFloat (int predictionSeed, string sharedName, float lo, float hi, int additionalSeed = 0);

		void randomSeed (int seed)
		{
			m_randomSeed.as<void (*)(int)>()(seed);
		}

		int randomInt (int lo, int hi)
		{
			return m_randomInt.as<int (*)(int, int)>()(lo, hi);
		}

		float randomFloat (float lo, float hi)
		{
			return m_randomFloat.as<float (*)(float, float)>()(lo, hi);
		}

		IGameResources *getGameResources()
		{
			return m_getGameResources.as<IGameResources *(*)(void)>()();
		}

		void setPredictionRandomSeed (int seed)
		{
			*m_predictionRandomSeed.as<int*>() = seed;
		}

		void *getWpnData (void *wpn)
		{
			return m_getWpnData.as<void *(__thiscall*)(void*)>()(wpn);
		}

		string weaponIDToAlias (int weaponID)
		{
			return m_weaponIDToAlias.as<char *(*)(int)>()(weaponID);
		}

		void getBulletTypeParameters (void *player, int bullet, float &penetrationPower, float &penetrationDist)
		{
			return m_getBulletTypeParameters.as<void (__thiscall*)(void*, int, float&, float&)>()(player, bullet, penetrationPower, penetrationDist);
		}

		address inPredictionReturnAddress()
		{
			return m_inPredictionReturnAddress.get();
		}

		void setLastGameNameUsed (bool state)
		{
			*m_lastGameNameUsed.as<bool*>() = state;
		}

		address cvarCallbackReturnAddress()
		{
			return m_cvarCallbackReturnAddress.get();
		}
		
		void estimateAbsVelocity (void *entity, vec3 &vel)
		{
			m_estimateAbsVelocity.as<void (__thiscall*)(void*, vec3&)>()(entity, vel);
		}

		// modules
		sourcemodule clientdll;
		sourcemodule enginedll;
		sourcemodule serverdll;
		sourcemodule vguimatsurfacedll;
		sourcemodule inputsystemdll;
		sourcemodule vgui2dll;
		sourcemodule vstdlibdll;
		sourcemodule vphysicsdll;
		sourcemodule materialsystemdll;
		sourcemodule steamclientdll;
		
	private:
		// do findpattern scans for pointers
		bool appscan();
		
		// set up friends list
		bool initsteam();

		// imports
		xaddress m_randomSeed;
		xaddress m_randomInt;
		xaddress m_randomFloat;

		// pointers
		xaddress m_getGameResources;
		xaddress m_predictionRandomSeed;
		xaddress m_getWpnData;
		xaddress m_weaponIDToAlias;
		xaddress m_getBulletTypeParameters;
		xaddress m_inPredictionReturnAddress;
		xaddress m_lastGameNameUsed;
		xaddress m_cvarCallbackReturnAddress;
		xaddress m_estimateAbsVelocity;

		// save the friend list count
		int m_prevFriendCount;

		// steam friend ids
		vector<CSteamID> m_friends;
	};

	extern gamecore *core;

	extern CCvar *cvar;
	extern CDebugOverlay *debugoverlay;
	extern CGlobalVarsBase *gpGlobals;
	extern CHLClient *g_ClientDLL;
	extern CEngineClient *engine;
	extern CEngineTrace *enginetrace;
	extern CPhysicsSurfaceProps *physprops;
	extern CSpatialPartition *spatialpartition;
	extern CStaticPropMgr *staticpropmgr;
	extern CInput *input;
	extern CMoveHelper *movehelper;
	extern CGameMovement *gamemovement;
	extern CPrediction *prediction;
	extern CClientEntityList *entlist;
	extern CInputSystem *inputsystem;
	extern Panel *vguipanel;
	extern Surface *vguisurface;
	extern CModelInfo *modelinfoclient;
	extern CRenderView *renderview;
	extern CModelRender *modelrender;
	extern CSteamFriends *steamfriends;
	extern CSteamClient *steamclient;
	extern CGameRules *g_pGameRules;
};