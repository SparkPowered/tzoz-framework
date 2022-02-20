#pragma once

#include "../../util/vmthook.h"
#include "../../util/vec3.h"

using namespace mu;

namespace css
{
	struct ModelRenderInfo_t
	{
		vec3 origin;
		vec3 angles;
		void *renderable;
		const model_t *model;
		const matrix3x4_t *modeltoworld;
		const matrix3x4_t *lightingoffset;
		const float *lightingorigin;
		int flags;
		int entity_index;
		int skin;
		int body;
		int hitboxset;
		word instance;
	};

	class CRenderView : public vmthook {
	public:
		CRenderView() : vmthook() {}

		void SetColorModulation (const float *color);
		void GetColorModulation (float *color);
	};
	
	inline void CRenderView::SetColorModulation (const float *color)
	{
		old_method<void (__thiscall*)(void*, const float*)>(6)(inst(), color);
	}

	inline void CRenderView::GetColorModulation (float *color)
	{
		old_method<void (__thiscall*)(void*, float*)>(7)(inst(), color);
	}

	class CModelRender : public vmthook {
	public:
		CModelRender() : vmthook() {}

		void DrawModelExecute (void *thisptr, void *state, const ModelRenderInfo_t &info, matrix3x4_t *matrix);
	};

	inline void CModelRender::DrawModelExecute (void *thisptr, void *state, const ModelRenderInfo_t &info, matrix3x4_t *matrix)
	{
		old_method<void (__thiscall*)(void*, void*, const ModelRenderInfo_t&, matrix3x4_t*)>(19)(thisptr, state, info, matrix);
	}
};