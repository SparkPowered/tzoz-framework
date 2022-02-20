#pragma once

#include "sourcefont.h"
#include "config.h"

#include "../../util/color.h"
#include "../../util/vector.h"

using namespace mu;

namespace css
{
	// credits: AkaneTendo
	class guitree {
	public:
		guitree()
		{
			init("");
		}
		
		guitree (string title)
		{
			init(title);
		}

		virtual ~guitree()
		{
			shutdown();
		}
		
		virtual void init (string title);

		// delete all contents
		virtual void shutdown()
		{
			m_branches.free_contents();
		}
		
		virtual void baseDraw (sourcefont *font, int x, int y);
		virtual void render (sourcefont *font, int x, int y, bool on);
		virtual bool processInput (WPARAM wparam, LPARAM lparam, UINT msg);
		
		virtual bool selectable()
		{
			return m_branches.begin() != m_branches.end();
		}
		
		guitree *grow (string title);
		guitree *grow (guitree *node);
		
		void shrink (string title);
		void shrink (guitree *node);

		// currently within
		bool m_active;

	protected:
		// base object
		static void renderSelf (sourcefont *font, int x, int y, string title, bool on);
		
		// derived sections
		vector<guitree*> m_branches;

		// label for the section
		staticstring<64> m_title;

	private:
		// current selected node
		guitree **m_currentIterator;
	};

	// sub-category
	class guileafproxy : public guitree {
	public:
		guileafproxy (string title)
			: guitree(title)
		{}

		virtual void increment() {}
		virtual void decrement() {}
	};

	// terminal category for a variable
	class guileaf : public guileafproxy {
	public:
		guileaf (ctrlvar *var)
			: guileafproxy(var->m_name.str()), m_var(var)
		{}
		
		virtual void render (sourcefont *font, int x, int y, bool on);
		virtual bool processInput (WPARAM wparam, LPARAM lparam, UINT msg);

		virtual bool selectable()
		{
			return true;
		}

		virtual void increment();
		virtual void decrement();

	private:
		ctrlvar *m_var;
	};

	// container class for menu
	class guicore {
	public:
		guicore()
		{
			reset();
		}

		~guicore()
		{
		}

		void reset();
		void init();
		bool keyHook();
		
		// window hook
		WNDPROC m_wndProc;
		
		// draw font
		sourcefont m_font;

		// the gui
		guitree m_menu;
	};

	extern guicore *gui;
};