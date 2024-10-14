#pragma once

#ifndef _H_SAMPLE_
#define _H_SAMPLE_

#include "Application.h"
#include "DebugDraw.h"
#include <vector>
#include "Pose.h"
#include "Clip.h"

class Sample : public Application {
protected:
	Pose mRestPose;
	Pose mCurrentPose;
	Pose mBindPose;
	std::vector<Clip> mClips;
	unsigned int mCurrentClip;
	float mPlaybackTime;
	DebugDraw* mRestPoseVisual;
	DebugDraw* mBindPoseVisual;
	DebugDraw* mCurrentPoseVisual;
public:
	void Initialize();
	void Update(float deltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif