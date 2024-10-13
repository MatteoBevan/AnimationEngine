#include "Track.h"
#include "vec3.h"
#include "quat.h"

template Track<float, 1>; 
template Track<vec3, 3>; 
template Track<quat, 4>;

namespace TrackHelpers
{
	inline float Interpolate(float a, float b, float t)
	{
		return a + (b - a) * t;
	}

	inline vec3 Interpolate(const vec3& a, const vec3& b, float t)
	{
		return lerp(a, b, t);
	}

	inline quat Interpolate(const quat& a, const quat& b, float t)
	{
		quat result = mix(a, b, t);

		// Neighborhood
		if (dot(a, b) < 0)
		{
			result = mix(a, -b, t);
		}
		return normalized(result); //NLerp, not slerp 
	}

	// TODO see below
	inline float AdjustHermiteResult(float f)
	{
		return f;
	}

	// TODO instead of having AdjustHermiteResult just to normalize the quaternion we could re-design this
	inline vec3 AdjustHermiteResult(const vec3& v)
	{
		return v;
	}

	inline quat AdjustHermiteResult(const quat& q)
	{
		return normalized(q);
	}

	// TODO
	inline void Neighborhood(const float& a, float& b) {}

	// TODO 
	inline void Neighborhood(const vec3& a, vec3& b) {}

	inline void Neighborhood(const quat& a, quat& b)
	{
		if (dot(a, b) < 0)
		{
			b = -b;
		}
	}
};

template<> 
float Track<float, 1>::Cast(float* value) 
{ 
	return value[0]; 
} 

template<> 
vec3 Track<vec3, 3>::Cast(float* value) 
{ 
	return vec3(value[0], value[1], value[2]); 
} 

template<> quat Track<quat, 4>::Cast(float* value)
{
	quat r = quat(value[0], value[1], value[2], value[3]);

	return normalized(r);
}


template<typename T, int N>
inline T Track<T, N>::SampleLinear(float time, bool looping)
{
	int thisFrame = FrameIndex(time, looping);

	if (thisFrame < 0 || thisFrame >= mFrames.size() - 1)
	{
		return T();
	}

	int nextFrame = thisFrame + 1;

	// TODO FrameIndex and AdjustTimeToFitTrack have similar operations and they run together, can we pass in the adjusted time to FrameIndex to avoid one?
	float trackTime = AdjustTimeToFitTrack(time, looping);
	float thisTime = mFrames[thisFrame].mTime;
	float frameDelta = mFrames[nextFrame].mTime - thisTime;

	if (frameDelta <= 0.0f)
	{
		return T();
	}

	float t = (trackTime - thisTime) / frameDelta;

	T start = Cast(&mFrames[thisFrame].mValue[0]);
	T end = Cast(&mFrames[nextFrame].mValue[0]);

	return TrackHelpers::Interpolate(start, end, t);
}


template<typename T, int N>
inline T Track<T, N>::Hermite(float t, const T& p1, const T& s1, const T& _p2, const T& s2)
{
	float tt = t * t;
	float ttt = tt * t;

	T p2 = _p2;
	TrackHelpers::Neighborhood(p1, p2);

	float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
	float h2 = -2.0f * ttt + 3.0f * tt;
	float h3 = ttt - 2.0f * tt + t;
	float h4 = ttt - tt;

	T result = p1 * h1 + p2 * h2 + s1 * h3 + s2 * h4;

	return TrackHelpers::AdjustHermiteResult(result);
}