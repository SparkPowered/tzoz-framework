#include "../../stdafx.h"
#include "core.h"
#include "hooks.h"

#include "../../hash/hash.h"
#include "../../hash/crc32.h"

namespace css
{
	static gamecore _core;
	gamecore *core = &_core;

	static CCvar _cvar;
	static CDebugOverlay _debugoverlay;
	static CGlobalVarsBase _gpGlobals;
	static CHLClient _g_ClientDLL;
	static CEngineClient _engine;
	static CEngineTrace _enginetrace;
	static CPhysicsSurfaceProps _physprops;
	static CSpatialPartition _spatialpartition;
	static CStaticPropMgr _staticpropmgr;
	static CInput _input;
	static CMoveHelper _movehelper;
	static CGameMovement _gamemovement;
	static CPrediction _prediction;
	static CClientEntityList _entlist;
	static CInputSystem _inputsystem;
	static Panel _vguipanel;
	static Surface _vguisurface;
	static CModelInfo _modelinfoclient;
	static CRenderView _renderview;
	static CModelRender _modelrender;
	static CSteamFriends _steamfriends;
	static CSteamClient _steamclient;
	static CGameRules _g_pGameRules;

	CCvar *cvar = &_cvar;
	CDebugOverlay *debugoverlay = &_debugoverlay;
	CGlobalVarsBase *gpGlobals = &_gpGlobals;
	CHLClient *g_ClientDLL = &_g_ClientDLL;
	CEngineClient *engine = &_engine;
	CEngineTrace *enginetrace = &_enginetrace;
	CPhysicsSurfaceProps *physprops = &_physprops;
	CSpatialPartition *spatialpartition = &_spatialpartition;
	CStaticPropMgr *staticpropmgr = &_staticpropmgr;
	CInput *input = &_input;
	CMoveHelper *movehelper = &_movehelper;
	CGameMovement *gamemovement = &_gamemovement;
	CPrediction *prediction = &_prediction;
	CClientEntityList *entlist = &_entlist;
	CInputSystem *inputsystem = &_inputsystem;
	Panel *vguipanel = &_vguipanel;
	Surface *vguisurface = &_vguisurface;
	CModelInfo *modelinfoclient = &_modelinfoclient;
	CRenderView *renderview = &_renderview;
	CModelRender *modelrender = &_modelrender;
	CSteamFriends *steamfriends = &_steamfriends;
	CSteamClient *steamclient = &_steamclient;
	CGameRules *g_pGameRules = &_g_pGameRules;

	void gamecore::reset()
	{
		clientdll.reset();
		enginedll.reset();
		serverdll.reset();
		vguimatsurfacedll.reset();
		inputsystemdll.reset();
		vgui2dll.reset();
		vphysicsdll.reset();
		materialsystemdll.reset();
		steamclientdll.reset();

		m_randomSeed.reset();
		m_randomInt.reset();
		m_randomFloat.reset();

		m_getGameResources.reset();
		m_predictionRandomSeed.reset();
		m_getWpnData.reset();
		m_weaponIDToAlias.reset();
		m_getBulletTypeParameters.reset();
		m_inPredictionReturnAddress.reset();
		m_lastGameNameUsed.reset();
		m_cvarCallbackReturnAddress.reset();
		m_estimateAbsVelocity.reset();

		m_prevFriendCount = 0;

		m_friends.reset();
	}

	void gamecore::hook()
	{
		if (g_ClientDLL->hooktable(true))
		{
			g_ClientDLL->hookmethod(&newCreateMove, 21, true);
			LOG("hooked client\n");
		}

		if (vguipanel->hooktable(true))
		{
			vguipanel->hookmethod(&newPaintTraverse, 41, true);
			LOG("hooked panel\n");
		}

		if (prediction->hooktable(true))
		{
			prediction->hookmethod(&newInPrediction, 14, true);
			prediction->hookmethod(&newRunCommand, 17, true);
			LOG("hooked prediction\n");
		}

		if (modelrender->hooktable(true))
		{
			modelrender->hookmethod(&newDrawModelExecute, 19, true);
			LOG("hooked modelrender\n");
		}
	}

	bool gamecore::isfriend (int entnum)
	{
		player_info_t pi;

		if (engine->GetPlayerInfo(entnum, &pi))
		{
			for (CSteamID *id = m_friends.begin(); id != m_friends.end(); id++)
			{
				if (id->m_unAccountID == pi.friendsID)
					return true;
			}
		}

		return false;
	}

	// RandomInt using shared string for seed
	int gamecore::sharedRandomInt (int predictionSeed, string sharedName, int lo, int hi, int additionalSeed /*= 0*/)
	{
		randomSeed(crc32().seedFileLineHash(predictionSeed, sharedName, additionalSeed));
		return randomInt(lo, hi);
	}

	// RandomFloat using shared string for seed
	float gamecore::sharedRandomFloat (int predictionSeed, string sharedName, float lo, float hi, int additionalSeed /*= 0*/)
	{
		randomSeed(crc32().seedFileLineHash(predictionSeed, sharedName, additionalSeed));
		return randomFloat(lo, hi);
	}

	// make this update whenever we add/remove someone
	void gamecore::fillFriendIdList()
	{
		CSteamID id;

		int currentCount = steamfriends->GetFriendCount(4);

		// check if we called before
		if (m_prevFriendCount != 0)
		{
			// no changes
			if (m_prevFriendCount == currentCount)
				return;

			// destroy the list so we can fill a new one
			m_friends.free();
		}

		m_prevFriendCount = currentCount;

		// fill the list
		for (int i = 0; i < currentCount; i++)
		{
			steamfriends->GetFriendByIndex(id, i, 4);
			m_friends.append(id);
		}
	}

	bool gamecore::initsteam()
	{
		static xstring<16> friendsversion(/*SteamFriends009*/ 0x0F, 0xA5, 0xF6D2C2C9, 0xC4ECD9C5, 0xC8C0CBC3, 0x81828A00);

		int pipe = steamclient->CreateSteamPipe();
		int user = steamclient->ConnectToGlobalUser(pipe);

		void *pfriends = steamclient->GetISteamFriends(user, pipe, friendsversion.decrypt());

		if (steamfriends->fill(pfriends)) {
			LOG("SteamFriends009 = 0x%x\n", pfriends);
		} else return false;

		fillFriendIdList();

		return true;
	}

#define INIT_MODULE(m, h) if (!m.init(h)) return false
#define CREATE_INTERFACE(m, h, i, c) if (m.createInterface(h, i, c) == nullptr) return false

	bool gamecore::init()
	{
		INIT_MODULE(clientdll, k_clientDll);
		INIT_MODULE(enginedll, k_engineDll);
		INIT_MODULE(serverdll, k_serverDll);
		INIT_MODULE(vguimatsurfacedll, k_vguiMatSurfaceDll);
		INIT_MODULE(inputsystemdll, k_inputSystemDll);
		INIT_MODULE(vgui2dll, k_vgui2Dll);
		INIT_MODULE(vstdlibdll, k_vstdlibDll);
		INIT_MODULE(vphysicsdll, k_vphysicsDll);
		INIT_MODULE(materialsystemdll, k_materialSystemDll);
		INIT_MODULE(steamclientdll, k_steamClientDll);

		m_randomSeed.set(vstdlibdll.proc(0xd37d9329));
		m_randomInt.set(vstdlibdll.proc(0x6b41f8cd));
		m_randomFloat.set(vstdlibdll.proc(0x44199aae));

		CREATE_INTERFACE(clientdll, 0xe31ec98a/*VClient*/, g_ClientDLL, true);
		CREATE_INTERFACE(clientdll, 0x543b825f/*VClientPrediction*/, prediction, true);
		CREATE_INTERFACE(clientdll, 0x78aca813/*VClientEntityList*/, entlist, false);
		CREATE_INTERFACE(clientdll, 0xbba31408/*GameMovement*/, gamemovement, false);
		CREATE_INTERFACE(enginedll, 0x13622ae4/*VEngineClient*/, engine, false);
		CREATE_INTERFACE(enginedll, 0xf0e5e7d3/*EngineTraceClient*/, enginetrace, false);
		CREATE_INTERFACE(enginedll, 0xc8f015b5/*SpatialPartition*/, spatialpartition, false);
		CREATE_INTERFACE(enginedll, 0x333d1fab/*VModelInfoClient*/, modelinfoclient, false);
		CREATE_INTERFACE(enginedll, 0x11562581/*StaticPropMgrClient*/, staticpropmgr, false);
		CREATE_INTERFACE(enginedll, 0xaacc9a78/*VDebugOverlay*/, debugoverlay, false);
		CREATE_INTERFACE(enginedll, 0x53405d5a/*VEngineRenderView*/, renderview, false);
		CREATE_INTERFACE(enginedll, 0xa246ef32/*VEngineModel*/, modelrender, true);
		CREATE_INTERFACE(vguimatsurfacedll, 0x2e4b2042/*VGUI_Surface*/, vguisurface, false);
		CREATE_INTERFACE(inputsystemdll, 0x2164e2ac/*InputSystemVersion*/, inputsystem, false);
		CREATE_INTERFACE(vgui2dll, 0xdc8b6201/*VGUI_Panel*/, vguipanel, true);
		CREATE_INTERFACE(vstdlibdll, 0xb760b53b/*VEngineCvar*/, cvar, false);
		CREATE_INTERFACE(vphysicsdll, 0xcd75efe3/*VPhysicsSurfaceProps*/, physprops, false);
		CREATE_INTERFACE(steamclientdll, 0xd78a50f2/*SteamClient*/, steamclient, false);

		if (!initsteam())
		{
			LOG("gamecore::initsteam() failed\n");
			return false;
		}
		
		if (!appscan())
		{
			LOG("gamecore::appscan() failed\n");
			return false;
		}
		
		return true;
	}

#undef INIT_MODULE
#undef CREATE_INTERFACE

#ifndef LOG_ENABLE
	#define SCAN_LOG(m, n, v)
#else
	#define SCAN_LOG(m, n, v) LOG("%s -> %s = 0x%x\n", m.m_name.str(), n, v.ptr())
#endif

	bool gamecore::appscan()
	{
		address scanptr;

		scanptr = clientdll.hashScan(SECTION_TEXT, 0x3FA787A9, 0xFC3);
		if (scanptr != nullptr) {
			scanptr = **scanptr.get<size_t>(2).as<void***>();
			if (scanptr != nullptr) {
				input->set(scanptr);
				SCAN_LOG(clientdll, "CInput", scanptr);
			} else return false;
		} else return false;

		// credits: Badster
		scanptr = clientdll.hashScanFromReference(0x76E26555, 0x3FF, 0x68);
		if (scanptr != nullptr) {
			scanptr = findpattern(scanptr, 0x20, 0xCCC30459, 0x3);
			if (scanptr != nullptr) {
				scanptr = scanptr.get<size_t>(2).to<void*>();
				if (scanptr != nullptr) {
					g_pGameRules->set(scanptr);
					SCAN_LOG(clientdll, "CGameRules", scanptr);
				} else return false;
			} else return false;
		} else return false;
		
		scanptr = clientdll.hashScan(SECTION_TEXT, 0xA00BCF98, 0x3FC3);
		if (scanptr != nullptr) {
			scanptr = **scanptr.get<size_t>(2).as<void***>();
			if (scanptr != nullptr) {
				movehelper->fill(scanptr);
				SCAN_LOG(clientdll, "IMoveHelper", scanptr);
			} else return false;
		} else return false;

		scanptr = clientdll.hashScan(SECTION_TEXT, 0x417231f6, 0x3e1);
		if (scanptr != nullptr) {
			m_getGameResources.set(scanptr);
			SCAN_LOG(clientdll, "GetGameResources", scanptr);
		} else return false;

		scanptr = clientdll.hashScan(SECTION_TEXT, 0xa9e1fdc6, 0x7c03);
		if (scanptr != nullptr) {
			scanptr = scanptr.get<size_t>(2).to<void*>();
			if (scanptr != nullptr) {
				m_predictionRandomSeed.set(scanptr);
				SCAN_LOG(clientdll, "C_BaseEntity::m_nPredictionRandomSeed", scanptr);
			} else return false;
		} else return false;

		scanptr = clientdll.hashScan(SECTION_TEXT, 0xBBF5D86F, 0x61E1);
		if (scanptr != nullptr) {
			scanptr = jmpaddress<void*>(scanptr);
			if (scanptr != nullptr) {
				m_getWpnData.set(scanptr);
				SCAN_LOG(clientdll, "C_BaseCombatWeapon::GetWpnData", scanptr);
			} else return false;
		} else return false;

		scanptr = clientdll.hashScan(SECTION_TEXT, 0xD99ECFE7, 0x3FF);
		if (scanptr != nullptr) {
			m_weaponIDToAlias.set(scanptr);
			SCAN_LOG(clientdll, "WeaponIDToAlias", scanptr);
		} else return false;

		scanptr = clientdll.hashScan(SECTION_TEXT, 0x72CF4CAA, 0x7C30FF);
		if (scanptr != nullptr) {
			m_getBulletTypeParameters.set(scanptr);
			SCAN_LOG(clientdll, "C_CSPlayer::GetBulletTypeParameters", scanptr);
		} else return false;

		scanptr = clientdll.hashScan(SECTION_TEXT, 0xA52B73F5, 0x10FF);
		if (scanptr != nullptr) {
			m_inPredictionReturnAddress.set(scanptr);
			SCAN_LOG(clientdll, "InPrediction return address", scanptr);
		} else return false;
		
		scanptr = clientdll.hashScan(SECTION_TEXT, 0x8236E07B, 0xC3FF);
		if (scanptr != nullptr) {
			m_estimateAbsVelocity.set(scanptr);
			SCAN_LOG(clientdll, "C_BaseEntity::EstimateAbsVelocity", scanptr);
		} else return false;

		// credits: Badster
		scanptr = enginedll.hashScanFromReference(0x83BF2D09, 0x1FFFF, 0x68);
		if (scanptr != nullptr) {
			scanptr = findpattern(scanptr.get<size_t>(0x10), 0x10, 0xBFFA242D, 0x1);
			if (scanptr != nullptr) {
				scanptr = scanptr.get<size_t>(1).to<void*>();
				if (scanptr != nullptr) {
					gpGlobals->set(scanptr);
					SCAN_LOG(enginedll, "CGlobalVarsBase", scanptr);
				} else return false;
			} else return false;
		} else return false;

		scanptr = enginedll.hashScan(SECTION_TEXT, 0xA7C6263D, 0xF083);
		if (scanptr != nullptr) {
			scanptr = scanptr.get<size_t>(2).to<void*>();
			if (scanptr != nullptr) {
				m_lastGameNameUsed.set(scanptr);
				SCAN_LOG(enginedll, "LastGameNameUsed", scanptr);
			} else return false;
		} else return false;

		scanptr = enginedll.hashScan(SECTION_TEXT, 0xB8CE4298, 0xFF);
		if (scanptr != nullptr) {
			m_cvarCallbackReturnAddress.set(scanptr);
			SCAN_LOG(enginedll, "ConVar callback return address", scanptr);
		} else return false;

		return true;
	}

#undef SCAN_LOG
};