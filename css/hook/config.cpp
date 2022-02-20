#include "../../stdafx.h"
#include "config.h"

#include "../../hash/hash.h"

#include "../../win/imports.h"

namespace css
{
	static configmgr _cfg;
	configmgr *cfg = &_cfg;

	ctrlvar::~ctrlvar()
	{
		cfg->unlink(this);
	}

	void ctrlvar::reset()
	{
		m_name.reset();
		m_value = m_min = m_max = m_step = 0;
		m_isRegistered = false;
	}

	void ctrlvar::init (string name, int value /*= 0*/, int min /*= 0*/, int max /*= 1*/, int step /*= 1*/)
	{
		m_name = name;
		m_value = value;
		m_min = min;
		m_max = max;
		m_step = step;		
	}
		
	void ctrlvar::link()
	{
		cfg->link(this);
	}

	// maxLength = 32
	void ctrlvar::strvalue (string value, bool booleanStr /*= true*/)
	{
		static xstring<4> enc_fmt(/*%d*/ 0x02, 0xF9, 0xDC9E0000);
		static xstring<4> enc_true(/*true*/ 0x04, 0xB6, 0xC2C5CDDC);
		static xstring<8> enc_false(/*false*/ 0x05, 0xD1, 0xB7B3BFA7, 0xB0000000);

		if (isBoolean() && booleanStr)
		{
			value.copy(!m_value ? enc_false.decrypt() : enc_true.decrypt());
		}
		else
		{
			imports->snprintf(value, 32, enc_fmt.decrypt(), m_value);
		}
	}

	void configmgr::reset()
	{		
		m_name.reset();
		m_file.reset();
		m_list = nullptr;
	}

	void configmgr::init (string name, string dir, string path)
	{
		if (m_list == nullptr)
			m_list = new linkedlist<ctrlvar*>();

		m_name = name;
		m_file = dir;
		m_file.append(path);
	}

	void configmgr::shutdown()
	{
		if (m_list != nullptr)
		{
			// destroy all items in the list
			delete m_list;
			m_list = nullptr;
		}

		m_name.reset();
		m_file.reset();
	}

	void configmgr::load()
	{
		staticstring<32> value;
		staticstring<256> buf;

		for (linkednode<ctrlvar*> *it = m_list->begin(); it != nullptr; it = it->m_next)
		{
			// get the value as a string
			it->m_info->strvalue(value.str(), false);

			// parse the .ini file
			imports->getPrivateProfileString(m_name, it->m_info->m_name, value, buf, sizeof buf, m_file);

			// set the last value we saved
			it->m_info->m_value = mu::atoi(buf);
		}
	}

	void configmgr::save()
	{
		staticstring<32> value;

		for (linkednode<ctrlvar*> *it = m_list->begin(); it != nullptr; it = it->m_next)
		{
			// get the value as a string
			it->m_info->strvalue(value.str(), false);

			// write to the .ini file
			imports->writePrivateProfileString(m_name, it->m_info->m_name, value, m_file);
		}
	}

	bool configmgr::link (ctrlvar *var)
	{
		if (var == nullptr)
			return false;

		// check if we've already linked it
		if (!var->m_isRegistered || m_list->search(var) == nullptr)
		{
			// seal of approval
			var->m_isRegistered = true;

			// link that shit
			m_list->insertHead(var);
		}

		return true;
	}

	void configmgr::unlink (ctrlvar *var)
	{
		// remove corresponding node from the list
		m_list->remove(var);
		
		// unregister
		var->m_isRegistered = false;
	}

	// find by hash if desired
	ctrlvar *configmgr::find (dword hash)
	{
		for (linkednode<ctrlvar*> *it = m_list->begin(); it != nullptr; it = it->m_next)
		{
			if (hash_t(it->m_info->m_name.str()) == hash)
				return it->m_info;
		}

		return nullptr;
	}
};