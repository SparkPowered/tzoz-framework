#pragma once

#include "const.h"
#include "../../util/color.h"
#include "../../util/vmthook.h"

namespace css
{
	class IConVar;

	typedef void (*FnChangeCallback_t)(IConVar *var, const char *pOldValue, float flOldValue);

	class IConVar {
	public:
		virtual void SetValue (const char *pValue) = 0;
		virtual void SetValue (float flValue) = 0;	
		virtual void SetValue (int nValue) = 0;
		virtual char *GetName() const = 0;
		virtual bool IsFlagSet (int nFlag) const = 0;
	};

	class ConCommandBase {
	public:
		virtual ~ConCommandBase() {} // 0x0

		ConCommandBase *m_pNext; // 0x4
		bool m_bRegistered; // 0x8
		char *m_pszName; // 0xc
		char *m_pszHelpString; // 0x10
		int m_nFlags; // 0x14
	};

	class ConVar : public ConCommandBase, public IConVar {
	public:
		virtual	~ConVar() {} // 0x18

		ConVar *m_pParent; // 0x1c
		char *m_pszDefaultValue; // 0x20
		char *m_pszString; // 0x24
		int	m_nStringLength; // 0x28
		float m_fValue; // 0x2c
		int	m_nValue; // 0x30
		bool m_bHasMin; // 0x34
		float m_fMinVal; // 0x38
		bool m_bHasMax; // 0x3c
		float m_fMaxVal; // 0x40
		FnChangeCallback_t m_fnChangeCallback; // 0x44
	};

	class CCvar : public mu::vmthook {
	public:
		CCvar()
		{
			reset();
		}

		ConVar *FindVar (mu::string name)
		{
			return old_method<ConVar *(__thiscall*)(void*, char*)>(12)(inst(), name.str());
		}
	};
};