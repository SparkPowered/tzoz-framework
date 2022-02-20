#include "../../stdafx.h"
#include "enginetrace.h"

namespace css
{
	void Ray_t::Init (const vec3 &src, const vec3 &dest)
	{
		*(vec3*)&m_Delta = dest - src;
			
		m_IsSwept = (*(vec3*)&m_Delta).sqlength() != 0.0f;
		m_IsRay = true;
			
		*(vec3*)&m_Start = src;
			
		(*(vec3*)&m_Extents).zero();
		(*(vec3*)&m_Offset).zero();
	}
		
	void Ray_t::Init (const vec3 &src, const vec3 &dest, const vec3 &mins, const vec3 &maxs)
	{
		*(vec3*)&m_Delta = dest - src;
			
		m_IsSwept = (*(vec3*)&m_Delta).sqlength() != 0.0f;
			
		*(vec3*)&m_Extents = (maxs - mins) * 0.5f;
			
		m_IsRay = (*(vec3*)&m_Extents).sqlength() < 1e-6;
			
		*(vec3*)&m_Offset = (mins + maxs) * 0.5f;
		*(vec3*)&m_Start = src + *(vec3*)&m_Offset;
		*(vec3*)&m_Offset *= -1.0f;
	}
};