#pragma once

template<unsigned int N> 
class Frame 
{ 
public: 
	float mValue[N]; 
	float mIn[N]; 
	float mOut[N]; 
	float mTime; 
};

// TODO a potential memory optimisation is to have a variable amount of scalar values per frame
// here for example every vector will use 3 scalars no matter what, when it could have one scalar per value actually modified by the animation (so up to 3)
typedef Frame<1> ScalarFrame; 
typedef Frame<3> VectorFrame; 
typedef Frame<4> QuaternionFrame;
