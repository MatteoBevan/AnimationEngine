#pragma once

#define VEC3_EPSILON 0.000001f

struct vec3
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		float v[3];
	};

	inline vec3() : x(0.0f), y(0.0f), z(0.0f) { } 
	inline vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { } 
	inline vec3(float* fv) : x(fv[0]), y(fv[1]), z(fv[2]) { }

	vec3 operator+(const vec3& r) const;
	vec3 operator-(const vec3& r) const;
	vec3 operator*(const float& f) const;
	vec3 operator*(const vec3& r) const;

	bool operator==(const vec3& r) const;
	bool operator!=(const vec3& r) const;

	float dot(const vec3& r) const;
	float lenSq() const;
	float len() const;
	void normalize();
	vec3 normalized() const;
	float angle(vec3& r) const;

	vec3 project(const vec3& b) const;
	vec3 reject(const vec3& b) const;
	vec3 reflect(const vec3& b) const;
	vec3 cross(const vec3& r) const;

	static vec3 lerp(const vec3& s, const vec3& e, float t);
	static vec3 slerp(const vec3& s, const vec3& e, float t);
	static vec3 nlerp(const vec3& s, const vec3& e, float t);

};