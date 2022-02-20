#pragma once

namespace mu
{
	// const
	#define m_pi			3.14159265358979323846
	#define flt_epsilon		1.19209290E-07F
	#define deg2rad(deg)	((deg) * (m_pi/180.0f))
	#define rad2deg(rad)	((rad) * (180.0/m_pi))
	#define	ang2short(ang)	((int)((ang) * (65536.0f/360.0f)) & 65535)
	#define short2ang(sh)	((sh) * (360.0f/65536.0f))
	#define float2int(f)	_mm_cvtt_ss2si(_mm_load_ss(f))

	// asm math
	extern float sqrt (float x);
	extern float sin (float f);
	extern float cos (float f);
};