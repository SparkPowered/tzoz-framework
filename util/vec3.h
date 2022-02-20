#pragma once

#include "mathlib.h"

namespace mu
{
	// Source SDK types
	typedef float matrix3x4_t[3][4];
	typedef float VMatrix[4][4];

	// generic 3D vector
	class vec3 {
	public:
		vec3()
		{
			zero();
		}	
	
		vec3 (float X, float Y, float Z)
		{
			init(X, Y, Z);
		}
	
		vec3 (const vec3& v)
		{
			*this = v;
		}

		explicit vec3 (const float *pf)
		{
			init(pf[0], pf[1], pf[2]);
		}

		explicit vec3 (float f)
		{
			x = y = z = f;
		}
	
		void init (float X, float Y, float Z)
		{
			x = X; y = Y; z = Z;
		}

		void zero()
		{
			x = y = z = 0.0f;
		}

		float *base()
		{
			return reinterpret_cast<float*>(this);
		}

		const float *base() const
		{
			return reinterpret_cast<const float*>(this);
		}
	
		bool operator== (const vec3& v) const
		{
			return (x == v.x) && (y == v.y) && (z == v.z);
		}

		bool operator== (float f) const
		{
			return (x == f) && (y == f) && (z == f);
		}

		float operator[](int i) const
		{
			switch (i) {
			case 1:
				return y;
			case 2:
				return z;
			default:
				return x;
			}
		}

		float &operator[](int i)
		{
			switch (i) {
			case 1:
				return y;
			case 2:
				return z;
			default:
				return x;
			}
		}

		vec3 &operator= (const vec3& v)
		{
			init(v.x, v.y, v.z);
			return (*this);
		}

		vec3 &operator= (const float *pf)
		{
			init(pf[0], pf[1], pf[2]);
			return (*this);
		}

		vec3 &operator+= (const vec3& v)
		{
			x += v.x; y += v.y; z += v.z;
			return (*this);
		}

		vec3 &operator-= (const vec3& v)
		{
			x -= v.x; y -= v.y; z -= v.z;
			return (*this);
		}

		vec3 &operator*= (const vec3& v)
		{
			x *= v.x; y *= v.y; z *= v.z;
			return (*this);
		}

		vec3 &operator/= (const vec3& v)
		{
			x /= v.x; y /= v.y; z /= v.z;
			return (*this);
		}

		vec3 &operator+= (float f)
		{
			x += f; y += f; z += f;
			return (*this);
		}

		vec3 &operator-= (float f)
		{
			x -= f; y -= f; z -= f;
			return (*this);
		}

		vec3 &operator*= (float f)
		{
			x *= f; y *= f; z *= f;
			return (*this);
		}

		vec3 &operator/= (float f)
		{
			x /= f; y /= f; z /= f;
			return (*this);
		}
	
		vec3 operator+ (const vec3& v) const
		{
			return vec3(x + v.x, y + v.y, z + v.z);
		}

		vec3 operator- (const vec3& v) const
		{
			return vec3(x - v.x, y - v.y, z - v.z);
		}

		vec3 operator* (const vec3& v) const
		{
			return vec3(x * v.x, y * v.y, z * v.z);
		}

		vec3 operator/ (const vec3& v) const
		{
			return vec3(x / v.x, y / v.y, z / v.z);
		}

		vec3 operator+ (float f) const
		{
			return vec3(x + f, y + f, z + f);
		}

		vec3 operator- (float f) const
		{
			return vec3(x - f, y - f, z - f);
		}

		vec3 operator* (float f) const
		{
			return vec3(x * f, y * f, z * f);
		}

		vec3 operator/ (float f) const
		{
			return vec3(x / f, y / f, z / f);
		}
		
		vec3 negate()
		{
			return vec3(-x, -y, -z);
		}

		float sqlength2d() const
		{
			return (x * x) + (y * y);
		}

		float sqlength() const
		{
			return sqlength2d() + (z * z);
		}

		float length2d() const
		{
			return sqrt(sqlength2d());
		}

		float length() const
		{
			return sqrt(sqlength());
		}

		float dot (const vec3& v) const
		{
			return (x * v.x) + (y * v.y) + (z * v.z);
		}

		float dot2d (const vec3 &v) const
		{
			return (x * v.x) + (y * v.y);
		}

		float dot (const float *pf) const
		{
			return (x * pf[0]) + (y * pf[1]) + (z * pf[2]);
		}

		float dist (const vec3& v) const
		{
			return (v - (*this)).length();
		}

		float dist2d (const vec3& v) const
		{
			return (v - (*this)).length2d();
		}

		vec3 &transform (const vec3& v, const matrix3x4_t& m);
		vec3 &cross (const vec3& a, const vec3& b);
		float vectorNormalize();
		float vectorNormalize2D();
		float angleFOV (const vec3& src, const vec3& dest) const; // class is the angle set
		float angleBetween (vec3 &v) const;
		vec3 &angleNormalize();
		void angleVectors (vec3 *f, vec3 *r, vec3 *u) const;
		void angleVectorsTranspose (vec3 *f, vec3 *r, vec3 *u) const;
		void compensateSpread (float sx, float sy); // class is the angle set
		void vectorAngles (vec3 &ang, vec3 *pseudoUp = nullptr) const;
		void vectorVectors (vec3 &r, vec3 &u) const;

		float x, y, z;
	};
};