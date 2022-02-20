#pragma once

#include "../../util/vmthook.h"
#include "../../util/vec3.h"

#include "client_class.h"

using namespace mu;

namespace css
{
	typedef enum
	{
		FRAME_UNDEFINED = -1, // (haven't run any frames yet)
		FRAME_START,
		// A network packet is being recieved
		FRAME_NET_UPDATE_START,
		// Data has been received and we're going to start calling PostDataUpdate
		FRAME_NET_UPDATE_POSTDATAUPDATE_START,
		// Data has been received and we've called PostDataUpdate on all data recipients
		FRAME_NET_UPDATE_POSTDATAUPDATE_END,
		// We've received all packets, we can now do interpolation, prediction, etc..
		FRAME_NET_UPDATE_END,
		// We're about to start rendering the scene
		FRAME_RENDER_START,
		// We've finished rendering the scene.
		FRAME_RENDER_END
	} ClientFrameStage_t;

	class CHLClient : public vmthook {
	public:
		CHLClient()
		{
			reset();
		}
		
		ClientClass *GetAllClasses() 
		{
			return old_method<ClientClass *(__thiscall*)(void*)>(8)(inst());
		}

		void CreateMove (void *thisptr, int sequence_number, float frametime, bool active)
		{
			old_method<void (__thiscall*)(void*, int, float, bool)>(21)(thisptr, sequence_number, frametime, active);
		}
	};
	
	class CViewVectors {
	public:
		vec3 m_vView;
		vec3 m_vHullMin;
		vec3 m_vHullMax;
		vec3 m_vDuckHullMin;
		vec3 m_vDuckHullMax;
		vec3 m_vDuckView;
		vec3 m_vObsHullMin;
		vec3 m_vObsHullMax;
		vec3 m_vDeadViewHeight;
	};

	class CGameRules : public xaddress {
	public:
		CGameRules()
		{
			reset();
		}

		bool IsFreezePeriod();
		bool ShouldCollide (int collisionGroup0, int collisionGroup1);
		const CViewVectors *GetViewVectors();
		
		// we're actually storing the reference in this instance
		// the class pointer itself is not initialized until later
		address inst()
		{
			return get().to<void*>();
		}
	};
};