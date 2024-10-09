#pragma once

#include <vector>

class IndexBuffer 
{
public: 
	unsigned int mHandle; 
	unsigned int mCount; 
private: 
	IndexBuffer(const IndexBuffer& other); 
	IndexBuffer& operator=(const IndexBuffer& other); 
public: 
	IndexBuffer(); 
	~IndexBuffer();
	  
	void Set(unsigned int* rr, unsigned int len); 
	void Set(std::vector<unsigned int>& input); 
	unsigned int Count(); 
	unsigned int GetHandle();
};
