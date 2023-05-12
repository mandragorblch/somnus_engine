#include "window_class.h"

void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<vk*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

window_class::window_class(vk* p_vk){
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    p_vk->window = window;
}

void window_class::setCursorMode(int mode) {
    glfwSetInputMode(window, GLFW_CURSOR, mode);
}