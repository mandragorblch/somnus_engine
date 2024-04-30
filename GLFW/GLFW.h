#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RIGHT_HANDED
#define GLM_FORCE_RADIANS
#define GLM_DEFAULT_ALIGNED_GENTYPES
#include <GLFW/glfw3.h>

class GLFW {
public:
	GLFW();
	~GLFW();
};