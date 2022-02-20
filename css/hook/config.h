#pragma once

#include "../../stdafx.h"

#include "../../util/linkedlist.h"

using namespace mu;

namespace css
{
	class ctrlvar {
	public:
		ctrlvar()
		{
			reset();
		}

		ctrlvar (string name, int value = 0, int min = 0, int max = 1, int step = 1)
		{
			init(name, value, min, max, step);
			link();
		}

		~ctrlvar();

		void reset();
		void init (string name, int value = 0, int min = 0, int max = 1, int step = 1);
		void link();
		void strvalue (string value, bool booleanStr = true);

		bool isValid() const
		{
			return (m_max != 0 && m_step != 0) || !m_isRegistered;
		}

		bool isBoolean() const
		{
			return m_min == 0 && m_max == 1;
		}

		// title
		staticstring<32> m_name;

		// values
		int m_value, m_min, m_max, m_step;

		// for the linked list
		bool m_isRegistered;
	};

	class configmgr {
	public:
		configmgr()
		{
			reset();
		}

		~configmgr()
		{
			shutdown();
		}

		void reset();
		void init (string name, string dir, string file);
		void shutdown();
		void load();
		void save();
		bool link (ctrlvar *var);
		void unlink (ctrlvar *var);
		ctrlvar *find (dword hash);

	private:
		linkedlist<ctrlvar*> *m_list;
		
		staticstring<32> m_name;
		staticstring<MAX_PATH> m_file;
	};

	extern configmgr *cfg;
};