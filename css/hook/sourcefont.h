#pragma once

#include "../../util/color.h"
#include "../../util/wstring.h"

using namespace mu;

namespace css
{
	enum EFontFlags;

	class sourcefont {
	public:
		sourcefont()
		{
			reset();
		}

		void reset();
		bool init (string name, int tall, int flags);
		void drawstr (int x, int y, bool center, color32 clr, string str);
		void drawstrfmt (int x, int y, bool center, color32 clr, string fmt, ...);	

		operator ulong()
		{
			return m_font;
		}
		
	private:
		ulong m_font;
		staticwstring<512> m_str;
		bool m_init;
	};
};