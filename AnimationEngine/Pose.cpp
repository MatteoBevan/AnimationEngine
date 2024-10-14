#include "Pose.h"

Pose::Pose()
{
}

Pose::Pose(const Pose& p)
{
	*this = p;
}

Pose::Pose(unsigned int numJoints)
{
	Resize(numJoints);
}

Pose& Pose::operator=(const Pose& p)
{
	if (&p == this) 
	{ 
		return *this; 
	} 
	
	if (mParents.size() != p.mParents.size()) 
	{ 
		mParents.resize(p.mParents.size()); 
	} 
	
	if (mJoints.size() != p.mJoints.size()) 
	{ 
		mJoints.resize(p.mJoints.size()); 
	} 
	
	if (mParents.size() != 0) 
	{ 
		memcpy(&mParents[0], &p.mParents[0], sizeof(int) * mParents.size()); 
	} 
	
	if (mJoints.size() != 0) 
	{
		memcpy(&mJoints[0], &p.mJoints[0], sizeof(Transform) * mJoints.size());
	}
	
	return *this;
}

void Pose::Resize(unsigned int size)
{
	mParents.resize(size);
	mJoints.resize(size);
}

unsigned int Pose::Size()
{
	return (unsigned int)mJoints.size();
}

int Pose::GetParent(unsigned int index)
{
	return mParents[index];
}

void Pose::SetParent(unsigned int index, int parent)
{
	mParents[index] = parent;
}

// TODO can this be a const&? can the others Transform returns?
Transform Pose::GetLocalTransform(unsigned int index)
{
	return mJoints[index];
}

void Pose::SetLocalTransform(unsigned int index, const Transform& transform)
{
	mJoints[index] = transform;
}

Transform Pose::GetGlobalTransform(unsigned int index)
{
	Transform result = mJoints[index];

	for (int p = mParents[index]; p >= 0; p = mParents[p]) 
	{ 
		result = combine(mJoints[p], result); 
	} 
	
	return result;
}

Transform Pose::operator[](unsigned int index)
{
	return GetGlobalTransform(index);
}

void Pose::GetMatrixPalette(std::vector<mat4>& out)
{
	unsigned int size = Size(); 
	
	if (out.size() != size) 
	{ 
		out.resize(size); 
	} 
	
	for (unsigned int i = 0; i < size; ++i) 
	{ 
		const Transform& t = GetGlobalTransform(i); 
		out[i] = transformToMat4(t); 
	}
}

bool Pose::operator==(const Pose& other)
{
	if (mJoints.size() != other.mJoints.size()) 
	{ 
		return false; 
	} 
	
	if (mParents.size() != other.mParents.size()) 
	{ 
		return false; 
	} 
	
	unsigned int size = (unsigned int)mJoints.size(); 
	
	for (unsigned int i = 0; i < size; ++i) 
	{
		const Transform& thisLocal = mJoints[i]; 
		const Transform& otherLocal = other.mJoints[i];
		
		int thisParent = mParents[i]; 
		int otherParent = other.mParents[i]; 
		
		if (thisParent != otherParent) 
		{ 
			return false; 
		} 
		
		if (thisLocal.position != otherLocal.position) 
		{ 
			return false; 
		} 
		
		if (thisLocal.rotation != otherLocal.rotation)
		{ 
			return false; 
		} 
		
		if (thisLocal.scale != otherLocal.scale)
		{ 
			return false; 
		}
	} 
	
	return true;
}

bool Pose::operator!=(const Pose& other)
{
	return !(*this == other);
}
