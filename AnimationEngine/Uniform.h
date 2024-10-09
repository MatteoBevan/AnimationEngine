#pragma once

#include <vector>

template <typename T> 
class Uniform 
{
private: 
	Uniform(); 
	Uniform(const Uniform&); 
	Uniform& operator=(const Uniform&); 
	~Uniform();

public: 
	static void Set(unsigned int slot, const T& value); 
	static void Set(unsigned int slot, T* arr, unsigned int len); 
	static void Set(unsigned int slot, std::vector<T>& arr);
};

