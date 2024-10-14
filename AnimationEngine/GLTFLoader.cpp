#include "GLTFLoader.h"
#include <iostream>
#include "Transform.h"
#include "Pose.h"
#include <vector>
#include "Track.h"
#include "Clip.h"
#include "Skeleton.h"

cgltf_data* LoadGLTFFile(const char* path)
{
	cgltf_options options; 
	
	memset(&options, 0, sizeof(cgltf_options)); 
	
	cgltf_data* data = NULL; 
	cgltf_result result = cgltf_parse_file(&options, path, &data); 
	
	char resolved_path[500];
	_fullpath(resolved_path, path, 500);
	printf("\n Loading: %s\n", resolved_path);

	if (result != cgltf_result_success) 
	{ 
		std::cout << "Could not load: " << path << "\n"; 
		return 0; 
	} 
	
	result = cgltf_load_buffers(&options, data, path);
	
	if (result != cgltf_result_success) 
	{ 
		cgltf_free(data); 
		std::cout << "Could not load: " << path << "\n"; 
		return 0; 
	} 
	
	result = cgltf_validate(data); 
	
	if (result != cgltf_result_success) 
	{ 
		cgltf_free(data); 
		std::cout << "Invalid file: " << path << "\n"; 
		return 0; 
	} 
	
	return data;
}

void FreeGLTFFile(cgltf_data* handle)
{
	if (handle == 0)
	{ 
		std::cout << "WARNING: Can't free null data\n"; 
	}
	else 
	{
		cgltf_free(handle);
	}
}

std::vector<std::string> LoadJointNames(cgltf_data* data)
{
	unsigned int boneCount = (unsigned int)data->nodes_count;
	std::vector<std::string> result(boneCount, "Not Set");

	for (unsigned int i = 0; i < boneCount; ++i)
	{
		cgltf_node* node = &(data->nodes[i]);
		if (node->name == 0)
		{
			result[i] = "EMPTY NODE";
		}
		else
		{
			result[i] = node->name;
		}
	}

	return result;
}

namespace GLTFHelpers 
{
	Transform GetLocalTransform(cgltf_node& n) 
	{
		Transform result; 
		
		if (n.has_matrix) 
		{ 
			mat4 mat(&n.matrix[0]); 
			result = mat4ToTransform(mat); 
		} 
		
		if (n.has_translation) 
		{ 
			result.position = vec3(n.translation[0], n.translation[1], n.translation[2]); 
		} 
		
		if (n.has_rotation) 
		{
			result.rotation = quat(n.rotation[0], n.rotation[1], n.rotation[2], n.rotation[3]);
		} 
		
		if (n.has_scale) 
		{ 
			result.scale = vec3(n.scale[0], n.scale[1], n.scale[2]); 
		} 
		
		return result;
	}

	int GetNodeIndex(cgltf_node* target, cgltf_node* allNodes, unsigned int numNodes) 
	{ 
		if (target == 0) 
		{ 
			return -1; 
		} 
		
		for (unsigned int i = 0; i < numNodes; ++i) 
		{ 
			if (target == &allNodes[i]) 
			{ 
				return (int)i; 
			} 
		} 
		
		return -1; 
	}

	void GetScalarValues(std::vector<float>& out, unsigned int compCount, const cgltf_accessor& inAccessor)
	{
		out.resize(inAccessor.count * compCount);

		for (cgltf_size i = 0; i < inAccessor.count; ++i)
		{
			cgltf_accessor_read_float(&inAccessor, i, &out[i * compCount], compCount);
		}
	}

	template<typename T, int N>
	void TrackFromChannel(Track<T, N>& inOutTrack, const cgltf_animation_channel& inChannel) 
	{
		cgltf_animation_sampler& sampler = *inChannel.sampler;

		Interpolation interpolation = Interpolation::Constant;
		if (inChannel.sampler->interpolation == cgltf_interpolation_type_linear) 
		{
			interpolation = Interpolation::Linear;
		}
		else if (inChannel.sampler->interpolation == cgltf_interpolation_type_cubic_spline) 
		{
			interpolation = Interpolation::Cubic;
		}
	
		bool isSamplerCubic = interpolation == Interpolation::Cubic;
		inOutTrack.SetInterpolation(interpolation);

		std::vector<float> timelineFloats;
		GetScalarValues(timelineFloats, 1, *sampler.input);

		std::vector<float> valueFloats;
		GetScalarValues(valueFloats, N, *sampler.output);

		unsigned int numFrames = (unsigned int)sampler.input->count;
		unsigned int numberOfValuesPerFrame = (unsigned int)(valueFloats.size() / timelineFloats.size());
		inOutTrack.Resize(numFrames);
		for (unsigned int i = 0; i < numFrames; ++i) 
		{
			int baseIndex = i * numberOfValuesPerFrame;
			Frame<N>& frame = inOutTrack[i];
			int offset = 0;

			frame.mTime = timelineFloats[i];

			for (int component = 0; component < N; ++component) 
			{
				frame.mIn[component] = isSamplerCubic ? valueFloats[baseIndex + offset++] : 0.0f;
			}

			for (int component = 0; component < N; ++component) 
			{
				frame.mValue[component] = valueFloats[baseIndex + offset++];
			}

			for (int component = 0; component < N; ++component) 
			{
				frame.mOut[component] = isSamplerCubic ? valueFloats[baseIndex + offset++] : 0.0f;
			}
		}
	}
};


Pose LoadRestPose(cgltf_data* data)
{
	unsigned int boneCount = (unsigned int)data->nodes_count;

	Pose result(boneCount);

	for (unsigned int i = 0; i < boneCount; ++i)
	{
		cgltf_node* node = &(data->nodes[i]);

		Transform transform = GLTFHelpers::GetLocalTransform(data->nodes[i]);

		result.SetLocalTransform(i, transform);
		int parent = GLTFHelpers::GetNodeIndex(node->parent, data->nodes, boneCount);
		result.SetParent(i, parent);
	}

	return result;
}

std::vector<Clip> LoadAnimationClips(cgltf_data* data) 
{
	unsigned int numClips = (unsigned int)data->animations_count;
	unsigned int numNodes = (unsigned int)data->nodes_count;

	std::vector<Clip> result;
	result.resize(numClips);

	for (unsigned int i = 0; i < numClips; ++i) 
	{
		result[i].SetName(data->animations[i].name);

		unsigned int numChannels = (unsigned int)data->animations[i].channels_count;
		
		for (unsigned int j = 0; j < numChannels; ++j) 
		{
			cgltf_animation_channel& channel = data->animations[i].channels[j];
			cgltf_node* target = channel.target_node;
		
			int nodeId = GLTFHelpers::GetNodeIndex(target, data->nodes, numNodes);
			
			if (channel.target_path == cgltf_animation_path_type_translation) 
			{
				VectorTrack& track = result[i][nodeId].GetPositionTrack();
				GLTFHelpers::TrackFromChannel<vec3, 3>(track, channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_scale) 
			{
				VectorTrack& track = result[i][nodeId].GetScaleTrack();
				GLTFHelpers::TrackFromChannel<vec3, 3>(track, channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_rotation) 
			{
				QuaternionTrack& track = result[i][nodeId].GetRotationTrack();
				GLTFHelpers::TrackFromChannel<quat, 4>(track, channel);
			}
		}

		result[i].RecalculateDuration();
	}

	return result;
}

Pose LoadBindPose(cgltf_data* data) 
{
	Pose restPose = LoadRestPose(data);
	unsigned int numBones = restPose.Size();

	std::vector<Transform> globalBindPose(numBones);
	for (unsigned int i = 0; i < numBones; ++i) 
	{
		globalBindPose[i] = restPose.GetGlobalTransform(i);
	}

	unsigned int numSkins = (unsigned int)data->skins_count;
	
	for (unsigned int i = 0; i < numSkins; ++i) 
	{
		cgltf_skin* skin = &(data->skins[i]);
		std::vector<float> invBindAccessor;
		GLTFHelpers::GetScalarValues(invBindAccessor, 16, *skin->inverse_bind_matrices);

		unsigned int numJoints = (unsigned int)skin->joints_count;
		for (unsigned int j = 0; j < numJoints; ++j) 
		{
			// Read the inverse bind matrix of the joint
			float* matrix = &(invBindAccessor[j * 16]);
			mat4 invBindMatrix = mat4(matrix);
		
			// invert, convert to transform
			mat4 bindMatrix = inverse(invBindMatrix);
			Transform bindTransform = mat4ToTransform(bindMatrix);
			
			// Set that transform in the globalBindPose.
			cgltf_node* jointNode = skin->joints[j];
			int jointIndex = GLTFHelpers::GetNodeIndex(jointNode, data->nodes, numBones);
			globalBindPose[jointIndex] = bindTransform;
		} // end for each joint
	} // end for each skin
	
	// Convert the global bind pose to a regular bind pose
	Pose bindPose = restPose;
	for (unsigned int i = 0; i < numBones; ++i) 
	{
		Transform current = globalBindPose[i];
		int p = bindPose.GetParent(i);
		if (p >= 0) 
		{ // Bring into parent space
			Transform parent = globalBindPose[p];
			current = combine(inverse(parent), current);
		}
		bindPose.SetLocalTransform(i, current);
	}

	return bindPose;
} 

Skeleton LoadSkeleton(cgltf_data* data) 
{
	return Skeleton(
		LoadRestPose(data),
		LoadBindPose(data),
		LoadJointNames(data)
	);
}
