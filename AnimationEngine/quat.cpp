#include "quat.h" 
#include <cmath> 

quat angleAxis(float angle, const vec3& axis) 
{ 
	vec3 norm = normalized(axis); 
	float s = sinf(angle * 0.5f); 
	return quat(norm.x * s, norm.y * s, norm.z * s, cosf(angle * 0.5f) ); 
}

quat fromTo(const vec3& from, const vec3& to) 
{
	vec3 f = normalized(from); 
	vec3 t = normalized(to); 
	
	if (f == t) 
	{ 
		return quat(); 
	}
	else if (f == t * -1.0f) 
	{ 
		vec3 ortho = vec3(1, 0, 0); 
		
		if (fabsf(f.y) < fabsf(f.x)) 
		{ 
			ortho = vec3(0, 1, 0); 
		} 
		
		if (fabsf(f.z) < fabs(f.y) && fabs(f.z) < fabsf(f.x)) 
		{ 
			ortho = vec3(0, 0, 1); 
		} 
		
		vec3 axis = normalized(cross(f, ortho)); 
		
		return quat(axis.x, axis.y, axis.z, 0); 
	}
	
	vec3 half = normalized(f + t); 
	vec3 axis = cross(f, half); 
	
	return quat(axis.x, axis.y, axis.z, dot(f, half));
}

vec3 getAxis(const quat& quat) 
{ 
	return normalized(vec3(quat.x, quat.y, quat.z)); 
} 

float getAngle(const quat& quat) 
{ 
	return 2.0f * acosf(quat.w); 
}

quat operator+(const quat& a, const quat& b) 
{
	return quat(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
} 

quat operator-(const quat& a, const quat& b) 
{ 
	return quat(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); 
} 

quat operator*(const quat& a, float b) 
{ 
	return quat(a.x * b, a.y * b, a.z * b, a.w * b); 
} 

quat operator-(const quat& q) 
{ 
	return quat(-q.x, -q.y, -q.z, -q.w); 
}

bool operator==(const quat& left, const quat& right) 
{
	return (fabsf(left.x - right.x) <= QUAT_EPSILON && fabsf(left.y - right.y) <= QUAT_EPSILON &&
		fabsf(left.z - right.z) <= QUAT_EPSILON && fabsf(left.w - right.w) <= QUAT_EPSILON);
} 

bool operator!=(const quat& a, const quat& b) 
{ 
	return !(a == b); 
}

bool sameOrientation(const quat& l, const quat& r) 
{
	return (
		fabsf(l.x - r.x) <= QUAT_EPSILON && fabsf(l.y - r.y) <= QUAT_EPSILON && fabsf(l.z - r.z) <= QUAT_EPSILON && fabsf(l.w - r.w) <= QUAT_EPSILON) || 
		(fabsf(l.x + r.x) <= QUAT_EPSILON && fabsf(l.y + r.y) <= QUAT_EPSILON && fabsf(l.z + r.z) <= QUAT_EPSILON && fabsf(l.w + r.w) <= QUAT_EPSILON);
}

float dot(const quat& a, const quat& b) 
{ 
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; 
}

float lenSq(const quat& q) 
{ 
	return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w; 
}

float len(const quat& q) 
{ 
	float lenSquared = lenSq(q);
	if (lenSquared < QUAT_EPSILON)
	{ 
		return 0.0f; 
	} 
	
	return sqrtf(lenSquared);
}

void normalize(quat& q) 
{
	float lenSquared = lenSq(q);
	if (lenSquared < QUAT_EPSILON)
	{ 
		return; 
	} 
	float i_len = 1.0f / sqrtf(lenSquared);
	q.x *= i_len; 
	q.y *= i_len; 
	q.z *= i_len; 
	q.w *= i_len; 
}

quat normalized(const quat& q) 
{
	float lenSquared = lenSq(q);

	if (lenSquared < QUAT_EPSILON) 
	{ 
		return quat(); 
	} 
	
	float il = 1.0f / sqrtf(lenSquared); // il: inverse length 
	return quat(q.x * il, q.y * il, q.z * il,q.w * il); 
}

quat conjugate(const quat& q) 
{ 
	return quat(-q.x, -q.y, -q.z, q.w); 
}

quat inverse(const quat& q) 
{
	float lenSquared = lenSq(q); 
	
	if (lenSquared < QUAT_EPSILON)
	{ 
		return quat(); 
	} 
	
	float recip = 1.0f / lenSquared;
	
	return quat(-q.x * recip, -q.y * recip, -q.z * recip, q.w * recip);
}

quat operator*(const quat& Q1, const quat& Q2) 
{ 
	return quat(
		Q2.x * Q1.w + Q2.y * Q1.z - Q2.z * Q1.y + Q2.w * Q1.x, 
		-Q2.x * Q1.z + Q2.y * Q1.w + Q2.z * Q1.x + Q2.w * Q1.y, 
		Q2.x * Q1.y - Q2.y * Q1.x + Q2.z * Q1.w + Q2.w * Q1.z, 
		-Q2.x * Q1.x - Q2.y * Q1.y - Q2.z * Q1.z + Q2.w * Q1.w); 
}

// Multiplying a vector by a quaternion will always yield a vector that is rotated by the quaternion.
vec3 operator*(const quat& q, const vec3& v) 
{ 
	return q.vector * 2.0f * dot(q.vector, v) + 
		v * (q.scalar * q.scalar - dot(q.vector, q.vector)) + 
		cross(q.vector, v) * 2.0f * q.scalar; 
}

quat mix(const quat& from, const quat& to, float t) 
{ 
	return from * (1.0f - t) + to * t; 
}

quat nlerp(const quat& from, const quat& to, float t) 
{ 
	return normalized(from + (to - from) * t); 
}

quat operator^(const quat& q, float f) 
{
	float angle = 2.0f * acosf(q.scalar); 
	vec3 axis = normalized(q.vector);
	
	float halfCos = cosf(f * angle * 0.5f); 
	float halfSin = sinf(f * angle * 0.5f); 
	
	return quat(axis.x * halfSin, axis.y * halfSin, axis.z * halfSin, halfCos);
}


// slerp should only be used if consistent velocity is required.In most cases, nlerp will be a better interpolation method.
quat slerp(const quat& start, const quat& end, float t)
{
	if (fabsf(dot(start, end)) > 1.0f - QUAT_EPSILON)
	{
		return nlerp(start, end, t);
	}

	quat delta = inverse(start) * end;

	return normalized((delta ^ t) * start);
}

quat lookRotation(const vec3& direction, const vec3& up) 
{ 
	// Find orthonormal basis vectors 
	vec3 f = normalized(direction); // Object Forward 
	vec3 u = normalized(up); // Desired Up 
	vec3 r = cross(u, f); // Object Right 
	u = cross(f, r); // Object Up 
	
	// From world forward to object forward 
	quat worldToObject = fromTo(vec3(0, 0, 1), f);
	// what direction is the new object up? 
	vec3 objectUp = worldToObject * vec3(0, 1, 0); 
	// From object up to desired up 
	quat u2u = fromTo(objectUp, u); 
	// Rotate to forward direction first 
	// then twist to correct up 
	quat result = worldToObject * u2u; 
	
	// Don't forget to normalize the result 
	return normalized(result); 
}

mat4 quatToMat4(const quat& q) 
{ 
	vec3 r = q * vec3(1, 0, 0); 
	vec3 u = q * vec3(0, 1, 0); 
	vec3 f = q * vec3(0, 0, 1); 
	
	return mat4(r.x, r.y, r.z, 0, u.x, u.y, u.z, 0, f.x, f.y, f.z, 0, 0, 0, 0, 1); 
}

quat mat4ToQuat(const mat4& m) 
{ 
	vec3 up = normalized(vec3(m.up.x, m.up.y, m.up.z)); 
	vec3 forward = normalized(vec3(m.forward.x, m.forward.y, m.forward.z)); 

	vec3 right = cross(up, forward); 
	up = cross(forward, right); 

	return lookRotation(forward, up); 
}





