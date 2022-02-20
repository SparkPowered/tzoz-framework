#include "../../stdafx.h"
#include "hooks.h"
#include "drawpanel.h"
#include "client.h"
#include "settings.h"

namespace css
{
	// called in CL_Move in engine.dll
	// this prepares the user command before sending to the server
	void __fastcall newCreateMove (void *ecx, void *edx, int sequence_number, float frametime, bool active)
	{
		volatile void *ebp = getEBP();

		move->preMove((void*)ebp, sequence_number, active);
		g_ClientDLL->CreateMove(ecx, sequence_number, frametime, active);
		move->postMove();
	}

	// engine drawing done here on an existing panel
	void __fastcall newPaintTraverse (void *ecx, void *edx, dword handle, bool forceRepaint, bool allowForce)
	{
		vguipanel->PaintTraverse(ecx, handle, forceRepaint, allowForce);
		draw->frame(handle);
	}

	// this gets called in CalcPlayerView
	// we can intercept it and remove visual recoil
	bool __fastcall newInPrediction (void *ecx, void *edx)
	{
		volatile void *ebp = getEBP();

		bool result = prediction->InPrediction(ecx);

		if (settings->aim.antirecoil->m_value && clientmove::inPrediction())
			result = true; // remove view shake

		return result;
	}

	// shared prediction code gets run here
	// this is central to undoing certain precision errors
	void __fastcall newRunCommand (void *ecx, void *edx, void *player, CUserCmd *ucmd, void *moveHelper)
	{
		move->preCommand(player, ucmd, false);
		prediction->RunCommand(ecx, player, ucmd, moveHelper);
		move->postCommand(player, ucmd);
	}

	// player models are rendered here
	// this is where we modify them for chams
	void __fastcall newDrawModelExecute (void *ecx, void *edx, void *state, const ModelRenderInfo_t &info, matrix3x4_t *matrix)
	{
		// if this succeeds then we don't need to draw the model again
		if (settings->esp.enable->m_value && settings->esp.chams->m_value)
		{
			if (modelparams(ecx, state, info, matrix).doChams())
				return;
		}
		
		modelrender->DrawModelExecute(ecx, state, info, matrix);
	}
};