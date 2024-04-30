#include "GLFW/GLFW.h"
#include <stdexcept>

GLFW::GLFW() {
	if (!glfwInit()) {
		throw std::runtime_error ("error at initGlFW");
	}
}

GLFW::~GLFW() {
	glfwTerminate();
}