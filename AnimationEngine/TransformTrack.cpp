#include "TransformTrack.h"

TransformTrack::TransformTrack()
{
	mId = 0;
}

unsigned int TransformTrack::GetId()
{
	return mId;
}

void TransformTrack::SetId(unsigned int id)
{
	mId = id;
}

VectorTrack& TransformTrack::GetPositionTrack()
{
	return mPosition;
}

QuaternionTrack& TransformTrack::GetRotationTrack()
{
	return mRotation;
}

VectorTrack& TransformTrack::GetScaleTrack()
{
	return mScale;
}

float TransformTrack::GetStartTime()
{
	float result = std::numeric_limits<float>::max();

	if (mPosition.Size() > 1)
	{
		result = mPosition.GetStartTime();
	}

	if (mRotation.Size() > 1)
	{
		float rotationStart = mRotation.GetStartTime();

		if (rotationStart < result)
		{
			result = rotationStart;
		}
	}

	if (mScale.Size() > 1)
	{
		float scaleStart = mScale.GetStartTime();

		if (scaleStart < result)
		{
			result = scaleStart;
		}
	}

	return result;
}

float TransformTrack::GetEndTime()
{
	float result = std::numeric_limits<float>::min();

	if (mPosition.Size() > 1)
	{
		result = mPosition.GetStartTime();
	}

	if (mRotation.Size() > 1)
	{
		float rotationStart = mRotation.GetStartTime();

		if (rotationStart > result)
		{
			result = rotationStart;
		}
	}

	if (mScale.Size() > 1)
	{
		float scaleStart = mScale.GetStartTime();

		if (scaleStart > result)
		{
			result = scaleStart;
		}
	}

	return result;
}

bool TransformTrack::IsValid()
{
	return (mPosition.Size() > 1 || mRotation.Size() > 1 || mScale.Size() > 1);
}

Transform TransformTrack::Sample(const Transform& ref, float time, bool looping)
{
	Transform result = ref; // Assign default values 
	
	if (mPosition.Size() > 1) 
	{ // Only if valid 
		result.position = mPosition.Sample(time, looping); 
	} 
	
	if (mRotation.Size() > 1) 
	{ // Only if valid
		result.rotation = mRotation.Sample(time, looping);
	} 
	
	if (mScale.Size() > 1) 
	{ // Only if valid 
		result.scale = mScale.Sample(time, looping); 
	} 

	return result; 
}

