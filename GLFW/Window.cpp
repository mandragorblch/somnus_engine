#include "GL/gl.h"

#include "Window.h"
#include <stdexcept>
#include <iostream>
#include "Events.h"


GLFWwindow* Window::mWindow;
int Window::width = 0;
int Window::height = 0;

//void window_iconify_callback(GLFWwindow* window, int iconified)
//{
//	if (iconified)
//	{
//		std::cout << "iconified\n";
//	}
//	else
//	{
//		std::cout << "uniconified\n";
//		glfwRestoreWindow(window);
//	}
//}

Window::Window(const std::string& title, int width, int height)
{
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);


	//glfwWindowHint(GLFW_SAMPLES, 2);
	//glEnable(GL_MULTISAMPLE);

	mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	if (!mWindow)
	{
		throw std::runtime_error("Could not initialize window");
	}

	setContextCurrent();
	//glfwSetWindowIconifyCallback(mWindow, window_iconify_callback);
	//glfwSwapInterval(0);

	glewExperimental = true;

	if (glewInit() != GLEW_OK){
		throw std::runtime_error("Could not initialize GLEW");
	}

	glViewport(0, 0, width, height);

	Window::width = width;
	Window::height = height;

}

Window::~Window()
{
	glfwDestroyWindow(mWindow);
}

void Window::setContextCurrent()
{
	glfwMakeContextCurrent(mWindow);
}
void Window::setCursorMode(int mode) {
	glfwSetInputMode(mWindow, GLFW_CURSOR, mode);
}
