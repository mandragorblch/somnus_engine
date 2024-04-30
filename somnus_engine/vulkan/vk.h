#pragma once
#ifndef VK_ENGINE
#define VK_ENGINE

#include "GLFW/GLFW.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include "init_storage.h"
#include "tools/camera.h"
#include "GLFW/window_class.h"
#include "misc/vk_misc.h"
//#include "tools/three_d_object.h"
#include "render_objects/two_d_object.h"
#include "render_objects/button.h"
#include "utility/better_debugging.h"
#include "objects/particles/particle.h"

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
#include <ppl.h>
#include "render_objects/model.h"
#include <thread>
#include <queue>
#include <list>
#include <boost/pool/poolfwd.hpp>
#include <boost/pool/pool_alloc.hpp>
//#include <vma/vk_mem_alloc.h>

#define MAX_LIGHT_SOURCES 64

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

        VkDevice device;
        
        std::vector<physicalObjectInfo> physicalObjectsInfo;
        std::list<uint32_t, boost::fast_pool_allocator<uint32_t>> unusedPO;
        std::list<uint32_t, boost::fast_pool_allocator<uint32_t>> unusedClusterIndexes;
        std::vector<class physical_object*>* physicalObjects;

        //std::vector<three_d_object*> threeDObjects;

        std::vector<class model*> models;
        
        std::vector<class mesh*> all_meshes{};
        std::vector<index_wrapper> PO_continuous_indexes{MAX_COMMANDS};
        std::vector<index_wrapper> clusters_continuous{};
        std::vector<struct Vertex> all_vertices{};
        std::vector<uint32_t> all_indices{};

        std::vector<glm::mat4> transforms;

        std::vector<Texture*> all_texures;
        std::vector<Texture*> all_normal_maps;

        std::vector<class two_d_object*> twoDObjects;
        std::vector<class two_d_object*> button_twoDObjects;

        class init_storage* p_init_storage;

        void init_engine();

        const int MAX_FRAMES_IN_FLIGHT = 1;

        uint32_t verts_in_render = 0;

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
            struct VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        uint32_t imageIndex;
        VkResult resultOf_vkAcquireNextImageKHR;
        VkResult resultOf_vkQueuePresentKHR;

        bool framebufferResized = false;
        bool calc_is_performing = false;
        std::vector<bool> shadow_is_casting = { false };
        std::vector<bool> light_moved = { true };
        std::vector<lightSourceInfo> lightSourcesInfo;

        // Depth bias (and slope) are used to avoid shadowing artifacts
        // Constant depth bias factor (always applied)
        float depthBiasConstant = 1.25f;
        // Slope depth bias factor, applied depending on polygon's slope
        float depthBiasSlope = 1.75f;

        class window_class* mWindow;
        struct GLFWwindow* window;

        class vk* p_vk;

        class Ñamera* p_camera = new Ñamera(glm::vec3(5, 7, 5), glm::radians(45.0f));//if fov tends to 47.125 fov breaks

        UniformBufferObject ubo{ p_camera->projView };

        float mouseSensitivity = 5 * 0.0001f;
        float cameraSpeed = 10.f;

        unsigned long long frames = 0;
        std::chrono::steady_clock::time_point time_from_last_frame;
        unsigned long long second_timer = 0;

        unsigned int texture_counter = 0;
        unsigned int normal_map_counter = 0;
        unsigned int model_counter = 0;

        unsigned int current_light_sources = 0;
        unsigned int current_meshes = 0;

        std::vector<class button*> buttons;

        typedef boost::fast_pool_allocator<indirectBatch*> draws_allocator;
        std::list<indirectBatch*, draws_allocator> draws;
        typedef boost::fast_pool_allocator<indirectBatchCluster*> draws_cluster_allocator;
        std::list<indirectBatchCluster*, draws_cluster_allocator> draws_cluster;
        bool draw_object_count_changed = true;
        uint32_t render_object_amount = 0;
        uint32_t physical_object_amount = 0;

        bool screenshot_should_be_saved = false;

        //uint32_t imageIndex;

        //VkResult result;
        void initWindow();

        void recreateSwapChain();

        void recreateClickSwapChain();

        void updateUniformBuffer();  
        void updateCameraInfoBuffers();
        void updateCameraBuffer();
        void updateShadowMapBuffers();
        void updateClickBuffers();
        void retrievePhysicalObjectsInfo();
        void transferPhysicalObjectsInfo();
        void transferAnimationInfo();
        void retrieveDrawIndexedIndirectBuffer();
        void transferDrawIndexedIndirectBuffer();
        
        //std::vector<indirectBatch> compact_draws();

        void runPhysics();
        void waitForPhysicsFinish();

        void prepareRender();
        void drawShadowMapsMultiThread();
        void drawShadowMaps();
        void saveRenderToFile();
        void waitForShadowMapRenderFinish();
        void drawClickMap();
        void waitForClickMapRenderFinish();

        void drawFrame();

        /**
        *\brief checks and adds unique texture to graphic pipeline OR do nothing, but return
        *\brief returns index of unique texture
        */
        unsigned int addUniqueTexture(std::string filename, std::string typeName, class vk* p_vk);
        unsigned int adduniqueModel(std::string filename);
        void prepareParticlesIndividualy();
        void prepareParticlesClusterized();

        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

        VkQueue graphicsQueue;
        VkQueue computeQueue;
        VkQueue presentQueue;

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        VkExtent2D shadowExtent;
        VkExtent2D textureExtent;
        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> swapChainFramebuffers;
        std::vector<VkFramebuffer> shadowFramebuffer;
        std::vector<VkFramebuffer> clickFramebuffer;

        VkRenderPass renderPass;
        VkRenderPass shadowRenderPass;
        VkRenderPass clickRenderPass;

        VkDescriptorSetLayout shadowDescriptorSetLayout;
        VkPipelineLayout shadowPipelineLayout;
        VkPipeline shadowGraphicsPipeline;

        VkDescriptorSetLayout clickDescriptorSetLayout;
        VkPipelineLayout clickPipelineLayout;
        VkPipeline clickGraphicsPipeline;

        VkDescriptorSetLayout threeDDescriptorSetLayout;
        VkPipelineLayout threeDPipelineLayout;
        VkPipeline threeDGraphicsPipeline;

        VkDescriptorSetLayout twoDDescriptorSetLayout;
        VkPipelineLayout twoDPipelineLayout;
        VkPipeline twoDGraphicsPipeline;

        VkDescriptorSetLayout physicComputeDescriptorSetLayout;
        VkPipelineLayout physicComputePipelineLayout;
        VkPipeline physicComputePipeline;

        std::vector<VkCommandPool> commandPoolMultiThreads;
        VkCommandPool commandPool;

        VkImage colorImage;
        VkDeviceMemory colorImageMemory;
        VkImageView colorImageView;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        VkImage depthClickImage;
        VkDeviceMemory depthClickImageMemory;
        VkImageView depthClickImageView;

        VkImage shadowDepthImage;
        VkDeviceMemory shadowDepthImageMemory;
        std::vector<VkImageView> shadowDepthImageView;
        VkImageView shadowDepthImageViewArray;
        VkSampler shadowDepthSampler;

        VkImage clickImage;
        VkDeviceMemory clickImageMemory;
        VkImageView clickImageView;
        VkSampler clickSampler;

        std::vector<uint32_t> mipLevels;
        VkImage textureImage;
        VkDeviceMemory textureImagesMemory;
        std::vector<VkImageView> textureImageView;
        std::vector<VkSampler> textureSampler;
        VkSampler UISampler;

        std::vector<uint32_t> mipLevelsNormalMap;
        VkImage normalMapsImage;
        VkDeviceMemory normalMapsMemory;
        std::vector<VkImageView> normalMapsView;
        std::vector<VkSampler> normalMapsSampler;

        //--------------------------------------------------------------------BUFFERS-----------------------------------------------------------------------------------------------
        std::vector<VkBuffer> vectorThreeDVertexBuffers;
        std::vector<VkDeviceMemory> vertexThreeDBuffersMemory;
        std::vector<void*> vertexThreeDBuffersMemory_data;
        std::vector<VkBuffer> vectorThreeDIndexBuffers;
        std::vector<VkDeviceMemory> indexThreeDBuffersMemory;
        std::vector<void*> indexThreeDBuffersMemory_data;

        std::vector<VkBuffer> finalBonesMatricesBuffers;
        std::vector<VkDeviceMemory> finalBonesMatricesBuffersMemory;
        std::vector<void*> finalBonesMatricesBuffersMemory_data;

        std::vector<VkBuffer> cameraInfo_buffers;
        std::vector<VkDeviceMemory> cameraInfo_buffers_memory;
        std::vector<void*> cameraInfo_buffers_memory_data;

        std::vector<VkBuffer> vectorTwoDVertexBuffers;
        std::vector<VkDeviceMemory> vertexTwoDBuffersMemory;
        std::vector<void*> vertexTwoDBuffersMemory_data;
        std::vector<VkBuffer> vectorTwoDIndexBuffers;
        std::vector<VkDeviceMemory> indexTwoDBuffersMemory;
        std::vector<void*> indexTwoDBuffersMemory_data;

        VkBuffer light_source_uniform_buffer;
        VkDeviceMemory light_source_uniform_buffer_memory;
        void* light_source_uniform_buffer_memory_data;
        std::vector<VkBuffer> light_source_for_render_uniform_buffers;
        std::vector<VkDeviceMemory> light_source_for_render_uniform_buffers_memory;
        std::vector<void*> light_source_for_render_uniform_buffers_memory_data;

        std::vector<VkBuffer> MVP_buffers;
        std::vector<VkDeviceMemory> MVP_buffers_memory;
        std::vector<void*> MVP_buffers_memory_data;
        std::vector<VkBuffer> lightGlobalInfo_buffers;
        std::vector<VkDeviceMemory> lightGlobalInfo_buffers_memory;
        std::vector<void*> lightGlobalInfo_buffers_memory_data;
        std::vector<VkBuffer> object_info_buffers;
        std::vector<VkDeviceMemory> object_info_buffers_memory;
        std::vector<void*> object_info_buffers_memory_data;
        std::vector<VkBuffer> cluster_size_buffers;
        std::vector<VkDeviceMemory> cluster_size_buffers_memory;
        std::vector<void*> cluster_size_buffers_memory_data;
        std::vector<VkBuffer> ui_uniform_buffers;
        std::vector<VkDeviceMemory> ui_uniform_buffers_memory;
        std::vector<void*> ui_uniform_buffers_memory_data;

        std::vector <VkBuffer> physicInfoIn_buffers;
        std::vector <VkDeviceMemory> physicInfoIn_buffers_memory;
        std::vector<void*> physicInfoIn_buffers_memory_data;
        std::vector <VkBuffer> physicalObjectsInfoHost_buffers;
        std::vector <VkDeviceMemory> physicalObjectsInfoHost_buffers_memory;
        VkMappedMemoryRange physicalObjectsInfoHostbuffers_memory_mapped_range;
        std::vector<void*> physicalObjectsInfoHost_buffers_memory_data;
        std::vector <VkBuffer> physicalObjectsInfoDevice_buffers;
        std::vector <VkDeviceMemory> physicalObjectsInfoDevice_buffers_memory;
        std::vector<void*> physicalObjectsInfoDevice_buffers_memory_data;
        std::vector <VkBuffer> verticesBuffer_IN_buffers;
        std::vector <VkDeviceMemory> verticesBuffer_IN_buffers_memory;
        std::vector<void*> verticesBuffer_IN_buffers_memory_data;
        //--------------------------------------------------------------------BUFFERS-----------------------------------------------------------------------------------------------

        VkDescriptorPool shadowDescriptorPool;
        std::vector<VkDescriptorSet> shadowDescriptorSets;
        VkDescriptorPool clickDescriptorPool;
        std::vector<VkDescriptorSet> clickDescriptorSets;
        VkDescriptorPool threeDDescriptorPool;
        std::vector<VkDescriptorSet> threeDDescriptorSets;
        VkDescriptorPool twoDDescriptorPool;
        std::vector<VkDescriptorSet> twoDDescriptorSets;
        VkDescriptorPool physicComputeDescriptorPool;
        std::vector<VkDescriptorSet> physicComputeDescriptorSets;

        std::vector<std::vector<VkCommandBuffer>> shadowGraphicsCommandBuffers_multiThread;
        std::vector<VkCommandBuffer> shadowGraphicsCommandBuffers;
        std::vector<VkCommandBuffer> clickGraphicsCommandBuffers;
        std::vector<VkCommandBuffer> graphicsCommandBuffers;
        std::vector<VkCommandBuffer> physicComputeCommandBuffers;

        std::vector<VkDrawIndexedIndirectCommand> drawIndexedIndirectCommand = std::vector<VkDrawIndexedIndirectCommand>(MAX_COMMANDS);
        std::vector<VkBuffer> drawIndexedIndirectCommand_buffers;
        std::vector<VkDeviceMemory> drawIndexedIndirectCommand_buffers_memory;
        std::vector<void*> drawIndexedIndirectCommand_buffers_memory_data;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> shadowAvailableSemaphores;
        std::vector<VkSemaphore> clickAvailableSemaphores;
        std::vector<VkSemaphore> computeAvailableSemaphores;

        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkSemaphore> shadowRenderFinishedSemaphores;
        std::vector<VkSemaphore> clickRenderFinishedSemaphores;
        std::vector<VkSemaphore> computeFinishedSemaphores;

        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        std::vector<VkFence> shadowInFlightFences;
        std::vector<VkFence> clickInFlightFences;
        std::vector<VkFence> physicComputeInFlightFences;
        size_t currentFrame = 0;

        physicInfo myPhysicInfo;

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

        void initVulkan();

        physicalObjectInfo& getClickInfo(float x, float y);

        void createShadowCasting();

        void createClickCasting();

        void cleanupSwapChain();

        void cleanupClickSwapChain();

        void createInstance();

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        void setupDebugMessenger();

        void createSurface();

        void pickPhysicalDevice();

        void createLogicalDevice();

        void createSwapChain();

        void createImageViews();

        void createRenderPass();

        void createShadowRenderPass();

        void createClickRenderPass();


        void createShadowDescriptorSetLayout();

        void createClickDescriptorSetLayout();

        void create3DDescriptorSetLayout();

        void create2DDescriptorSetLayout();

        void createPhysicComputeDescriptorSetLayout();


        void createShadowGraphicsPipeline();

        void createClickGraphicsPipeline();

        void create3DGraphicsPipeline();

        void create2DGraphicsPipeline();

        void createComputePipeline(std::string path, VkDescriptorSetLayout* computeDescriptorSetLayout, VkPipelineLayout* computePipelineLayout, VkPipeline* computePipeline);


        void createFramebuffers();

        void createShadowFramebuffers(int lightSourceIndex);

        void createClickFramebuffers();

        void createCommandPoolsForMultiThread(int threads_count);

        void createCommandPool(VkCommandPool& commandPool);

        void createColorResources(unsigned int width, unsigned int height, VkImage& colorImage, VkImageView& colorImageView, VkFormat colorFormat, VkDeviceMemory& colorImageMemory, VkSampleCountFlagBits msaaSamples);

        void createDepthResources(unsigned int width, unsigned int height, VkImage& depthImage, VkImageView& depthImageView, VkImageViewType type, VkFormat depthFormat, VkDeviceMemory& depthImageMemory, VkSampleCountFlagBits msaaSamples, uint32_t layers, uint32_t baseLayer);

        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        VkFormat findDepthFormat();

        bool hasStencilComponent(VkFormat format);

        void createTextureImage(const std::vector<Texture*>& textures);

        void createNormalMapImage(const std::vector<Texture*>& normal_maps);

        void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

        VkSampleCountFlagBits getMaxUsableSampleCount();

        void createTextureImageView(int index);

        void createNormalMapImageView(int index);

        void createTextureSampler(VkFilter my_filter, VkBool32 anisotropyEnable, std::vector<VkSampler>* samplers, VkBorderColor borderColor, VkSamplerAddressMode mode);

        void createTextureSampler(VkFilter my_filter, VkBool32 anisotropyEnable, VkSampler& samplers, VkBorderColor borderColor, VkSamplerAddressMode mode);

        VkImageView createImageView(VkImage& image, VkImageViewType type, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, uint32_t layers, uint32_t baseLayer);

        void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageLayout initialLayout, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, VkImageType type, uint32_t arrayLayers);

        void transitionImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, uint32_t layerCount);

        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

        void createObjectBuffers();

        void recreateAspectDependentObjects();
        void applyChangedObject(unsigned int objectIndex);

        void rewriteTwoDVertexBuffer(std::vector<ui_Vertex>& vertices, unsigned int index);

        void createThreeDVertexBuffers(std::vector<Vertex>& vertices);
        void allocateThreeDVertexBuffers();

        void createTwoDVertexBuffer(std::vector<ui_Vertex> &vertices);

        void createThreeDIndexBuffers(std::vector<uint32_t>& indices);
        void allocateThreeDIndexBuffer();

        void createTwoDIndexBuffer(std::vector<uint32_t>& indices);

        void createShaderStorageBuffers();

        void createUniformBuffers();

        void createShadowDescriptorPool();

        void createClickDescriptorPool();

        void createThreeDDescriptorPool();

        void createTwoDDescriptorPool();

        void createPhysicComputeDescriptorPool();


        void createShadowDescriptorSets();

        void createClickDescriptorSets();

        void createThreeDDescriptorSets();

        void createTwoDDescriptorSets();

        void createPhysicComputeDescriptorSets();


        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void createOnlyBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer);
        uint32_t allocateDeviceMemory(VkMemoryPropertyFlags properties, std::vector<VkBuffer>& buffers, VkDeviceMemory& bufferMemory);

        uint32_t allocateDeviceMemory(VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

        VkCommandBuffer beginSingleTimeCommands();

        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);


        void createShadowGraphicsCommandBuffersMultiThread();

        void createShadowGraphicsCommandBuffers();

        void createClickCommandBuffers();

        void createGraphicsCommandBuffers();

        void recordShadowGraphicsCommandBuffer(const VkCommandBuffer& commandBuffer, uint32_t lightSourceIndex);

        void recordClickGraphicsCommandBuffer(const VkCommandBuffer& commandBuffer, uint32_t imageIndex);

        void recordGraphicsCommandBuffer(const VkCommandBuffer& commandBuffer, uint32_t imageIndex);

        void createComputeCommandBuffers(std::vector<VkCommandBuffer>* commandBuffer);

        void recordComputeCommandBuffer(VkDescriptorSet* descriptorSet, VkCommandBuffer* commandBuffer, VkPipeline* computePipeline, VkPipelineLayout* computePipelineLayout, int x, int y, int z);

        void createSyncObjects();

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

#endif