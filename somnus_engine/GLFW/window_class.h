#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vulkan/vk.h"

class window_class
{
public:
	int WIDTH = 1000;
	int HEIGHT = 1000;
	GLFWwindow* window;
	GLFWmonitor* primaryMonitor;
	GLFWvidmode* videoMode;
	int max_width = 0, max_height = 0;
	window_class(class vk* p_vk);
	void setCursorMode(int mode);
	void setCursorPosition(glm::vec2 pos);
	void setFullscreen();
	void setWindowed(int width, int height);
};