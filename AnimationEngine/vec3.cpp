#include "vec3.h"
#include <math.h>

vec3 operator+(const vec3& v, const vec3& r)
{
	return vec3(v.x + r.x, v.y + r.y, v.z + r.z);
}

vec3 operator-(const vec3& v, const vec3& r)
{
	return vec3(v.x - r.x, v.y - r.y, v.z - r.z);
}

vec3 operator*(const vec3& v, const float& f)
{
	return vec3(v.x * f, v.y * f, v.z * f);
}

vec3 operator*(const vec3& v, const vec3& r)
{
	return vec3(v.x * r.x, v.y * r.y, v.z * r.z);
}

bool operator==(const vec3& v, const vec3& r)
{ 
	vec3 diff(v - r); 
	return lenSq(diff) < VEC3_EPSILON;
} 

bool operator!=(const vec3& v, const vec3& r)
{ 
	return !(v == r);
}

float dot(const vec3& v, const vec3& r)
{
	return v.x * r.x + v.y * r.y + v.z * r.z;
}

float lenSq(const vec3& v)
{ 
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

float len(const vec3& v)
{ 
	float lenSqRes = lenSq(v);
	
	if (lenSqRes < VEC3_EPSILON)
	{ 
		return 0.0f; 
	} 
	
	return sqrtf(lenSqRes);
}

void normalize(vec3& v)
{
	float lenSqRes = lenSq(v);
	
	if (lenSqRes < VEC3_EPSILON)
	{ 
		return; 
	} 
	
	float invLen = 1.0f / sqrtf(lenSqRes);
	v.x *= invLen;
	v.y *= invLen;
	v.z *= invLen;
}

vec3 normalized(const vec3& v)
{ 
	float lenSqRes = lenSq(v);
	
	if (lenSqRes < VEC3_EPSILON)
	{ 
		return v; 
	} 
	
	float invLen = 1.0f / sqrtf(lenSqRes);
	
	return vec3(v.x * invLen, v.y * invLen, v.z * invLen);
}

float angle(const vec3& v, vec3& r)
{ 
	float sqMagL = lenSq(v); 
	float sqMagR = lenSq(r); 
	
	if (sqMagL < VEC3_EPSILON || sqMagR < VEC3_EPSILON) 
	{ 
		return 0.0f; 
	} 
	
	float dotRes = dot(v, r);
	
	float len = sqrtf(sqMagL) * sqrtf(sqMagR); 
	
	/*
	The acosf function returns angles in radians. To convert radians to degrees, multiply by 57.2958f. To convert degrees to radians, multiply by 0.0174533f.
	*/

	return acosf(dotRes / len);
}

vec3 project(const vec3& v, const vec3& b)
{ 
	float magBSq = len(b); 
	
	if (magBSq < VEC3_EPSILON) 
	{ 
		return vec3(); 
	} 
	
	float scale = dot(v, b) / magBSq; 
	return b * scale; 
}

vec3 reject(const vec3& v, const vec3& b)
{
	vec3 projection = project(v, b); 
	return v - projection;
}

vec3 reflect(const vec3& v, const vec3& b)
{
	float magBSq = len(b); 
	
	if (magBSq < VEC3_EPSILON) 
	{ 
		return vec3(); 
	} 
	
	float scale = dot(v,b) / magBSq; 
	vec3 proj2 = b * (scale * 2); 
	
	return v - proj2;
}

vec3 cross(const vec3& v, const vec3& r)
{ 
	return vec3(v.y * r.z - v.z * r.y, v.z * r.x - v.x * r.z, v.x * r.y - v.y * r.x);
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
	
	vec3 from = normalized(s); 
	vec3 to = normalized(e); 
	float theta = angle(from, to);
	float sin_theta = sinf(theta); 
	float a = sinf((1.0f - t) * theta) / sin_theta; 
	float b = sinf(t * theta) / sin_theta;
	
	return from * a + to * b;
}

vec3 nlerp(const vec3& s, const vec3& e, float t) 
{
	vec3 linear(s.x + (e.x - s.x) * t,	s.y + (e.y - s.y) * t, s.z + (e.z - s.z) * t); 
	return normalized(linear);
}
