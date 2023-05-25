#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "src/vk.h"

class window_class
{
public:
	int WIDTH = 600;
	int HEIGHT = 600;
	GLFWwindow* window;
	window_class(class vk* p_vk);
	void setCursorMode(int mode);
};