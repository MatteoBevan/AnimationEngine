#include "vec3.h"
#include <math.h>

vec3 vec3::operator+(const vec3& r) const
{
	return vec3(x + r.x, y + r.y, z + r.z);
}

vec3 vec3::operator-(const vec3& r) const
{
	return vec3(x - r.x, y - r.y, z - r.z);
}

vec3 vec3::operator*(const float& f) const
{
	return vec3(x * f, y * f, z * f);
}

vec3 vec3::operator*(const vec3& r) const
{
	return vec3(x * r.x, y * r.y, z * r.z);
}

bool vec3::operator==(const vec3& r) const
{ 
	vec3 diff(*this - r); 
	return diff.lenSq() < VEC3_EPSILON; 
} 

bool vec3::operator!=(const vec3& r) const
{ 
	return !(*this == r);
}

float vec3::dot(const vec3& r) const
{
	return x * r.x + y * r.y + z * r.z; 
}

float vec3::lenSq() const
{ 
	return x * x + y * y + z * z; 
}

float vec3::len() const
{ 
	float lenSqRes = lenSq();
	
	if (lenSqRes < VEC3_EPSILON)
	{ 
		return 0.0f; 
	} 
	
	return sqrtf(lenSqRes);
}

void vec3::normalize()
{
	float lenSqRes = lenSq();
	
	if (lenSqRes < VEC3_EPSILON)
	{ 
		return; 
	} 
	
	float invLen = 1.0f / sqrtf(lenSqRes);
	x *= invLen; 
	y *= invLen; 
	z *= invLen;
}

vec3 vec3::normalized() const
{ 
	float lenSqRes = lenSq();
	
	if (lenSqRes < VEC3_EPSILON)
	{ 
		return *this; 
	} 
	
	float invLen = 1.0f / sqrtf(lenSqRes);
	
	return vec3(x * invLen, y * invLen, z * invLen); 
}

float vec3::angle(vec3& r) const
{ 
	float sqMagL = lenSq(); 
	float sqMagR = r.lenSq(); 
	
	if (sqMagL < VEC3_EPSILON || sqMagR < VEC3_EPSILON) 
	{ 
		return 0.0f; 
	} 
	
	float dotRes = dot(r);
	
	float len = sqrtf(sqMagL) * sqrtf(sqMagR); 
	
	/*
	The acosf function returns angles in radians. To convert radians to degrees, multiply by 57.2958f. To convert degrees to radians, multiply by 0.0174533f.
	*/

	return acosf(dotRes / len);
}

vec3 vec3::project(const vec3& b) const
{ 
	float magBSq = b.len(); 
	
	if (magBSq < VEC3_EPSILON) 
	{ 
		return vec3(); 
	} 
	
	float scale = dot(b) / magBSq; 
	return b * scale; 
}

vec3 vec3::reject(const vec3& b) const
{
	vec3 projection = project(b); 
	return *this - projection;
}

vec3 vec3::reflect(const vec3& b) const
{
	float magBSq = b.len(); 
	
	if (magBSq < VEC3_EPSILON) 
	{ 
		return vec3(); 
	} 
	
	float scale = dot(b) / magBSq; 
	vec3 proj2 = b * (scale * 2); 
	
	return *this - proj2;
}

vec3 vec3::cross(const vec3& r) const
{ 
	return vec3(y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x); 
}

vec3 lerp(const vec3& s, const vec3& e, float t) 
{
	return vec3(s.x + (e.x - s.x) * t, s.y + (e.y - s.y) * t, s.z + (e.z - s.z) * t);
}

vec3 slerp(const vec3& s, const vec3& e, float t) 
{
	if (t < 0.01f) 
	{ 
		return lerp(s, e, t); 
	} 
	
	vec3 from = s.normalized(); 
	vec3 to = e.normalized(); 
	float theta = from.angle(to);
	float sin_theta = sinf(theta); 
	float a = sinf((1.0f - t) * theta) / sin_theta; 
	float b = sinf(t * theta) / sin_theta;
	
	return from * a + to * b;
}

vec3 nlerp(const vec3& s, const vec3& e, float t) 
{
	vec3 linear(s.x + (e.x - s.x) * t,	s.y + (e.y - s.y) * t, s.z + (e.z - s.z) * t); 
	return linear.normalized();
}
