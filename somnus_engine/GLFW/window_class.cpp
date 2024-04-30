#include "window_class.h"

void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<vk*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

window_class::window_class(vk* p_vk){
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, "frenzied friends", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    primaryMonitor = glfwGetPrimaryMonitor();
    videoMode = const_cast<GLFWvidmode*>(glfwGetVideoMode(primaryMonitor));
    max_width = videoMode->width;
    max_height = videoMode->height;

    p_vk->window = window;
}

void window_class::setCursorMode(int mode) {
    glfwSetInputMode(window, GLFW_CURSOR, mode);
}

void window_class::setCursorPosition(glm::vec2 pos)
{
    glfwSetCursorPos(window, pos.x, pos.y);
}

void window_class::setFullscreen()
{
    glfwSetWindowMonitor(window, primaryMonitor, 0, 0, max_width, max_height, GLFW_DONT_CARE);
}

void window_class::setWindowed(int width, int height)
{
    glfwSetWindowMonitor(window, NULL, 900, 32, width, height, GLFW_DONT_CARE);
}
