#pragma once

#include "GLFW/GLFW.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include "tools/camera.h"
#include "GLFW/window_class.h"
#include "misc/vk_misc.h"
#include "tools/three_d_object.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <optional>
#include <set>
#include <unordered_map>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class vk
{
    public:
        vk();
        ~vk();

        std::vector<three_d_object*> objects;

        void run();

        const std::string TEXTURE_PATH = "textures/viking_room.png";

        const int MAX_FRAMES_IN_FLIGHT = 2;

        const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        struct QueueFamilyIndices {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> graphicsAndComputeFamily;
            std::optional<uint32_t> presentFamily;

            bool isComplete() {
                return graphicsFamily.has_value() && graphicsAndComputeFamily.has_value() && presentFamily.has_value();
            }
        };

        struct SwapChainSupportDetails {
            class VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        struct UniformBufferObject {
            alignas(16) glm::mat4 model;
            alignas(16) glm::mat4 view;
            alignas(16) glm::mat4 proj;
        };

        struct SSBO {
            glm::vec3 coeff;
            glm::vec3 pow;
            int radius;
        };

        bool framebufferResized = false;

        class window_class* mWindow;
        class GLFWwindow* window;

        class vk* p_vk;

        class camera* p_camera = new camera(glm::vec3(-1, -1, -1), 45);

        float mouseSensitivity = 5;
        float cameraSpeed = 0.1f;

        SSBO my_ssbo;

    private:

        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
        VkDevice device;

        VkQueue graphicsQueue;
        VkQueue computeQueue;
        VkQueue presentQueue;

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> swapChainFramebuffers;

        VkRenderPass renderPass;
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;

        VkDescriptorSetLayout computeDescriptorSetLayout;
        VkPipelineLayout computePipelineLayout;
        VkPipeline computePipeline;

        VkCommandPool commandPool;

        VkImage colorImage;
        VkDeviceMemory colorImageMemory;
        VkImageView colorImageView;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        std::vector<uint32_t> mipLevels;
        std::vector<VkImage> textureImage;
        VkDeviceMemory textureImageMemory;
        std::vector<VkImageView> textureImageView;
        std::vector<VkSampler> textureSampler;

        std::vector<VkBuffer> vectorVertexBuffers;
        VkDeviceMemory vertexBufferMemory;
        std::vector<VkBuffer> vectorIndexBuffers;
        VkDeviceMemory indexBufferMemory;

        std::vector <VkBuffer> MVP_buffers;
        std::vector <VkDeviceMemory> MVP_buffers_memory;
        std::vector<VkBuffer> object_info_buffers;
        std::vector<VkDeviceMemory> object_info_buffers_memory;
        std::vector <VkBuffer> shader_storage_buffers;
        std::vector <VkDeviceMemory> shader_storage_buffers_memory;

        VkDescriptorPool descriptorPool;
        VkDescriptorPool computeDescriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;
        std::vector<VkDescriptorSet> computeDescriptorSets;

        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkCommandBuffer> computeCommandBuffers;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkSemaphore> computeFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        std::vector<VkFence> computeInFlightFences;
        size_t currentFrame = 0;

        void initWindow();

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

        void initVulkan();

        void mainLoop();

        void cleanupSwapChain();

        void recreateSwapChain();

        void createInstance();

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        void setupDebugMessenger();

        void createSurface();

        void pickPhysicalDevice();

        void createLogicalDevice();

        void createSwapChain();

        void createImageViews();

        void createRenderPass();

        void createDescriptorSetLayout();

        void createComputeDescriptorSetLayout();

        void createGraphicsPipeline();

        void createComputePipeline();

        void createFramebuffers();

        void createCommandPool();

        void createColorResources();

        void createDepthResources();

        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        VkFormat findDepthFormat();

        bool hasStencilComponent(VkFormat format);

        void createTextureImage(std::string path);

        void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

        VkSampleCountFlagBits getMaxUsableSampleCount();

        void createTextureImageView(int index);

        void createTextureSampler();

        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

        void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

        void createObjectBuffers();

        void createVertexBuffer(std::vector<Vertex> vertices);

        void createIndexBuffer(std::vector<uint32_t> indices);

        void createShaderStorageBuffers();

        void createUniformBuffers();

        void createComputeBuffers();

        void createDescriptorPool();

        void createComputeDescriptorPool();

        void createDescriptorSets();

        void createComputeDescriptorSets();

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

        VkCommandBuffer beginSingleTimeCommands();

        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void createCommandBuffers();

        void createComputeCommandBuffers();

        void recordComputeCommandBuffer(VkCommandBuffer commandBuffer);

        void createSyncObjects();

        void updateUniformBuffer(uint32_t currentImage);

        void calc();

        void drawFrame();

        VkShaderModule createShaderModule(const std::vector<char>& code);

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        bool isDeviceSuitable(VkPhysicalDevice device);

        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

        std::vector<const char*> getRequiredExtensions();

        bool checkValidationLayerSupport();

        static std::vector<char> readFile(const std::string& filename);

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};