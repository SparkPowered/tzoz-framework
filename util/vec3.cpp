#include "../stdafx.h"
#include "vec3.h"

#include "../win/imports.h"

namespace mu
{
	vec3 &vec3::transform (const vec3& v, const matrix3x4_t& m)
	{
		x = v.dot(m[0]) + m[0][3];
		y = v.dot(m[1]) + m[1][3];
		z = v.dot(m[2]) + m[2][3];

		return (*this);
	}

	vec3 &vec3::cross (const vec3& a, const vec3& b)
	{
		x = (a.y * b.z) - (a.z * b.y);
		y = (a.z * b.x) - (a.x * b.z);
		z = (a.x * b.y) - (a.y * b.x);

		return (*this);
	}

	float vec3::vectorNormalize()
	{
		float len = length();
		float inv = 1.0f / (len + FLT_EPSILON);

		*this *= inv;

		return len;
	}

	float vec3::vectorNormalize2D()
	{
		float len = length2d();
		float inv = 1.0f / (len + FLT_EPSILON);

		x *= inv;
		y *= inv;

		return len;
	}

	float vec3::angleFOV (const vec3& src, const vec3& dest) const
	{
		vec3 f, d;

		angleVectors(&f, nullptr, nullptr);

		d = dest - src;
		d.vectorNormalize();

		return max(f.angleBetween(d), 0.0f);
	}

	float vec3::angleBetween (vec3 &v) const
	{
		return rad2deg(imports->acos(dot(v)));
	}

	vec3 &vec3::angleNormalize()
	{
		if (x > 180.0f)
			x -= 360.0f;
		else if (x < -180.0f)
			x += 360.0f;

		if (y > 180.0f)
			y -= 360.0f;
		else if (y < -180.0f)
			y += 360.0f;

		return *this;
	}
	
	void vec3::compensateSpread (float sx, float sy)
	{
		vec3 dir, f, r, u, angles;
		
		// nothing to compensate
		if (sx == 0.0f && sy == 0.0f)
			return;
		
		// origin (null) angles
		vec3().angleVectors(&f, &r, &u);

		// spread direction to spread angles (relative to origin)
		dir = f + (r * sx) + (u * sy);
		dir.vectorNormalize();
		
		// reference up is not needed
		dir.vectorAngles(angles);

		// subtract our pitch angle
		angles.x -= x;
		
		// transpose the spread angles
		angles.angleNormalize().angleVectorsTranspose(&f, &r, &u);
		
		// now we use reference up to get the compensated angles
		f.vectorAngles(angles, &u);
		
		// add our yaw angle
		angles.y += y;
		
		// apply
		*this = angles.angleNormalize();
	}

	// translate 3d angle set into 3x3 matrix
	// each of these unit vectors are perpendicular to each other
	void vec3::angleVectors (vec3 *f, vec3 *r, vec3 *u) const
	{
		float sp, sy, sr, cp, cy, cr;

		vec3 rad(deg2rad(x), deg2rad(y), deg2rad(z));

		sp = sin(rad.x); cp = cos(rad.x);
		sy = sin(rad.y); cy = cos(rad.y);
		sr = sin(rad.z); cr = cos(rad.z);	

		if (f != nullptr)
		{
			f->x = cp * cy;
			f->y = cp * sy;
			f->z = -sp;
		}
	
		if (r != nullptr)
		{
			r->x = -sr * sp * cy + -cr * -sy;
			r->y = -sr * sp * sy + -cr * cy;
			r->z = -sr * cp;
		}
	
		if (u != nullptr)
		{
			u->x = cr * sp * cy + -sr * -sy;
			u->y = cr * sp * sy + -sr * cy;
			u->z = cr * cp;
		}
	}

	// translate 3d angle set into transposed 3x3 matrix 
	// each of these unit vectors are perpendicular to each other
	void vec3::angleVectorsTranspose (vec3 *f, vec3 *r, vec3 *u) const
	{
		float sp, sy, sr, cp, cy, cr;

		vec3 rad(deg2rad(x), deg2rad(y), deg2rad(z));

		sp = sin(rad.x); cp = cos(rad.x);
		sy = sin(rad.y); cy = cos(rad.y);
		sr = sin(rad.z); cr = cos(rad.z);	

		if (f != nullptr)
		{
			f->x = cp * cy;
			f->y = sr * sp * cy + cr * -sy;
			f->z = cr * sp * cy + -sr * -sy;
		}
		
		if (r != nullptr)
		{
			r->x = cp * sy;
			r->y = sr * sp * sy + cr * cy;
			r->z = cr * sp * sy + -sr * cy;
		}
	
		if (u != nullptr)
		{
			u->x = -sp;
			u->y = sr * cp;
			u->z = cr * cp;
		}
	}
	
	void vec3::vectorAngles (vec3 &ang, vec3 *pseudoUp /*= nullptr*/) const
	{
		vec3 left;

		float len = length2d();
		
		// up component is needed for roll angle
		if (pseudoUp != nullptr)
		{
			// get the final orthogonal vector in this 3D space
			left.cross(*pseudoUp, *this);
			left.vectorNormalize();

			if (len > 0.001f)
			{
				float adj = (left.y * x) - (left.x * y); // up.z

				ang.x = rad2deg(imports->atan2(-z, len));
				ang.y = rad2deg(imports->atan2(y, x));
				ang.z = rad2deg(imports->atan2(left.z, adj)); // opposite, adjacent
			}
			else
			{
				ang.x = rad2deg(imports->atan2(-z, len));
				ang.y = rad2deg(imports->atan2(-left.x, left.y));
				ang.z = 0.0f;
			}
		}
		else // just calculate pitch/yaw
		{
			if (len > 0.0f)
			{
				ang.x = rad2deg(imports->atan2(-z, len));
				ang.y = rad2deg(imports->atan2(y, x));

				if (ang.x < 0.0f)
					ang.x += 360.0f;

				if (ang.y < 0.0f)
					ang.y += 360.0f;
			}
			else
			{
				ang.x = (z > 0.0f) ? 270.0f : 90.0f;
				ang.y = 0.0f;
			}
		
			ang.z = 0.0f;
		}
	}

	void vec3::vectorVectors (vec3 &r, vec3 &u) const
	{
		vec3 up_temp; // 0,0,0

		if (x != 0.0f && y != 0.0f)
		{
			r.cross(*this, up_temp);
			r.vectorNormalize();
			u.cross(r, *this);
			u.vectorNormalize();
		}
		else
		{
			r.init(0.0f, -1.0f, 0.0f);
			u.init(-z, 0.0f, 0.0f);
		}
	}
};