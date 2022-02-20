#pragma once

#include "core.h"
#include "sourcefont.h"

namespace css
{
	// engine drawing in PaintTraverse hook
	// esp, menu, etc.
	class drawpanel {
	public:
		drawpanel()
		{
			m_fontName.reset();
			m_fontData.reset();
			m_fontUi.reset();
		}

		bool init();
		void frame (dword handle);
		void paint();
		void playerESP();
		void worldESP();

		sourcefont m_fontName, m_fontData, m_fontUi;
	};

	extern drawpanel *draw;

	// used in DrawModelExecute hook
	// chams, wallhack, etc.
	class modelparams {
	public:
		modelparams()
		{
		}

		modelparams (void *ecx, void *state, const ModelRenderInfo_t &info, matrix3x4_t *matrix)
		{
			init(ecx, state, info, matrix);
		}

		void init (void *ecx, void *state, const ModelRenderInfo_t &info, matrix3x4_t *matrix);
		bool doChams();

	private:
		void *m_ecx;
		void *m_state;
		ModelRenderInfo_t m_info;
		matrix3x4_t *m_matrix;
	};
};