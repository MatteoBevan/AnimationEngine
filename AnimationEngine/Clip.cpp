#include "Clip.h"

float Clip::AdjustTimeToFitRange(float time)
{
	float duration = mEndTime - mStartTime;

	if (duration <= 0.0f)
	{
		return 0.0f;
	}

	if (mLooping)
	{
		time = fmodf(time - mStartTime, mEndTime - mStartTime);

		if (time < 0.0f)
		{
			time += duration;
		}

		time = time + mStartTime;
	}
	else
	{
		if (time <= mStartTime)
		{
			time = mStartTime;
		}

		if (time >= mEndTime)
		{
			time = mEndTime;
		}
	}

	return time;

}

Clip::Clip()
{
	mName = "No name given"; 
	mStartTime = 0.0f; 
	mEndTime = 0.0f; 
	mLooping = true;
}

unsigned int Clip::GetIdAtIndex(unsigned int index)
{
	return mTracks[index].GetId();
}

void Clip::SetIdAtIndex(unsigned int idx, unsigned int id)
{

}

unsigned int Clip::Size()
{
	return (unsigned int)mTracks.size();
}

float Clip::Sample(Pose& outPose, float inTime)
{
	if (GetDuration() == 0.0f) 
	{ 
		return 0.0f; 
	} 
	
	inTime = AdjustTimeToFitRange(inTime); 
	
	unsigned int size = (unsigned int)mTracks.size();
	
	for (unsigned int i = 0; i < size; ++i) 
	{
		unsigned int j = mTracks[i].GetId(); // Joint 
		
		const Transform& local = outPose.GetLocalTransform(j);
		const Transform& animated = mTracks[i].Sample( local, inTime, mLooping);
		
		outPose.SetLocalTransform(j, animated); 
	} 
	
	return inTime;
}

TransformTrack& Clip::operator[](unsigned int joint)
{
	for (int i = 0, s = (unsigned int)mTracks.size(); i < s; ++i)
	{
		if (mTracks[i].GetId() == joint)
		{
			return mTracks[i];
		}
	}

	mTracks.push_back(TransformTrack()); 
	mTracks[mTracks.size() - 1].SetId(joint); 
	
	return mTracks[mTracks.size() - 1];
}

void Clip::RecalculateDuration()
{
	mStartTime = 0.0f; 
	mEndTime = 0.0f; 
	
	bool startSet = false; 
	bool endSet = false; 
	unsigned int tracksSize = (unsigned int)mTracks.size();
	
	for (unsigned int i = 0; i < tracksSize; ++i) 
	{
		if (mTracks[i].IsValid()) 
		{
			float startTime = mTracks[i].GetStartTime(); 
			float endTime = mTracks[i].GetEndTime(); 
			
			if (startTime < mStartTime || !startSet) 
			{
				mStartTime = startTime; 
				startSet = true;
				
			} 
			
			if (endTime > mEndTime || !endSet) 
			{ 
				mEndTime = endTime; 
				endSet = true; 
			}
		}
	}
}

std::string& Clip::GetName()
{
	return mName;
}

void Clip::SetName(const std::string& inNewName)
{
	mName = inNewName;
}

float Clip::GetDuration()
{
	return mEndTime - mStartTime;
}

float Clip::GetStartTime()
{
	return mStartTime;
}

float Clip::GetEndTime()
{
	return mEndTime;
}

bool Clip::GetLooping()
{
	return mLooping;
}

void Clip::SetLooping(bool inLooping)
{
	mLooping = inLooping;
}
