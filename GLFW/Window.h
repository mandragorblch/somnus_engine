#pragma once
#include <string>
#include <GLFW/glfw3.h>

//class GLFWwindow;

class Window
{
public:
	static GLFWwindow* mWindow;
	static int width;
	static int height;
	Window(const std::string& title, int width, int height);
	void setContextCurrent();
	static void setCursorMode(int mode);
	~Window();
};