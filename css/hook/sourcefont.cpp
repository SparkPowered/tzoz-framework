#include "../../stdafx.h"
#include "sourcefont.h"
#include "core.h"
#include "../../win/imports.h"

namespace css
{
	void sourcefont::reset()
	{
		m_font = 0;
		m_str.reset();
		m_init = false;
	}

	bool sourcefont::init (string name, int tall, int flags)
	{
		return m_init = vguisurface->SetFontGlyphSet(m_font = vguisurface->CreateFont(), name, tall, 0, 0, 0, flags);
	}

	void sourcefont::drawstr (int x, int y, bool center, color32 clr, string str)
	{
		int wide, tall;

		if (!m_init)
			return;

		mbstowcs(m_str, str, str.length());

		vguisurface->DrawSetTextFont(m_font);
		vguisurface->DrawSetTextColor(clr.r, clr.g, clr.b, clr.a);

		if (center)
		{
			vguisurface->GetTextSize(m_font, m_str.str(), wide, tall);
			vguisurface->DrawSetTextPos(x - (wide / 2), y);
		}
		else
		{
			vguisurface->DrawSetTextPos(x, y);
		}

		vguisurface->DrawPrintText(m_str.str(), m_str.length());
	}

	void sourcefont::drawstrfmt (int x, int y, bool center, color32 clr, string fmt, ...)
	{
		int wide, tall;
		va_list valist;
		staticstring<512> buf;

		if (!m_init)
			return;
		
		va_start(valist, fmt);
		imports->vsnprintf(buf, sizeof buf, fmt, valist);
		va_end(valist);

		mbstowcs(m_str, buf, buf.length());

		vguisurface->DrawSetTextFont(m_font);
		vguisurface->DrawSetTextColor(clr.r, clr.g, clr.b, clr.a);

		if (center)
		{
			vguisurface->GetTextSize(m_font, m_str.str(), wide, tall);
			vguisurface->DrawSetTextPos(x - (wide / 2), y);
		}
		else
		{
			vguisurface->DrawSetTextPos(x, y);
		}

		vguisurface->DrawPrintText(m_str.str(), m_str.length());
	}
};