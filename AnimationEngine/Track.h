#pragma once
#include "quat.h"
#include "vec3.h"
#include "Interpolation.h"
#include "Frame.h"
#include <vector>
#include <cassert>

template<typename T, int N> 
class Track 
{
protected: 
	std::vector<Frame<N>> mFrames; 
	Interpolation mInterpolation;
public: 
	Track();
	void Resize(unsigned int size); 
	unsigned int Size() const;
	Interpolation GetInterpolation() const;
	void SetInterpolation(Interpolation interp); 
	float GetStartTime() const;
	float GetEndTime() const;
	T Sample(float time, bool looping); 
	Frame<N>& operator[](unsigned int index);

protected: 
	T SampleConstant(float time, bool looping); 
	T SampleLinear(float time, bool looping); 
	T SampleCubic(float time, bool looping);
	T Hermite(float time, const T& p1, const T& s1, const T& p2, const T& s2);
	int FrameIndex(float time, bool looping); 
	float AdjustTimeToFitTrack(float t, bool loop);

	// TODO having a frame per scalar value of the curve would mean we could keep a float, vec3, quat and so on cached in the track to update
	// instead of casting and creating a new one every frame
	T Cast(float* value); 
};

typedef Track<float, 1> ScalarTrack; 
typedef Track<vec3, 3> VectorTrack; 
typedef Track<quat, 4> QuaternionTrack;

template<typename T, int N> 
Track<T, N>::Track() 
{ 
	mInterpolation = Interpolation::Linear; 
}

template<typename T, int N>
inline void Track<T, N>::Resize(unsigned int size)
{
	mFrames.resize(size);
}

template<typename T, int N>
inline unsigned int Track<T, N>::Size()  const
{
	return (unsigned int)mFrames.size();
}

template<typename T, int N>
inline Interpolation Track<T, N>::GetInterpolation() const
{
	return mInterpolation;
}

template<typename T, int N>
inline void Track<T, N>::SetInterpolation(Interpolation interp)
{
	mInterpolation = interp;
}

template<typename T, int N> 
float Track<T, N>::GetStartTime()  const
{
	return mFrames[0].mTime;
} 

template<typename T, int N> 
float Track<T, N>::GetEndTime()  const
{ 
	return mFrames[mFrames.size() - 1].mTime; 
}

template<typename T, int N> 
T Track<T, N>::Sample(float time, bool looping)
{
	switch (mInterpolation)
	{
	case Interpolation::Constant:
		return SampleConstant(time, looping);
	case Interpolation::Linear:
		return SampleLinear(time, looping);
	case Interpolation::Cubic:
		return SampleCubic(time, looping);
	default:
		break;
	}

	assert(false);

	return T();
}

template<typename T, int N> 
Frame<N>& Track<T, N>::operator[](unsigned int index) 
{ 
	return mFrames[index]; 
}

template<typename T, int N>
inline T Track<T, N>::SampleConstant(float time, bool looping)
{
	int frame = FrameIndex(time, looping);
	
	if (frame < 0 || frame >= (int)mFrames.size()) 
	{ 
		return T(); 
	} 
	
	return Cast(&mFrames[frame].mValue[0]);
}

template<typename T, int N>
inline T Track<T, N>::SampleCubic(float time, bool looping)
{
	int thisFrame = FrameIndex(time, looping);

	if (thisFrame < 0 || thisFrame >= mFrames.size() - 1) 
	{ 
		return T(); 
	} 
	
	int nextFrame = thisFrame + 1; 
	
	float trackTime = AdjustTimeToFitTrack(time, looping); 
	float thisTime = mFrames[thisFrame].mTime; 
	float frameDelta = mFrames[nextFrame].mTime - thisTime; 
	
	if (frameDelta <= 0.0f) 
	{ 
		return T(); 
	} 
	
	float t = (trackTime - thisTime) / frameDelta; 
	size_t fltSize = sizeof(float); 
	
	T point1 = Cast(&mFrames[thisFrame].mValue[0]);

	// slopes will use memcopy to copy the values to avoid the normalization in the Cast for the quaternion case
	// TODO can we avoid this moving to a per scalar curve approach?
	T slope1;// = mFrames[thisFrame].mOut * frameDelta; 
	memcpy(&slope1, mFrames[thisFrame].mOut, N * fltSize); 
	slope1 = slope1 * frameDelta; 
	
	T point2 = Cast(&mFrames[nextFrame].mValue[0]); 
	T slope2;// = mFrames[nextFrame].mIn[0] * frameDelta; 
	memcpy(&slope2, mFrames[nextFrame].mIn, N * fltSize); 
	slope2 = slope2 * frameDelta; 
	
	return Hermite(t, point1, slope1, point2, slope2);
}

template<typename T, int N>
inline int Track<T, N>::FrameIndex(float time, bool looping)
{
	// TODO this whole block till the for seems to be the same as AdjustTimeToFitTrack, can we just call that instead?
	unsigned int size = Size(); 
	
	if (size <= 1) 
	{ 
		return -1; 
	}

	if (looping)
	{
		float startTime = mFrames[0].mTime;
		float endTime = mFrames[size - 1].mTime;
		float duration = endTime - startTime;

		time = fmodf(time - startTime, endTime - startTime);

		if (time < 0.0f)
		{
			time += duration;
		}

		time = time + startTime;
	}
	else 
	{ 
		if (time <= mFrames[0].mTime) 
		{ 
			return 0; 
		} 
		
		if (time >= mFrames[size - 2].mTime) 
		{ 
			return (int)size - 2; 
		}
	}

	// TODO here we want the first frame that is under our target time, is there a better way to find it?
	for (int i = (int)size - 1; i >= 0; --i) 
	{ 
		if (time >= mFrames[i].mTime) 
		{ 
			return i; 
		} 
	} 
	
	assert(false);
	
	return 0;
}

template<typename T, int N>
inline float Track<T, N>::AdjustTimeToFitTrack(float time, bool looping)
{
	unsigned int size = Size();

	if (size <= 1)
	{
		return -1;
	}

	float startTime = mFrames[0].mTime;
	float endTime = mFrames[size - 1].mTime; 
	float duration = endTime - startTime; 
	
	if (duration <= 0.0f) 
	{ 
		return 0.0f; 
	}

	if (looping) 
	{ 
		time = fmodf(time - startTime, endTime - startTime); 
		
		if (time < 0.0f) 
		{ 
			time += duration;
		}

		time = time + startTime; 
	}
	else 
	{ 
		if (time <= mFrames[0].mTime) 
		{ 
			time = startTime; 
		} 
		
		if (time >= mFrames[size - 1].mTime) 
		{ 
			time = endTime; 
		} 
	} 
	
	return time;
}
