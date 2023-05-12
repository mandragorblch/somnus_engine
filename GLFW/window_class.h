#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "src/vk.h"

class window_class
{
private:
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;
public:
	GLFWwindow* window;
	window_class(class vk* p_vk);
	void setCursorMode(int mode);
};