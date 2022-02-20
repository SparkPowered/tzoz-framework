#pragma once

#include "../../util/vmthook.h"
#include "../../util/wstring.h"

using namespace mu;

#ifdef CreateFont
#undef CreateFont
#endif

#ifdef PlaySound
#undef PlaySound
#endif

namespace css
{
	enum FontDrawType_t
	{
		// Use the "additive" value from the scheme file
		FONT_DRAW_DEFAULT = 0,
		// Overrides
		FONT_DRAW_NONADDITIVE,
		FONT_DRAW_ADDITIVE,
		FONT_DRAW_TYPE_COUNT = 2,
	};

	// adds to the font
	enum EFontFlags
	{
		FONTFLAG_NONE,
		FONTFLAG_ITALIC = 0x001,
		FONTFLAG_UNDERLINE = 0x002,
		FONTFLAG_STRIKEOUT = 0x004,
		FONTFLAG_SYMBOL = 0x008,
		FONTFLAG_ANTIALIAS = 0x010,
		FONTFLAG_GAUSSIANBLUR = 0x020,
		FONTFLAG_ROTARY = 0x040,
		FONTFLAG_DROPSHADOW = 0x080,
		FONTFLAG_ADDITIVE = 0x100,
		FONTFLAG_OUTLINE = 0x200,
		FONTFLAG_CUSTOM = 0x400, // custom generated font - never fall back to asian compatibility mode
		FONTFLAG_BITMAP = 0x800, // compiled bitmap font - no fallbacks
	};

	class Surface : public vmthook {
	public:
		Surface()
		{
			reset();
		}
		
		void DrawSetColor (int r, int g, int b, int a)
		{
			old_method<void (__thiscall*)(void*, int, int, int, int)>(11)(inst(), r, g, b, a);
		}

		void DrawFilledRect (int x, int y, int w, int h)
		{
			old_method<void (__thiscall*)(void*, int, int, int, int)>(12)(inst(), x, y, w, h);
		}

		void DrawFilledRect (int x, int y, int w, int h, color32 clr)
		{
			DrawSetColor(clr.r, clr.g, clr.b, clr.a);
			DrawFilledRect(x, y, x + w, y + h);
		}

		void DrawOutlinedRect (int x, int y, int w, int h)
		{
			old_method<void (__thiscall*)(void*, int, int, int, int)>(14)(inst(), x, y, w, h);
		}
		
		void DrawOutlinedRect (int x, int y, int w, int h, color32 clr)
		{
			DrawSetColor(clr.r, clr.g, clr.b, clr.a);
			DrawOutlinedRect(x, y, x + w, y +h);
		}

		void DrawLine (int x0, int y0, int x1, int y1)
		{
			old_method<void (__thiscall*)(void*, int, int, int, int)>(15)(inst(), x0, y0, x1, y1);
		}

		void DrawLine (int x0, int y0, int x1, int y1, color32 clr)
		{
			DrawSetColor(clr.r, clr.g, clr.b, clr.a);
			DrawLine(x0, y0, x1, y1);
		}

		void DrawSetTextFont (ulong font)
		{
			old_method<void (__thiscall*)(void*, ulong)>(17)(inst(), font);
		}

		void DrawSetTextColor (int r, int g, int b, int a)
		{
			old_method<void (__thiscall*)(void*, int, int, int, int)>(19)(inst(), r, g, b, a);
		}

		void DrawSetTextPos (int x, int y)
		{
			old_method<void (__thiscall*)(void*, int, int)>(20)(inst(), x, y);
		}

		void DrawPrintText (wchar_t *str, int length, dword type = 0)
		{
			old_method<void (__thiscall*)(void*, wchar_t*, int, dword)>(22)(inst(), str, length, type);
		}

		ulong CreateFont()
		{
			return old_method<ulong (__thiscall*)(void*)>(66)(inst());
		}

		bool SetFontGlyphSet (ulong font, string name, int tall, int weight, int blur, int scanLines, int flags, int rangeMin = 0, int rangeMax = 0)
		{
			return old_method<bool (__thiscall*)(void*, ulong, char*, int, int, int, int, int, int, int)>(67)(inst(), font, name, tall, weight, blur, scanLines, flags, rangeMin, rangeMax);
		}

		int GetFontTall (ulong font)
		{
			return old_method<int (__thiscall*)(void*, ulong)>(69)(inst(), font);
		}

		void GetTextSize (ulong font, wchar_t *text, int &wide, int &tall)
		{
			old_method<void (__thiscall*)(void*, ulong, wchar_t*, int&, int&)>(75)(inst(), font, text, wide, tall);
		}

		void PlaySound (string filename)
		{
			old_method<void (__thiscall*)(void*, char*)>(78)(inst(), filename);
		}
	};
};