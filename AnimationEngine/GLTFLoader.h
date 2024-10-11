#pragma once

#include "cgltf.h" 

cgltf_data* LoadGLTFFile(const char* path); 

void FreeGLTFFile(cgltf_data* handle); 
