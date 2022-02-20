#pragma once

#include "../stdafx.h"

namespace mu
{
	// general purpose integer colors
	class color32 {
	public:	
		color32()
		{
			clear();
		}

		color32 (byte R, byte G, byte B, byte A)
		{
			set(R, G, B, A);
		}

		color32 (const byte *p)
		{
			set(p[0], p[1], p[2], p[3]);
		}
		
		color32 (dword c)
		{
			set(c);
		}
		
		operator byte*()
		{
			return reinterpret_cast<byte*>(this);
		}

		operator const byte*() const
		{
			return reinterpret_cast<const byte*>(this);
		}
	
		void clear()
		{
			r = g = b = a = 0;
		}

		void set (byte R, byte G, byte B, byte A)
		{
			r = R; g = G; b = B; a = A;
		}

		color32 &operator= (const color32 &c)
		{
			set(c.r, c.g, c.b, c.a);
			return *this;
		}	
	
		void set (dword c)
		{
			r = (c >> 24) & 0xff;
			g = (c >> 16) & 0xff;
			b = (c >> 8) & 0xff;
			a = c & 0xff;
		}

		dword toint() const
		{
			return (r << 24)|(g << 16)|(b << 8)|(a);
		}
	
		byte r, g, b, a;
	};

	// floating point colors
	class fcolor32 {
	public:
		fcolor32()
		{
			clear();
		}

		fcolor32 (float R, float G, float B, float A)
		{
			set(R, G, B, A);
		}

		fcolor32 (const float *p)
		{
			set(p[0], p[1], p[2], p[3]);
		}
			
		operator float*()
		{
			return (float*)this;
		}

		operator const float*() const
		{
			return (const float*)this;
		}

		long *toint()
		{
			return (long*)this;
		}

		const long *toint() const
		{
			return (const long*)this;
		}
	
		void clear()
		{
			r = g = b = a = 0.0f;
		}

		void set (float *f)
		{
			r = f[0]; g = f[1]; b = f[2]; a = f[3];
		}

		void set (float R, float G, float B)
		{
			r = R; g = G; b = B;
		}

		void set (float R, float G, float B, float A)
		{
			r = R; g = G; b = B; a = A;
		}

		fcolor32 &operator= (const fcolor32& c)
		{
			set(c.r, c.g, c.b, c.a);
			return *this;
		}
	
		float r, g, b, a;
	};
};