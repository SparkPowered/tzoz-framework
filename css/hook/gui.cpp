#include "../../stdafx.h"
#include "gui.h"
#include "core.h"
#include "settings.h"

#include "../../win/imports.h"

namespace css
{
	static guicore _gui;
	guicore *gui = &_gui;
	
	void guitree::init (string title)
	{
		m_title = title;
		m_active = false;	
		m_currentIterator = nullptr;
		m_branches.reset();		
	}
	
	// add by name
	guitree *guitree::grow (string title)
	{
		for (auto it = m_branches.begin(); it != m_branches.end(); ++it)
		{
			if (!(*it)->m_title.cmp(title))
				return *it;
		}

		return grow(new guitree(title));
	}

	// add by address
	guitree *guitree::grow (guitree *node)
	{
		for (auto it = m_branches.begin(); it != m_branches.end(); ++it)
		{
			if (*it == node)
				return *it;
		}

		m_branches.append(node);
		m_currentIterator = m_branches.begin();
		return node;
	}

	// remove by name
	void guitree::shrink (string title)
	{
		for (dword i = 0; i < m_branches.count(); i++)
		{
			if (!m_branches[i]->m_title.cmp(title))
			{
				delete m_branches[i];
				m_branches.remove(i);
			}
		}
	}

	// remove by address
	void guitree::shrink (guitree *node)
	{
		for (dword i = 0; i < m_branches.count(); i++)
		{
			if (m_branches[i] == node)
			{
				delete m_branches[i];
				m_branches.remove(i);
			}
		}
	}

	// this is for the base menu object from which all other items derive
	// the base object is invisible hence no call to renderSelf()
	void guitree::baseDraw (sourcefont *font, int x, int y)
	{
		if (!m_active)
			return;

		//font->drawstr(x, y - 20, false, color32(-1), m_title.str());
		
		// start rendering all the derived objects
		for (auto it = m_branches.begin(); it != m_branches.end(); ++it)
		{
			y += 26;
			(*it)->render(font, x, y, m_currentIterator == it);
		}
	}

	// this is for the different (base) categories of the menu
	void guitree::render (sourcefont *font, int x, int y, bool on)
	{
		renderSelf(font, x, y, m_title.str(), on);

		if (!m_active)
			return;

		x += 109;

		// render the sub-derived objects
		for (auto it = m_branches.begin(); it != m_branches.end(); ++it)
		{
			(*it)->render(font, x, y, m_currentIterator == it);
			y += 26;
		}
	}

	// display the actual node
	void guitree::renderSelf (sourcefont *font, int x, int y, string title, bool on)
	{
		color32 col(
			settings->gui.r->m_value,
			settings->gui.g->m_value,
			settings->gui.b->m_value,
			!on ? 95 : 225);
			
		if (on)
		{
			col.r = 255 - col.r;
			col.g = 255 - col.g;
			col.b = 255 - col.b;
		}

		// light outline, filled interior, black outline
		vguisurface->DrawOutlinedRect(x - 6, y - 4, 105, 25, color32(192, 225, 255, 255));
		vguisurface->DrawFilledRect(x - 4, y - 2, 102, 23, col);
		vguisurface->DrawOutlinedRect(x - 5, y - 3, 104, 24, color32(0, 0, 0, 255));
		
		// show title
		font->drawstr(x, y, false, color32(-1), title);
	}

	// display control variables
	void guileaf::render (sourcefont *font, int x, int y, bool on)
	{
		staticstring<64> value;

		color32 colval, col(
			settings->gui.r->m_value,
			settings->gui.g->m_value,
			settings->gui.b->m_value,
			!m_active ? 95 : 225);
			
		if (m_active)
		{
			col.r = 255 - col.r;
			col.g = 255 - col.g;
			col.b = 255 - col.b;
		}

		// variable name
		renderSelf(font, x, y, m_title.str(), on);
		
		// variable value; this is the terminal node
		// light outline, filled interior, black outline
		vguisurface->DrawOutlinedRect(x + 103, y - 4, 47, 25, color32(192, 225, 255, 255));
		vguisurface->DrawFilledRect(x + 105, y - 2, 44, 23, col);
		vguisurface->DrawOutlinedRect(x + 104, y - 3, 46, 24, color32(0, 0, 0, 255));

		// value color
		if (m_var->isBoolean())
		{
			if (m_var->m_value == 0)
				colval.set(255, 71, 71, 255);
			else
				colval.set(56, 255, 92, 255);
		}
		else
		{
			colval.set(-1);
		}

		// draw the string value
		m_var->strvalue(value.str());
		font->drawstr(x + 108, y + 1, false, colval, value.str());
	}

	// select settings
	bool guitree::processInput (WPARAM wparam, LPARAM lparam, UINT msg)
	{
		bool result = false;

		if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK)
		{
			cfg->save();
			m_active = !m_active;
			result = true;
		}

		if (!m_active)
			return false;

		if (m_currentIterator != nullptr)
		{
			if (!(*m_currentIterator)->m_active)
			{
				if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK)
				{
					if ((*m_currentIterator)->selectable())
						(*m_currentIterator)->m_active = true;

					result = true;
				}
				else if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK)
				{
					m_active = false;
					result = true;
				}
				else if (msg == WM_MOUSEWHEEL)
				{
					short delta = HIWORD(wparam);

					if (delta > 0) // up
					{
						if (m_currentIterator == m_branches.begin())
							m_currentIterator = m_branches.end();

						m_currentIterator--;

						result = true;
					}
					else if (delta < 0) // down
					{
						if ((++m_currentIterator) == m_branches.end())
							m_currentIterator = m_branches.begin();

						result = true;
					}
				}
			}
			else
			{
				result = (*m_currentIterator)->processInput(wparam, lparam, msg);
			}
		}

		return result;
	}
	
	// change settings
	bool guileaf::processInput (WPARAM wparam, LPARAM lparam, UINT msg)
	{
		if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK)
		{
			cfg->save();
			m_active = false;			
			return true;
		}
		else if (msg == WM_MOUSEWHEEL)
		{
			short delta = HIWORD(wparam);

			if (delta > 0) // up
			{
				increment();
				return true;
			}
			else if (delta < 0) // down
			{
				decrement();
				return true;
			}
		}

		return false;
	}

	// increase cvar value
	void guileaf::increment()
	{
			if (m_var == nullptr)
			return;

		if (!m_var->isBoolean())
		{
			if (m_var->m_value < m_var->m_max)
				m_var->m_value += m_var->m_step;
		}
		else
		{
			m_var->m_value = m_var->m_max;
		}
	}
	
	// decrease cvar value
	void guileaf::decrement()
	{
		if (m_var == nullptr)
			return;

		if (!m_var->isBoolean())
		{
			if (m_var->m_value > m_var->m_min)
				m_var->m_value -= m_var->m_step;
		}
		else
		{
			m_var->m_value = m_var->m_min;
		}
	}
	
	// use this hooked callback to listen for user input
	// credits: s0beit
	LRESULT CALLBACK newWindowProc (HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		// pass to our menu class
		if (gui->m_menu.processInput(wparam, lparam, msg))
			return 0;
		
		return imports->callWindowProc(gui->m_wndProc, wnd, msg, wparam, lparam);
	}
	
	bool guicore::keyHook()
	{
		static xstring<8> classname(/*Valve001*/ 0x08, 0x08, 0x5E68667D, 0x693D3E3E);
		
		// Valve001 is the main window class used by Source Engine
		HWND wnd = imports->findWindowEx(nullptr, nullptr, classname.decrypt(), nullptr);

		if (wnd == nullptr)
			return false;

		// hook this window procedure, and save the original
		m_wndProc = (WNDPROC)imports->setWindowLong(wnd, GWL_WNDPROC, (LONG)&newWindowProc);
		
		return m_wndProc != nullptr;
	}
	
	void guicore::reset()
	{
		m_wndProc = nullptr;
		m_font.reset();
	}
	
	void guicore::init()
	{
		static xstring<8> _aim(/*aimbot*/ 0x06, 0x5C, 0x3D34333D, 0x0F150000);		
		static xstring<4> _esp(/*esp*/ 0x03, 0x87, 0xE2FBF900);
		static xstring<8> _player(/*player*/ 0x06, 0xC2, 0xB2AFA5BC, 0xA3B50000);
		static xstring<4> _hvh(/*hvh*/ 0x03, 0xF8, 0x908F9200);
		static xstring<4> _misc(/*misc*/ 0x04, 0xD3, 0xBEBDA6B5);
		static xstring<4> _gui(/*gui*/ 0x03, 0x80, 0xE7F4EB00);

		guitree *aim, *esp, *hvh, *misc, *ggui;
		guileafproxy *playeresp;

		m_menu.init("");

		// aimbot menu
		aim = m_menu.grow(_aim.decrypt());
		aim->grow(new guileaf(settings->aim.enable));
		aim->grow(new guileaf(settings->aim.triggerbot));
		aim->grow(new guileaf(settings->aim.priority));
		aim->grow(new guileaf(settings->aim.fov));
		aim->grow(new guileaf(settings->aim.silent));
		aim->grow(new guileaf(settings->aim.body));
		aim->grow(new guileaf(settings->aim.autowall));
		aim->grow(new guileaf(settings->aim.deathmatch));
		aim->grow(new guileaf(settings->aim.friends));
		aim->grow(new guileaf(settings->aim.team));
		aim->grow(new guileaf(settings->aim.antispread));
		aim->grow(new guileaf(settings->aim.antirecoil));
		
		// player esp menu
		playeresp = new guileafproxy(_player.decrypt());
		playeresp->grow(new guileaf(settings->esp.name));
		playeresp->grow(new guileaf(settings->esp.health));
		playeresp->grow(new guileaf(settings->esp.weapon));
		playeresp->grow(new guileaf(settings->esp.skeleton));
		playeresp->grow(new guileaf(settings->esp.team));
		
		// base esp menu
		esp = m_menu.grow(_esp.decrypt());
		esp->grow(new guileaf(settings->esp.enable));
		esp->grow(new guileaf(settings->esp.world));
		esp->grow(new guileaf(settings->esp.chams));
		esp->grow(playeresp);

		// hack versus hack menu
		hvh = m_menu.grow(_hvh.decrypt());
		hvh->grow(new guileaf(settings->hvh.antiaim));
		hvh->grow(new guileaf(settings->hvh.fakeduck));
		hvh->grow(new guileaf(settings->hvh.fakelag));
		hvh->grow(new guileaf(settings->hvh.lagfactor));

		// misc menu
		misc = m_menu.grow(_misc.decrypt());
		misc->grow(new guileaf(settings->misc.antismac));
		misc->grow(new guileaf(settings->misc.speedhack));
		misc->grow(new guileaf(settings->misc.speedfactor));
		misc->grow(new guileaf(settings->misc.namesteal));

		// gui color selector
		ggui = m_menu.grow(_gui.decrypt());
		ggui->grow(new guileaf(settings->gui.r));
		ggui->grow(new guileaf(settings->gui.g));
		ggui->grow(new guileaf(settings->gui.b));
	}
};