#pragma once

#include "core.h"

namespace css
{
	extern void __fastcall newCreateMove (void *ecx, void *edx, int sequence_number, float frametime, bool active);
	extern void __fastcall newFrameStageNotify (void *ecx, void *edx, ClientFrameStage_t stage);
	extern void __fastcall newPaintTraverse (void *ecx, void *edx, dword handle, bool forceRepaint, bool allowForce);
	extern bool __fastcall newInPrediction (void *ecx, void *edx);
	extern void __fastcall newRunCommand (void *ecx, void *edx, void *player, CUserCmd *ucmd, void *moveHelper);
	extern void __fastcall newDrawModelExecute (void *ecx, void *edx, void *state, const ModelRenderInfo_t &info, matrix3x4_t *matrix);
};