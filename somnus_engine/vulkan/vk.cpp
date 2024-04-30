#pragma once
#include "vulkan/vk.h"
#include "GLFW/Events.h"
#include "maths/math.h"

#include <limits>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

#include <time.h>
#include "utility/termcolor.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define DEPTH_FORMAT VK_FORMAT_D32_SFLOAT
#define CLICK_FORMAT VK_FORMAT_R32_SFLOAT

vk::vk()
{
    std::cout << "vulkan start to init\n";
    //wchar_t* path = new wchar_t[MAX_PATH];
    //std::wstring appendix = L"\\somnus_engine\\res\\config\\paths.txt";
    //GetCurrentDirectoryW(MAX_PATH, path);
    //appendix = path + appendix;
    //std::ifstream file;
    //file.open(appendix);
    //if (!file.is_open()) {
    //    assert(0);
    //}
    //std::string real_path;
    //std::getline(file, real_path);
    //_putenv_s("VK_LAYER_PATH", real_path.data());//rewrite this for any win-PC
    //delete path;
    //file.close();
}

vk::~vk()
{
    delete mWindow;

    delete p_camera;

    cleanupSwapChain();
    for (int i = 0; i < all_meshes.size(); i++) {
        vkDestroySampler(device, textureSampler[i], nullptr);
        vkDestroyImageView(device, textureImageView[i], nullptr);
        vkDestroyImage(device, textureImage, nullptr);
    }
    //vkFreeMemory(device, textureImagesMemory, nullptr);

    vkDestroyDescriptorSetLayout(device, threeDDescriptorSetLayout, nullptr);

    for (int i = 0; i < all_meshes.size(); i++) {
        vkDestroyBuffer(device, vectorThreeDIndexBuffers[i], nullptr);
        vkDestroyBuffer(device, vectorThreeDVertexBuffers[i], nullptr);
    }
    //vkFreeMemory(device, indexThreeDBuffersMemory, nullptr);
    //vkFreeMemory(device, vertexThreeDBuffersMemory, nullptr);

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(device, computeFinishedSemaphores[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);
        vkDestroyFence(device, physicComputeInFlightFences[i], nullptr);
    }

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        vkDestroyBuffer(device, MVP_buffers[i], nullptr);

        vkDestroyBuffer(device, cameraInfo_buffers[i], nullptr);

        vkDestroyBuffer(device, physicInfoIn_buffers[i], nullptr);
        vkFreeMemory(device, physicInfoIn_buffers_memory[i], nullptr);

        vkDestroyBuffer(device, verticesBuffer_IN_buffers[i], nullptr);
        vkFreeMemory(device, verticesBuffer_IN_buffers_memory[i], nullptr);
    }
    //vkFreeMemory(device, MVP_buffers_memory, nullptr);
    //vkFreeMemory(device, cameraInfo_buffers_memory, nullptr);


    //for (int objectIndex = 0; objectIndex < all_meshes.size(); objectIndex++) {//rewrite
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            vkDestroyBuffer(device, object_info_buffers[i], nullptr);
        }
        //vkFreeMemory(device, object_info_buffers_memory, nullptr);
    //}

    vkDestroyDescriptorPool(device, threeDDescriptorPool, nullptr);

    vkDestroyCommandPool(device, commandPool, nullptr);

    vkDestroyDevice(device, nullptr);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();
}

void vk::init_engine()
{
    initVulkan();
    Events::eventInitialize(p_vk);
}

unsigned int vk::addUniqueTexture(std::string filename, std::string typeName, vk* p_vk)
{
    if (typeName == "texture_diffuse") {
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        for (unsigned int j = 0; j < p_vk->all_texures.size(); j++)
        {
            if (std::strcmp(p_vk->all_texures[j]->path.c_str(), (filename).c_str()) == 0)
            {
                return p_vk->all_texures[j]->id;
            }
        }
        // if texture hasn't been loaded already, load it
        Texture* texture = new Texture;
        texture->id = p_vk->texture_counter++;
        texture->type = typeName;
        texture->path = filename;
        p_vk->all_texures.push_back(std::move(texture));  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.

        return texture->id;
    }
    else if (typeName == "texture_normal") {
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        for (unsigned int j = 0; j < p_vk->all_normal_maps.size(); j++)
        {
            if (std::strcmp(p_vk->all_normal_maps[j]->path.c_str(), (filename).c_str()) == 0)
            {
                return p_vk->all_normal_maps[j]->id;
            }
        }
        // if texture hasn't been loaded already, load it
        Texture* texture = new Texture;
        texture->id = p_vk->normal_map_counter++;
        texture->type = typeName;
        texture->path = filename;
        p_vk->all_normal_maps.push_back(std::move(texture));  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.

        return texture->id;
    }
}

unsigned int vk::adduniqueModel(std::string filename)
{
    // check if model was loaded before and if so, continue to next iteration: skip loading a new model
    for (unsigned int j = 0; j < models.size(); j++)
    {
        if (std::strcmp(models[j]->filename.c_str(), (filename).c_str()) == 0)
        {
            return models[j]->id;
        }
    }
    // if model hasn't been loaded already, load it
    model* current_model = new model(filename, this);
    current_model->id = model_counter;
    models.push_back(std::move(current_model));
    model_counter++;

    return current_model->id;
}

void vk::prepareParticlesClusterized()
{
    decltype(p_init_storage->particles[0]->cluster_info.begin()) iter;
    for (auto current : p_init_storage->particles) {
        iter = current->cluster_info.begin();
        while (iter != current->cluster_info.end()) {
            if (clusters_continuous[iter->index].index == 0) {
                current->removeCluster(iter++);
            }
            else {
                ++iter;
            }
            //iter++;
        }
    }
}

//maybe should be fixed
void vk::prepareParticlesIndividualy()
{
    decltype(p_init_storage->particles[0]->cluster_info.front().PO.begin()) iter;
    for (auto current_particle : p_init_storage->particles) {
        auto current_cluster = current_particle->cluster_info.begin();
        while(current_cluster != current_particle->cluster_info.end()){
            iter = current_cluster->PO.begin();
            while (iter != current_cluster->PO.end()) {
                if (p_vk->physicalObjectsInfo[*iter].should_be_deleted) {
                    current_particle->removeInstance(current_cluster, iter++);
                }
                else {
                    ++iter;
                }
            }
        }
    }
}

VkResult vk::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void vk::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{

    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void vk::initWindow()
{
    mWindow = new class window_class(p_vk);
    mWindow->setWindowed(mWindow->WIDTH, mWindow->HEIGHT);
}

void vk::initVulkan()
{
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();   
    createShadowRenderPass();
    createClickRenderPass();

    createShadowDescriptorSetLayout();
    createClickDescriptorSetLayout();
    create3DDescriptorSetLayout();
    create2DDescriptorSetLayout();
    createPhysicComputeDescriptorSetLayout();

    create3DGraphicsPipeline();
    create2DGraphicsPipeline();
    createComputePipeline("somnus_engine/res/shaders/spv/physic_comp.spv", &physicComputeDescriptorSetLayout, &physicComputePipelineLayout, &physicComputePipeline);
    createShadowGraphicsPipeline();
    createClickGraphicsPipeline();

    //createCommandPoolsForMultiThread(std::thread::hardware_concurrency());
    createCommandPool(commandPool);
    createShaderStorageBuffers();
    createColorResources(swapChainExtent.width, swapChainExtent.height, colorImage, colorImageView, swapChainImageFormat, colorImageMemory, msaaSamples);
    createDepthResources(swapChainExtent.width, swapChainExtent.height, depthImage, depthImageView, VK_IMAGE_VIEW_TYPE_2D, findDepthFormat(), depthImageMemory, msaaSamples, 1, 0);
    createDepthResources(swapChainExtent.width, swapChainExtent.height, depthClickImage, depthClickImageView, VK_IMAGE_VIEW_TYPE_2D, findDepthFormat(), depthClickImageMemory, VK_SAMPLE_COUNT_1_BIT, 1, 0);
    createFramebuffers();
    createShadowCasting();
    createUniformBuffers();
    createObjectBuffers();
    createClickCasting();

    createShadowDescriptorPool();
    createClickDescriptorPool();
    createThreeDDescriptorPool();
    createTwoDDescriptorPool();
    createPhysicComputeDescriptorPool();

    createShadowDescriptorSets();
    createThreeDDescriptorSets();
    createTwoDDescriptorSets();
    createPhysicComputeDescriptorSets();
    createClickDescriptorSets();

    //createShadowGraphicsCommandBuffersMultiThread();
    createShadowGraphicsCommandBuffers();
    createGraphicsCommandBuffers();
    createComputeCommandBuffers(&physicComputeCommandBuffers);
    createClickCommandBuffers();

    createSyncObjects();

    p_camera->updateProjectionAspect(static_cast<float>(swapChainExtent.height) / swapChainExtent.width);
}

physicalObjectInfo& vk::getClickInfo(float x, float y)
{
    drawClickMap();
    waitForClickMapRenderFinish();

    VkDeviceSize size = mWindow->WIDTH * mWindow->HEIGHT * 4;
    VkBuffer dstBuffer;
    VkDeviceMemory dstMemory;

    createBuffer(
        size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        dstBuffer,
        dstMemory);
    VkCommandBuffer copyCmd = beginSingleTimeCommands();

    // depth format -> VK_FORMAT_D32_SFLOAT_S8_UINT
    VkBufferImageCopy region{};
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = VkOffset3D{ 0, 0, 0 };
    region.imageExtent = VkExtent3D{ swapChainExtent.width, swapChainExtent.height, 1 };

    VkImage srcImage = clickImage;
    VkImageMemoryBarrier src_barrier{};
    src_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    src_barrier.image = srcImage;
    src_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    src_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    src_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    src_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    src_barrier.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, {}, 0, nullptr, 0, nullptr, 1, &src_barrier);

    vkCmdCopyImageToBuffer(
        copyCmd,
        srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        dstBuffer,
        1,
        &region
    );

    VkImageMemoryBarrier back_to_origin_barrier{};
    back_to_origin_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    back_to_origin_barrier.image = srcImage;
    back_to_origin_barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    back_to_origin_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    back_to_origin_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    back_to_origin_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    back_to_origin_barrier.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, {}, 0, nullptr, 0, nullptr, 1, &back_to_origin_barrier);

    endSingleTimeCommands(copyCmd);

    void* data = nullptr;
    vkMapMemory(device, dstMemory, 0, swapChainExtent.width * swapChainExtent.height * 4, 0, &data);
    uint32_t* array = reinterpret_cast<uint32_t*>(data);

    // Clean up resources
    vkUnmapMemory(device, dstMemory);
    vkFreeMemory(device, dstMemory, nullptr);
    vkDestroyBuffer(device, dstBuffer, nullptr);

    return physicalObjectsInfo[*(array + (static_cast<uint32_t>(x) + static_cast<uint32_t>(y) * swapChainExtent.width))];
}

void vk::createShadowCasting()
{
    shadowFramebuffer.resize(lightSourcesInfo.size());
    createImage(shadowExtent.width, shadowExtent.height, 1, VK_SAMPLE_COUNT_1_BIT, DEPTH_FORMAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED/*VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, shadowDepthImage, shadowDepthImageMemory, VK_IMAGE_TYPE_2D, lightSourcesInfo.size());
    transitionImageLayout(shadowDepthImage, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, lightSourcesInfo.size());
    shadowDepthImageViewArray = createImageView(shadowDepthImage, VK_IMAGE_VIEW_TYPE_2D_ARRAY, DEPTH_FORMAT, VK_IMAGE_ASPECT_DEPTH_BIT, 1, lightSourcesInfo.size(), 0);
    shadowDepthImageView.resize(lightSourcesInfo.size());
    createTextureSampler(VK_FILTER_NEAREST, VK_FALSE, shadowDepthSampler, VK_BORDER_COLOR_INT_OPAQUE_WHITE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
    for (int lightSourceIndex = 0; lightSourceIndex < lightSourcesInfo.size(); lightSourceIndex++) {
        shadowDepthImageView[lightSourceIndex] = createImageView(shadowDepthImage, VK_IMAGE_VIEW_TYPE_2D, DEPTH_FORMAT, VK_IMAGE_ASPECT_DEPTH_BIT, 1, 1, lightSourceIndex);
        createShadowFramebuffers(lightSourceIndex);
    }
}

void vk::createClickCasting()
{
    createImage(swapChainExtent.width, swapChainExtent.height, 1, VK_SAMPLE_COUNT_1_BIT, CLICK_FORMAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED/*VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, clickImage, clickImageMemory, VK_IMAGE_TYPE_2D, 1);
    //createTextureSampler(VK_FILTER_NEAREST, VK_FALSE, clickSampler, VK_BORDER_COLOR_INT_OPAQUE_WHITE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
    clickImageView = createImageView(clickImage, VK_IMAGE_VIEW_TYPE_2D, CLICK_FORMAT, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 0);
    clickFramebuffer.push_back(VkFramebuffer{});
    createClickFramebuffers();
}

void vk::cleanupSwapChain()
{
    vkDestroyImageView(device, depthImageView, nullptr);
    vkDestroyImage(device, depthImage, nullptr);
    vkFreeMemory(device, depthImageMemory, nullptr);

    vkDestroyImageView(device, colorImageView, nullptr);
    vkDestroyImage(device, colorImage, nullptr);
    vkFreeMemory(device, colorImageMemory, nullptr);

    for (uint32_t index = 0; index < swapChainFramebuffers.size(); index++) {
        vkDestroyFramebuffer(device, swapChainFramebuffers[index], nullptr);
    }

    vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(graphicsCommandBuffers.size()), graphicsCommandBuffers.data());

    vkDestroyPipeline(device, threeDGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, threeDPipelineLayout, nullptr);
    vkDestroyPipeline(device, twoDGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, twoDPipelineLayout, nullptr);

    vkDestroyRenderPass(device, renderPass, nullptr);

    for (uint32_t index = 0; index < swapChainImageViews.size(); index++) {
        vkDestroyImageView(device, swapChainImageViews[index], nullptr);
    }

    vkDestroySwapchainKHR(device, swapChain, nullptr);
}

void vk::cleanupClickSwapChain()
{
    vkDestroyImageView(device, depthClickImageView, nullptr);
    vkDestroyImage(device, depthClickImage, nullptr);
    vkFreeMemory(device, depthClickImageMemory, nullptr);

    vkDestroyImageView(device, clickImageView, nullptr);
    vkDestroyImage(device, clickImage, nullptr);
    vkFreeMemory(device, clickImageMemory, nullptr);

    vkDestroyFramebuffer(device, clickFramebuffer[0], nullptr);

    vkDestroyPipeline(device, clickGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, clickPipelineLayout, nullptr);

    vkDestroyRenderPass(device, clickRenderPass, nullptr);
}

void vk::recreateSwapChain()
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device);

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsCommandBuffers();
    create3DGraphicsPipeline();
    create2DGraphicsPipeline();

    createColorResources(swapChainExtent.width, swapChainExtent.height, colorImage, colorImageView, swapChainImageFormat, colorImageMemory, msaaSamples);
    createDepthResources(swapChainExtent.width, swapChainExtent.height, depthImage, depthImageView, VK_IMAGE_VIEW_TYPE_2D, findDepthFormat(), depthImageMemory, msaaSamples, 1, 0);

    createFramebuffers();

    imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);
}

void vk::recreateClickSwapChain()
{
    cleanupClickSwapChain();

    createImage(swapChainExtent.width, swapChainExtent.height, 1, VK_SAMPLE_COUNT_1_BIT, CLICK_FORMAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED/*VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, clickImage, clickImageMemory, VK_IMAGE_TYPE_2D, 1);
    createDepthResources(swapChainExtent.width, swapChainExtent.height, depthClickImage, depthClickImageView, VK_IMAGE_VIEW_TYPE_2D, findDepthFormat(), depthClickImageMemory, VK_SAMPLE_COUNT_1_BIT, 1, 0);
    clickImageView = createImageView(clickImage, VK_IMAGE_VIEW_TYPE_2D, CLICK_FORMAT, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 0);
    createClickRenderPass();
    createClickCommandBuffers();
    createClickGraphicsPipeline();

    createClickFramebuffers();
}

void vk::createInstance()
{
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        assert(0);
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "somnus engine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "somnus";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create instance!");
    }
}

void vk::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void vk::setupDebugMessenger()
{
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void vk::createSurface()
{
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create window surface!");
    }
}

void vk::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        assert(0);
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            msaaSamples = getMaxUsableSampleCount();
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        assert(0);
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void vk::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.graphicsAndComputeFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.multiDrawIndirect = VK_TRUE;//must be checked if this availiable on device REWRITE

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    VkPhysicalDeviceVulkan11Features feature{};
    feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    feature.shaderDrawParameters = VK_TRUE;

    createInfo.pNext = &feature;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(device, indices.graphicsAndComputeFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.graphicsAndComputeFamily.value(), 0, &computeQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void vk::createSwapChain()
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = { indices.graphicsAndComputeFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsAndComputeFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void vk::createImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());

    for (uint32_t i = 0; i < swapChainImages.size(); i++) {
        swapChainImageViews[i] = createImageView(swapChainImages[i], VK_IMAGE_VIEW_TYPE_2D, swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 0);
    }
}

void vk::createRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = msaaSamples;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = msaaSamples;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = swapChainImageFormat;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentResolveRef{};
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    subpass.pResolveAttachments = &colorAttachmentResolveRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create render pass!");
    }
}

void vk::createShadowRenderPass()
{
    VkAttachmentDescription attachmentDescription{};

    attachmentDescription.format = DEPTH_FORMAT;
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;							// Clear depth at beginning of the render pass
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;						// We will read from depth, so it's important to store the depth attachment results
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;					// We don't care about initial layout of the attachment
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL/*VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL*/;// Attachment will be transitioned to shader read at render pass end

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 0;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;			// Attachment will be used as depth/stencil during render pass

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 0;													// No color attachments
    subpass.pDepthStencilAttachment = &depthReference;									// Reference to our depth attachment

    // Use subpass dependencies for layout transitions
    std::array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


    VkRenderPassCreateInfo shadowRenderPassInfo{};
    shadowRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    shadowRenderPassInfo.attachmentCount = 1;
    shadowRenderPassInfo.pAttachments = &attachmentDescription;
    shadowRenderPassInfo.subpassCount = 1;
    shadowRenderPassInfo.pSubpasses = &subpass;
    shadowRenderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    shadowRenderPassInfo.pDependencies = dependencies.data();

    if (vkCreateRenderPass(device, &shadowRenderPassInfo, nullptr, &shadowRenderPass) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create render pass!");
    }

}

void vk::createClickRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = CLICK_FORMAT;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency object_picking_dependency{};
    object_picking_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    object_picking_dependency.dstSubpass = 0;
    object_picking_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    object_picking_dependency.srcAccessMask = 0;
    object_picking_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    object_picking_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkAttachmentDescription attachments[] = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(sizeof(attachments) / sizeof(attachments[0]));
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &object_picking_dependency;

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &clickRenderPass) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create render pass!");
    }
}

void vk::createShadowDescriptorSetLayout()
{
    // Shared pipeline layout for all pipelines used in this sample
    VkDescriptorSetLayoutBinding vertexShaderUniformBuffer{};
    vertexShaderUniformBuffer.binding = 0;
    vertexShaderUniformBuffer.descriptorCount = 1;
    vertexShaderUniformBuffer.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    vertexShaderUniformBuffer.pImmutableSamplers = nullptr;
    vertexShaderUniformBuffer.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding physicalObjectsInfoIn{};
    physicalObjectsInfoIn.binding = 1;
    physicalObjectsInfoIn.descriptorCount = 1;
    physicalObjectsInfoIn.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    physicalObjectsInfoIn.pImmutableSamplers = nullptr;
    physicalObjectsInfoIn.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding bonesMatricesLayoutBindings{};
    bonesMatricesLayoutBindings.binding = 2;
    bonesMatricesLayoutBindings.descriptorCount = 1;
    bonesMatricesLayoutBindings.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bonesMatricesLayoutBindings.pImmutableSamplers = nullptr;
    bonesMatricesLayoutBindings.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding objectInfoLayoutBinding{};
    objectInfoLayoutBinding.binding = 3;
    objectInfoLayoutBinding.descriptorCount = 1;
    objectInfoLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    objectInfoLayoutBinding.pImmutableSamplers = nullptr;
    objectInfoLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    std::array<VkDescriptorSetLayoutBinding, 4> bindings = { vertexShaderUniformBuffer, physicalObjectsInfoIn, bonesMatricesLayoutBindings, objectInfoLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &shadowDescriptorSetLayout) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void vk::createClickDescriptorSetLayout()
{
    // Shared pipeline layout for all pipelines used in this sample
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding physicalObjectsInfoIn{};
    physicalObjectsInfoIn.binding = 1;
    physicalObjectsInfoIn.descriptorCount = 1;
    physicalObjectsInfoIn.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    physicalObjectsInfoIn.pImmutableSamplers = nullptr;
    physicalObjectsInfoIn.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding bonesMatricesLayoutBindings{};
    bonesMatricesLayoutBindings.binding = 2;
    bonesMatricesLayoutBindings.descriptorCount = 1;
    bonesMatricesLayoutBindings.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bonesMatricesLayoutBindings.pImmutableSamplers = nullptr;
    bonesMatricesLayoutBindings.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding objectInfoLayoutBinding{};
    objectInfoLayoutBinding.binding = 3;
    objectInfoLayoutBinding.descriptorCount = 1;
    objectInfoLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    objectInfoLayoutBinding.pImmutableSamplers = nullptr;
    objectInfoLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding bindings[] = {uboLayoutBinding, physicalObjectsInfoIn, bonesMatricesLayoutBindings, objectInfoLayoutBinding};
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(sizeof(bindings) / sizeof(bindings[0]));
    layoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &clickDescriptorSetLayout) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void vk::create3DDescriptorSetLayout()
{
    unsigned int index = 0;
    VkDescriptorSetLayoutBinding physicalObjectsInfoIn{};
    physicalObjectsInfoIn.binding = index;
    physicalObjectsInfoIn.descriptorCount = 1;
    physicalObjectsInfoIn.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    physicalObjectsInfoIn.pImmutableSamplers = nullptr;
    physicalObjectsInfoIn.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    
    index++;
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = index;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    index++;
    VkDescriptorSetLayoutBinding objectInfoLayoutBinding{};
    objectInfoLayoutBinding.binding = index;
    objectInfoLayoutBinding.descriptorCount = 1;
    objectInfoLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    objectInfoLayoutBinding.pImmutableSamplers = nullptr;
    objectInfoLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    index++;
    VkDescriptorSetLayoutBinding bonesMatricesLayoutBindings{};
    bonesMatricesLayoutBindings.binding = index;
    bonesMatricesLayoutBindings.descriptorCount = 1;
    bonesMatricesLayoutBindings.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bonesMatricesLayoutBindings.pImmutableSamplers = nullptr;
    bonesMatricesLayoutBindings.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    index++;
    VkDescriptorSetLayoutBinding lightInfoBindings{};
    lightInfoBindings.binding = index;
    lightInfoBindings.descriptorCount = 1;
    lightInfoBindings.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    lightInfoBindings.pImmutableSamplers = nullptr;
    lightInfoBindings.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    index++;
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = index;
    samplerLayoutBinding.descriptorCount = 2;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    index++;
    VkDescriptorSetLayoutBinding cameraInfoBinding{};
    cameraInfoBinding.binding = index;
    cameraInfoBinding.descriptorCount = 1;
    cameraInfoBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    cameraInfoBinding.pImmutableSamplers = nullptr;
    cameraInfoBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    index++;
    VkDescriptorSetLayoutBinding shadowMapLayoutBinding{};
    shadowMapLayoutBinding.binding = index;
    shadowMapLayoutBinding.descriptorCount = 1;
    shadowMapLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    shadowMapLayoutBinding.pImmutableSamplers = nullptr;
    shadowMapLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    index++;
    VkDescriptorSetLayoutBinding lightGlobalInfo_buffer{};
    lightGlobalInfo_buffer.binding = index;
    lightGlobalInfo_buffer.descriptorCount = 1;
    lightGlobalInfo_buffer.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    lightGlobalInfo_buffer.pImmutableSamplers = nullptr;
    lightGlobalInfo_buffer.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding bindings[] = {physicalObjectsInfoIn, uboLayoutBinding, objectInfoLayoutBinding, bonesMatricesLayoutBindings, lightInfoBindings, samplerLayoutBinding, cameraInfoBinding, shadowMapLayoutBinding, lightGlobalInfo_buffer};
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(index + 1);
    layoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &threeDDescriptorSetLayout) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void vk::create2DDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &twoDDescriptorSetLayout) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void vk::createPhysicComputeDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding physicalObjectsInfoDevice{};
    physicalObjectsInfoDevice.binding = 0;
    physicalObjectsInfoDevice.descriptorCount = 1;
    physicalObjectsInfoDevice.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    physicalObjectsInfoDevice.pImmutableSamplers = nullptr;
    physicalObjectsInfoDevice.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding physicalObjectsInfoHost{};
    physicalObjectsInfoHost.binding = 1;
    physicalObjectsInfoHost.descriptorCount = 1;
    physicalObjectsInfoHost.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    physicalObjectsInfoHost.pImmutableSamplers = nullptr;
    physicalObjectsInfoHost.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding objectInfoLayoutBinding{};
    objectInfoLayoutBinding.binding = 2;
    objectInfoLayoutBinding.descriptorCount = 1;
    objectInfoLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    objectInfoLayoutBinding.pImmutableSamplers = nullptr;
    objectInfoLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding clusterSizeLayoutBinding{};
    clusterSizeLayoutBinding.binding = 3;
    clusterSizeLayoutBinding.descriptorCount = 1;
    clusterSizeLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    clusterSizeLayoutBinding.pImmutableSamplers = nullptr;
    clusterSizeLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding layoutBindings[] = {physicalObjectsInfoDevice, physicalObjectsInfoHost, objectInfoLayoutBinding, clusterSizeLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    constexpr int sz = sizeof(layoutBindings) / sizeof(layoutBindings[0]);
    layoutInfo.bindingCount = sz;
    layoutInfo.pBindings = layoutBindings;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &physicComputeDescriptorSetLayout) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create compute descriptor set layout!");
    }
}

void vk::createShadowGraphicsPipeline()
{
    std::vector<char> vertShaderCode = readFile("somnus_engine/res/shaders/spv/shadow_vert.spv");
    std::vector<char> fragShaderCode = readFile("somnus_engine/res/shaders/spv/shadow_frag.spv");

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)shadowExtent.width;
    viewport.height = (float)shadowExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = shadowExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    //std::vector<VkDynamicState> dynamicStates = {
    //    VK_DYNAMIC_STATE_DEPTH_BIAS/*,
    //    VK_DYNAMIC_STATE_VIEWPORT,
    //    VK_DYNAMIC_STATE_SCISSOR*/
    //};

    //VkPipelineDynamicStateCreateInfo dynamicState{};
    //dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    //dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    //dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &shadowDescriptorSetLayout;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &shadowPipelineLayout) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    //pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = shadowPipelineLayout;
    pipelineInfo.renderPass = shadowRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &shadowGraphicsPipeline) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

void vk::createClickGraphicsPipeline()
{
    std::vector<char> vertShaderCode = readFile("somnus_engine/res/shaders/spv/clicky_vert.spv");
    std::vector<char> fragShaderCode = readFile("somnus_engine/res/shaders/spv/clicky_frag.spv");

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &clickDescriptorSetLayout;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &clickPipelineLayout) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    //pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = clickPipelineLayout;
    pipelineInfo.renderPass = clickRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &clickGraphicsPipeline) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create click pipeline!");
    }

    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

void vk::create3DGraphicsPipeline()
{
    std::vector<char> vertShaderCode = readFile("somnus_engine/res/shaders/spv/three_d_shader_vert.spv");
    std::vector<char> fragShaderCode = readFile("somnus_engine/res/shaders/spv/three_d_shader_frag.spv");

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = msaaSamples;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

 /*   std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();*/

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &threeDDescriptorSetLayout;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &threeDPipelineLayout) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    //pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = threeDPipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &threeDGraphicsPipeline) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

void vk::create2DGraphicsPipeline()
{
    std::vector<char> vertShaderCode = readFile("somnus_engine/res/shaders/spv/ui_shader_vert.spv");
    std::vector<char> fragShaderCode = readFile("somnus_engine/res/shaders/spv/ui_shader_frag.spv");

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = ui_Vertex::getBindingDescription();
    auto attributeDescriptions = ui_Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = msaaSamples;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.depthWriteEnable = VK_FALSE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    /*   std::vector<VkDynamicState> dynamicStates = {
       VK_DYNAMIC_STATE_VIEWPORT,
       VK_DYNAMIC_STATE_SCISSOR
       };

       VkPipelineDynamicStateCreateInfo dynamicState{};
       dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
       dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
       dynamicState.pDynamicStates = dynamicStates.data();*/

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &twoDDescriptorSetLayout;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &twoDPipelineLayout) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    //pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = twoDPipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &twoDGraphicsPipeline) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

void vk::createComputePipeline(std::string path, VkDescriptorSetLayout* computeDescriptorSetLayout, VkPipelineLayout* computePipelineLayout, VkPipeline* computePipeline) {
    auto computeShaderCode = readFile(path);

    VkShaderModule computeShaderModule = createShaderModule(computeShaderCode);

    VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
    computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    computeShaderStageInfo.module = computeShaderModule;
    computeShaderStageInfo.pName = "main";

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = computeDescriptorSetLayout;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, computePipelineLayout) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create compute pipeline layout!");
    }

    VkComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.layout = *computePipelineLayout;
    pipelineInfo.stage = computeShaderStageInfo;

    if (vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, computePipeline) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create compute!");
    }

    vkDestroyShaderModule(device, computeShaderModule, nullptr);
}

void vk::createFramebuffers()
{
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        std::array<VkImageView, 3> attachments = {
            colorImageView,
            depthImageView,
            swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            assert(0);
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void vk::createShadowFramebuffers(int lightSourceIndex)
{
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = shadowRenderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = &shadowDepthImageView[lightSourceIndex];
    framebufferInfo.width = shadowExtent.width;
    framebufferInfo.height = shadowExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &shadowFramebuffer[lightSourceIndex]) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create framebuffer!");
    }
}

void vk::createClickFramebuffers()
{
    std::array<VkImageView, 2> attachments = {
        clickImageView,
        depthClickImageView
    };

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = clickRenderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = swapChainExtent.width;
    framebufferInfo.height = swapChainExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &clickFramebuffer.back()) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create framebuffer!");
    }
}

void vk::createCommandPoolsForMultiThread(int threads_count)//maximum is std::thread::hardware_concurrency()
{
    p_init_storage->max_threads_available = threads_count;
    commandPoolMultiThreads.resize(p_init_storage->max_threads_available);
    for (int threadIndex = 0; threadIndex < p_init_storage->max_threads_available; threadIndex++) {
        createCommandPool(commandPoolMultiThreads[threadIndex]);
    }
}

void vk::createCommandPool(VkCommandPool& commandPool)
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsAndComputeFamily.value();

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create graphics command pool!");
    }
}

void vk::createColorResources(unsigned int width, unsigned int height, VkImage& colorImage, VkImageView& colorImageView, VkFormat colorFormat, VkDeviceMemory& colorImageMemory, VkSampleCountFlagBits msaaSamples)
{
    createImage(width, height, 1, msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage, colorImageMemory, VK_IMAGE_TYPE_2D, 1);
    colorImageView = createImageView(colorImage, VK_IMAGE_VIEW_TYPE_2D, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 0);
}

void vk::createDepthResources(unsigned int width, unsigned int height, VkImage& depthImage, VkImageView& depthImageView, VkImageViewType type, VkFormat depthFormat, VkDeviceMemory& depthImageMemory, VkSampleCountFlagBits msaaSamples, uint32_t layers, uint32_t baseLayer)
{
    createImage(width, height, 1, msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory, VK_IMAGE_TYPE_2D, 1);
    depthImageView = createImageView(depthImage, type, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, layers, baseLayer);
}

VkFormat vk::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    assert(0);
    throw std::runtime_error("failed to find supported format!");
}

VkFormat vk::findDepthFormat()
{
    return findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

bool vk::hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void vk::createTextureImage(const std::vector<Texture*>& textures)
{
    uint32_t localMipLevels = static_cast<uint32_t>(std::floor(std::log2((std::max)(textureExtent.width, textureExtent.height)))) + 1;
    VkDeviceSize imagesSize = textureExtent.width * textureExtent.height * 4 * texture_counter;
    std::vector<stbi_uc> texturesData(imagesSize);
    textureSampler.resize(texture_counter);
    int textureIndex = 0;
    for (const Texture* current_texture : textures) {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(("somnus_engine/res/textures/" + current_texture->path).c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        if (texWidth != textureExtent.width || texHeight != textureExtent.height) {
            assert(0);
            throw std::runtime_error("is not suitable!");
        }
        if (!pixels) {
            assert(0);
            throw std::runtime_error("failed to load texture image!");
        }

        memcpy(&(texturesData[(textureIndex) * imagesSize / texture_counter]), pixels, imagesSize / texture_counter);
        mipLevels.push_back(localMipLevels);
        createTextureSampler(VK_FILTER_LINEAR, VK_TRUE, textureSampler[textureIndex++], VK_BORDER_COLOR_INT_OPAQUE_BLACK, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    }
    createImage(textureExtent.width, textureExtent.height, localMipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImagesMemory, VK_IMAGE_TYPE_2D, texture_counter);
    textureImageView.resize(texture_counter);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(imagesSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, imagesSize, 0, &data);
    memcpy(data, texturesData.data(), static_cast<size_t>(imagesSize));
    vkUnmapMemory(device, stagingBufferMemory);

    //transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, localMipLevels);
    //copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(textureExtent.width), static_cast<uint32_t>(textureExtent.height));
    std::vector<VkBufferImageCopy> bufferCopyRegions{ texture_counter };
    for (uint32_t layer = 0; layer < texture_counter; layer++)
    { 
        // Setup a buffer image copy structure for the current array layer
        VkBufferImageCopy& bufferCopyRegion = bufferCopyRegions[layer];
        bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bufferCopyRegion.imageSubresource.mipLevel = 0;
        bufferCopyRegion.imageSubresource.baseArrayLayer = layer;
        bufferCopyRegion.imageSubresource.layerCount = 1;
        bufferCopyRegion.imageExtent.width = textureExtent.width;
        bufferCopyRegion.imageExtent.height = textureExtent.height;
        bufferCopyRegion.imageExtent.depth = 1;
        bufferCopyRegion.bufferOffset = layer * imagesSize / texture_counter;
    }


    //// Image barrier for optimal image (target)
    //// Set initial layout for all array layers (faces) of the optimal (target) tiled texture
    //VkImageSubresourceRange subresourceRange = {};
    //subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //subresourceRange.baseMipLevel = 0;
    //subresourceRange.levelCount = 1;
    //subresourceRange.layerCount = texture_counter;
    transitionImageLayout(textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, localMipLevels, texture_counter);

    VkCommandBuffer copyCmd = beginSingleTimeCommands();

    vkCmdCopyBufferToImage(copyCmd, stagingBuffer, textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(bufferCopyRegions.size()), bufferCopyRegions.data());

    endSingleTimeCommands(copyCmd);

    transitionImageLayout(textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, localMipLevels, texture_counter);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    for (int i = 0; i < texture_counter; i++) {
        textureImageView[i] = createImageView(textureImage, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels[i], 1, i);
    }
}

void vk::createNormalMapImage(const std::vector<Texture*>& normal_maps)
{
    uint32_t localMipLevels = static_cast<uint32_t>(std::floor(std::log2((std::max)(textureExtent.width, textureExtent.height)))) + 1;
    VkDeviceSize imagesSize = textureExtent.width * textureExtent.height * 4 * normal_map_counter;
    std::vector<unsigned char> texturesData(imagesSize);
    int normal_map_index = 0;
    for (const Texture* current_normal_map : normal_maps) {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(("somnus_engine/res/textures/" + current_normal_map->path).c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        if (texWidth != textureExtent.width || texHeight != textureExtent.height) {
            assert(0);
            throw std::runtime_error("is not suitable!");
        }
        if (!pixels) {
            assert(0);
            throw std::runtime_error("failed to load texture image!");
        }

        memcpy(&texturesData[(normal_map_index++) * imagesSize / normal_map_counter], pixels, imagesSize / normal_map_counter);
        mipLevelsNormalMap.push_back(localMipLevels);
    }
    createImage(textureExtent.width, textureExtent.height, localMipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, normalMapsImage, normalMapsMemory, VK_IMAGE_TYPE_2D, normal_map_counter);
    normalMapsView.resize(normal_map_counter);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(imagesSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, imagesSize, 0, &data);
    memcpy(data, texturesData.data(), static_cast<size_t>(imagesSize));
    vkUnmapMemory(device, stagingBufferMemory);

    std::vector<VkBufferImageCopy> bufferCopyRegions;
    for (uint32_t layer = 0; layer < normal_map_counter; layer++)
    {
        // Setup a buffer image copy structure for the current array layer
        VkBufferImageCopy bufferCopyRegion = {};
        bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bufferCopyRegion.imageSubresource.mipLevel = 0;
        bufferCopyRegion.imageSubresource.baseArrayLayer = layer;
        bufferCopyRegion.imageSubresource.layerCount = 1;
        bufferCopyRegion.imageExtent.width = textureExtent.width;
        bufferCopyRegion.imageExtent.height = textureExtent.height;
        bufferCopyRegion.imageExtent.depth = 1;
        bufferCopyRegion.bufferOffset = layer * imagesSize / normal_map_counter;
        bufferCopyRegions.push_back(bufferCopyRegion);
    }

    transitionImageLayout(normalMapsImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, localMipLevels, normal_map_counter);

    VkCommandBuffer copyCmd = beginSingleTimeCommands();
    vkCmdCopyBufferToImage(copyCmd, stagingBuffer, normalMapsImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(bufferCopyRegions.size()), bufferCopyRegions.data());

    endSingleTimeCommands(copyCmd);

    transitionImageLayout(normalMapsImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, localMipLevels, normal_map_counter);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    for (int i = 0; i < normal_map_counter; i++) {
        normalMapsView[i] = createImageView(normalMapsImage, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, mipLevelsNormalMap[i], 1, i);
    }
    //int texWidth, texHeight, texChannels;
    //unsigned char* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    //VkDeviceSize imageSize = texWidth * texHeight * 4;
    //uint32_t localMipLevels = static_cast<uint32_t>(std::floor(std::log2((std::max)(texWidth, texHeight)))) + 1;

    ////print_r(std::to_string(pixels[0]/ 255.) + " " + std::to_string(pixels[1] / 255.) + " " + std::to_string(pixels[2] / 255.));

    //if (!pixels) {
    //    assert(0);
    //    throw std::runtime_error("failed to load texture image!");
    //}

    //VkBuffer stagingBuffer;
    //VkDeviceMemory stagingBufferMemory;
    //createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    //void* data;
    //vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
    //memcpy(data, pixels, static_cast<size_t>(imageSize));
    //vkUnmapMemory(device, stagingBufferMemory);

    //stbi_image_free(pixels);

    //VkImage localTextureImage;
    //VkDeviceMemory localTextureImageBuffer;
    //createImage(texWidth, texHeight, localMipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, localTextureImage, localTextureImageBuffer, VK_IMAGE_TYPE_2D, 1);

    //transitionImageLayout(localTextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, localMipLevels);
    //copyBufferToImage(stagingBuffer, localTextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    ////transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

    //vkDestroyBuffer(device, stagingBuffer, nullptr);
    //vkFreeMemory(device, stagingBufferMemory, nullptr);

    //generateMipmaps(localTextureImage, VK_FORMAT_R8G8B8A8_UNORM, texWidth, texHeight, localMipLevels);

    //mipLevels.push_back(localMipLevels);
    //textureImage.push_back(localTextureImage);
    //textureImagesMemory.push_back(localTextureImageBuffer);
}

void vk::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        assert(0);
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer,
            image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit,
            VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    endSingleTimeCommands(commandBuffer);
}

VkSampleCountFlagBits vk::getMaxUsableSampleCount()
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    //std::cout << "\033[38;2;0;0;255m" << physicalDeviceProperties.driverVersion << "\033[0m\n";

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}

void vk::createTextureSampler(VkFilter my_filter, VkBool32 anisotropyEnable, std::vector<VkSampler>* samplers, VkBorderColor borderColor, VkSamplerAddressMode mode)
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = my_filter;
    samplerInfo.minFilter = my_filter;
    samplerInfo.addressModeU = mode;
    samplerInfo.addressModeV = mode;
    samplerInfo.addressModeW = mode;
    samplerInfo.anisotropyEnable = anisotropyEnable;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = borderColor;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    samplerInfo.mipLodBias = 0.0f;

    VkSampler localTextureSampler;
    if (vkCreateSampler(device, &samplerInfo, nullptr, &localTextureSampler) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create texture sampler!");
    }
    samplers->push_back(localTextureSampler);
}

void vk::createTextureSampler(VkFilter my_filter, VkBool32 anisotropyEnable, VkSampler& sampler, VkBorderColor borderColor, VkSamplerAddressMode mode)
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = my_filter;
    samplerInfo.minFilter = my_filter;
    samplerInfo.addressModeU = mode;
    samplerInfo.addressModeV = mode;
    samplerInfo.addressModeW = mode;
    samplerInfo.anisotropyEnable = anisotropyEnable;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = borderColor;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    samplerInfo.mipLodBias = 0.0f;

    if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create texture sampler!");
    }
}

VkImageView vk::createImageView(VkImage& image, VkImageViewType type, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, uint32_t layers, uint32_t baseLayer)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = type;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = baseLayer;
    viewInfo.subresourceRange.layerCount = layers;

    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

void vk::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageLayout initialLayout, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, VkImageType type, uint32_t arrayLayers)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = type;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = arrayLayers;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = initialLayout;
    imageInfo.usage = usage;
    imageInfo.samples = numSamples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(device, image, imageMemory, 0);
}

void vk::transitionImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, uint32_t layerCount)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = aspectMask;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = layerCount;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        assert(0);
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    endSingleTimeCommands(commandBuffer);
}

void vk::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(commandBuffer);
}

void vk::createObjectBuffers()
{
    unsigned int local_texture_counter = 0;
        //if (current_texture->type == "texture_normal") {
        //}
        //else {
        //    createTextureImage("somnus_engine/res/textures/" + current_texture->path);
        //    createTextureImageView(current_texture->id);
        //}
        //createTextureSampler(VK_FILTER_LINEAR, VK_TRUE, textureSampler, mipLevels.back(), VK_BORDER_COLOR_INT_OPAQUE_BLACK, VK_SAMPLER_ADDRESS_MODE_REPEAT);

    if (all_meshes.size()) {
        for (class mesh* current_mesh : all_meshes) {
            createThreeDVertexBuffers(current_mesh->vertices);
            createThreeDIndexBuffers(current_mesh->indices);
        }
        allocateThreeDVertexBuffers();
        allocateThreeDIndexBuffer();
    }
    else {
        assert(0);
        throw std::runtime_error("empty 3d buffers");
    }

    if (twoDObjects.size()) {
        for (/*class two_d_object* object : twoDObjects*/int iterator = 0; iterator < twoDObjects.size(); iterator++) {
            class two_d_object* object = twoDObjects[iterator];
            object->texIndex = addUniqueTexture(object->texture_path, "texture_diffuse", p_vk);
            createTwoDVertexBuffer(*(object->vertices));
            createTwoDIndexBuffer(object->indices);
            //createTextureImageView(local_texture_counter);//rewrite?
            createTextureSampler(VK_FILTER_NEAREST, VK_FALSE, UISampler, VK_BORDER_COLOR_INT_OPAQUE_BLACK, VK_SAMPLER_ADDRESS_MODE_REPEAT);
            applyChangedObject(object->objectIndex);
        }
    }

    if (buttons.size()) {
        for (button* m_button : buttons) {
            m_button->calculate_pixel_pos();
        }
    }

    createTextureImage(all_texures);
    createNormalMapImage(all_normal_maps);
}

void vk::recreateAspectDependentObjects()
{
    void* data;
    for (unsigned int objectIndex = 0; objectIndex < twoDObjects.size(); objectIndex++) {
        rewriteTwoDVertexBuffer(*(twoDObjects[objectIndex]->vertices), objectIndex);
    }
}

void vk::applyChangedObject(unsigned int objectIndex)
{
    rewriteTwoDVertexBuffer(*(twoDObjects[objectIndex]->vertices), objectIndex);
    for (int imageIndex = 0; imageIndex < swapChainImages.size(); imageIndex++) {
        memcpy(ui_uniform_buffers_memory_data[imageIndex + objectIndex * swapChainImages.size()], &twoDObjects[objectIndex]->info, sizeof(twoDObjects[objectIndex]->info));
    }
}

void vk::rewriteTwoDVertexBuffer(std::vector<ui_Vertex>& vertices, unsigned int index)
{
    memcpy(vertexTwoDBuffersMemory_data[0], vertices.data(), (size_t)(sizeof(ui_Vertex) * vertices.size()));
}

void vk::createThreeDVertexBuffers(std::vector<Vertex>& vertices)
{
    VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();
    unsigned int index = vectorThreeDVertexBuffers.size();
    VkBuffer localVertexBuffer;
    all_vertices.insert(all_vertices.end(), vertices.begin(), vertices.end());
    vectorThreeDVertexBuffers.push_back(std::move(localVertexBuffer));
    createOnlyBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vectorThreeDVertexBuffers.back());
}

void vk::allocateThreeDVertexBuffers()
{
    VkDeviceMemory local_memory = VkDeviceMemory{};
    vertexThreeDBuffersMemory.push_back(std::move(local_memory));
    uint32_t bufferSize = allocateDeviceMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vectorThreeDVertexBuffers, vertexThreeDBuffersMemory.back());

    void* data = nullptr;
    vertexThreeDBuffersMemory_data.push_back(std::move(data));

    VkCommandBuffer copyCmd = beginSingleTimeCommands();

    std::vector<VkBuffer> hostBuffers(vectorThreeDVertexBuffers.size());
    VkDeviceMemory hostMemory{};

    VkMemoryRequirements memRequirements;
    for (unsigned int bufferIndex = 0; bufferIndex < hostBuffers.size(); bufferIndex++) {
        vkGetBufferMemoryRequirements(device, vectorThreeDVertexBuffers[bufferIndex], &memRequirements);
        createOnlyBuffer(
            memRequirements.size,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            hostBuffers[bufferIndex]
        );
    }

    allocateDeviceMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, hostBuffers, hostMemory);

    vkMapMemory(device, hostMemory, 0, bufferSize, 0, &vertexThreeDBuffersMemory_data.back());
    memcpy(vertexThreeDBuffersMemory_data.back(), all_vertices.data(), (size_t)bufferSize);

    std::vector<VkBufferCopy> region(vectorThreeDVertexBuffers.size());

    uint32_t bufferOffset = 0;
    for (unsigned int bufferIndex = 0; bufferIndex < vectorThreeDVertexBuffers.size(); ++bufferIndex) {
        vkGetBufferMemoryRequirements(device, vectorThreeDVertexBuffers[bufferIndex], &memRequirements);
        region[bufferIndex].size = memRequirements.size;
        region[bufferIndex].dstOffset = 0;
        region[bufferIndex].srcOffset = 0;
        //bufferOffset += memRequirements.size; //maxSize;

        vkCmdCopyBuffer(copyCmd, hostBuffers[bufferIndex], vectorThreeDVertexBuffers[bufferIndex], 1, &region[bufferIndex]);
    }

    endSingleTimeCommands(copyCmd);

    vkUnmapMemory(device, hostMemory);
    vkFreeMemory(device, hostMemory, nullptr);
    for (unsigned int bufferIndex = 0; bufferIndex < vectorThreeDVertexBuffers.size(); ++bufferIndex)
        vkDestroyBuffer(device, hostBuffers[bufferIndex], nullptr);
}

void vk::createTwoDVertexBuffer(std::vector<ui_Vertex>& vertices)
{
    VkDeviceSize bufferSize = sizeof(ui_Vertex) * vertices.size();
    unsigned int index = vectorTwoDVertexBuffers.size();
    VkBuffer localVertexBuffer;
    VkDeviceMemory localVertexBufferMemory;
    void* data;
    vectorTwoDVertexBuffers.push_back(localVertexBuffer);
    vertexTwoDBuffersMemory.push_back(localVertexBufferMemory);
    vertexTwoDBuffersMemory_data.push_back(data);
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vectorTwoDVertexBuffers[index], vertexTwoDBuffersMemory[index]);

    vkMapMemory(device, vertexTwoDBuffersMemory[index], 0, bufferSize, 0, &vertexTwoDBuffersMemory_data[index]);
    memcpy(vertexTwoDBuffersMemory_data[index], vertices.data(), (size_t)bufferSize);
}

void vk::createThreeDIndexBuffers(std::vector<uint32_t>& indices)
{
    VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();
    unsigned int index = vectorThreeDIndexBuffers.size();
    VkBuffer localIndexBuffer;
    all_indices.insert(all_indices.end(), indices.begin(), indices.end());
    vectorThreeDIndexBuffers.push_back(std::move(localIndexBuffer));
    createOnlyBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, vectorThreeDIndexBuffers.back());
}

void vk::allocateThreeDIndexBuffer()
{
    VkDeviceMemory local_memory = VkDeviceMemory{};
    indexThreeDBuffersMemory.push_back(std::move(local_memory));
    uint32_t bufferSize = allocateDeviceMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vectorThreeDIndexBuffers, indexThreeDBuffersMemory.back());
    //uint32_t bufferSize = sizeof(uint32_t) * all_indices.size();
    void* data = nullptr;
    indexThreeDBuffersMemory_data.push_back(std::move(data));

    VkCommandBuffer copyCmd = beginSingleTimeCommands();

    std::vector<VkBuffer> hostBuffers(vectorThreeDIndexBuffers.size());
    VkDeviceMemory hostMemory{};

    VkMemoryRequirements memRequirements;
    for (unsigned int bufferIndex = 0; bufferIndex < hostBuffers.size(); bufferIndex++) {
        vkGetBufferMemoryRequirements(device, vectorThreeDIndexBuffers[bufferIndex], &memRequirements);
        createOnlyBuffer(
            memRequirements.size,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            hostBuffers[bufferIndex]
        );
    }

    allocateDeviceMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, hostBuffers, hostMemory);

    vkMapMemory(device, hostMemory, 0, bufferSize, 0, &indexThreeDBuffersMemory_data.back());
    memcpy(indexThreeDBuffersMemory_data.back(), all_indices.data(), (size_t)bufferSize);

    std::vector<VkBufferCopy> region(vectorThreeDIndexBuffers.size());

    uint32_t bufferOffset = 0;
    for (unsigned int bufferIndex = 0; bufferIndex < vectorThreeDIndexBuffers.size(); ++bufferIndex) {
        vkGetBufferMemoryRequirements(device, vectorThreeDIndexBuffers[bufferIndex], &memRequirements);
        region[bufferIndex].size = memRequirements.size;
        region[bufferIndex].dstOffset = 0;
        region[bufferIndex].srcOffset = 0;
        //bufferOffset += memRequirements.size; //maxSize;

        vkCmdCopyBuffer(copyCmd, hostBuffers[bufferIndex], vectorThreeDIndexBuffers[bufferIndex], 1, &region[bufferIndex]);
    }

    endSingleTimeCommands(copyCmd);

    vkUnmapMemory(device, hostMemory);
    vkFreeMemory(device, hostMemory, nullptr);
    for (unsigned int bufferIndex = 0; bufferIndex < vectorThreeDIndexBuffers.size(); ++bufferIndex)
        vkDestroyBuffer(device, hostBuffers[bufferIndex], nullptr);
}

void vk::createTwoDIndexBuffer(std::vector<uint32_t>& indices)
{
    VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();

    unsigned int index = vectorTwoDIndexBuffers.size();
    VkBuffer localIndexBuffer;
    VkDeviceMemory localIndexBufferMemory;
    void* data;
    vectorTwoDIndexBuffers.push_back(localIndexBuffer);
    indexTwoDBuffersMemory.push_back(localIndexBufferMemory);
    indexTwoDBuffersMemory_data.push_back(data);
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vectorTwoDIndexBuffers[index], indexTwoDBuffersMemory[index]);
    
    vkMapMemory(device, indexTwoDBuffersMemory[index], 0, bufferSize, 0, &indexTwoDBuffersMemory_data[index]);
    memcpy(indexTwoDBuffersMemory_data[index], indices.data(), (size_t)bufferSize);
}

void vk::createShaderStorageBuffers() {

    physicalObjectsInfoHost_buffers.resize(swapChainImages.size());
    physicalObjectsInfoHost_buffers_memory.resize(swapChainImages.size());
    physicalObjectsInfoHost_buffers_memory_data.resize(swapChainImages.size());

    physicalObjectsInfoDevice_buffers.resize(swapChainImages.size());
    physicalObjectsInfoDevice_buffers_memory.resize(swapChainImages.size());
    physicalObjectsInfoDevice_buffers_memory_data.resize(swapChainImages.size());

    verticesBuffer_IN_buffers.resize(swapChainImages.size());
    verticesBuffer_IN_buffers_memory.resize(swapChainImages.size());
    verticesBuffer_IN_buffers_memory_data.resize(swapChainImages.size());

    finalBonesMatricesBuffers.resize(swapChainImages.size());
    finalBonesMatricesBuffersMemory.resize(swapChainImages.size());
    finalBonesMatricesBuffersMemory_data.resize(swapChainImages.size());

    drawIndexedIndirectCommand_buffers.resize(swapChainImages.size());
    drawIndexedIndirectCommand_buffers_memory.resize(swapChainImages.size());
    drawIndexedIndirectCommand_buffers_memory_data.resize(swapChainImages.size());

    object_info_buffers.resize(swapChainImages.size());
    object_info_buffers_memory.resize(swapChainImages.size());
    object_info_buffers_memory_data.resize(swapChainImages.size());

    cluster_size_buffers.resize(swapChainImages.size());
    cluster_size_buffers_memory.resize(swapChainImages.size());
    cluster_size_buffers_memory_data.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        createOnlyBuffer(
            sizeof(physicalObjectInfo) * MAX_COMMANDS,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            physicalObjectsInfoDevice_buffers[i]);        
        allocateDeviceMemory(
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            physicalObjectsInfoDevice_buffers[i],
            physicalObjectsInfoDevice_buffers_memory[i]);

        createOnlyBuffer(
            sizeof(physicalObjectInfo) * MAX_COMMANDS, 
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            physicalObjectsInfoHost_buffers[i]);       
        allocateDeviceMemory(
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
            physicalObjectsInfoHost_buffers[i],
                physicalObjectsInfoHost_buffers_memory[i]);   
        vkMapMemory(device, physicalObjectsInfoHost_buffers_memory[i], 0, sizeof(physicalObjectInfo) * MAX_COMMANDS, 0, &physicalObjectsInfoHost_buffers_memory_data[i]);
        physicalObjectsInfoHostbuffers_memory_mapped_range.memory = physicalObjectsInfoHost_buffers_memory[i]; 
        physicalObjectsInfoHostbuffers_memory_mapped_range.offset = 0;
        physicalObjectsInfoHostbuffers_memory_mapped_range.pNext = NULL;
        physicalObjectsInfoHostbuffers_memory_mapped_range.size = sizeof(physicalObjectInfo) * MAX_COMMANDS;
        physicalObjectsInfoHostbuffers_memory_mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        memcpy(physicalObjectsInfoHost_buffers_memory_data[i], physicalObjectsInfo.data(), sizeof(physicalObjectInfo) * physicalObjectsInfo.size());

        createOnlyBuffer(
            sizeof(finalBonesMatrices), 
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            finalBonesMatricesBuffers[i]);
        allocateDeviceMemory(
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            finalBonesMatricesBuffers[i],
            finalBonesMatricesBuffersMemory[i]);
        vkMapMemory(device, finalBonesMatricesBuffersMemory[i], 0, sizeof(finalBonesMatrices), 0, &finalBonesMatricesBuffersMemory_data[i]);

        createOnlyBuffer(
            sizeof(VkDrawIndexedIndirectCommand) * MAX_COMMANDS,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
            drawIndexedIndirectCommand_buffers[i]);
        allocateDeviceMemory(
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            drawIndexedIndirectCommand_buffers[i],
            drawIndexedIndirectCommand_buffers_memory[i]);
        vkMapMemory(device, drawIndexedIndirectCommand_buffers_memory[i], 0, sizeof(VkDrawIndexedIndirectCommand) * MAX_COMMANDS, 0, &drawIndexedIndirectCommand_buffers_memory_data[i]);

        createOnlyBuffer(
            sizeof(index_wrapper) * MAX_COMMANDS,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            object_info_buffers[i]);
        allocateDeviceMemory(
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            object_info_buffers[i],
            object_info_buffers_memory[i]);
        vkMapMemory(device, object_info_buffers_memory[i], 0, sizeof(index_wrapper) * MAX_COMMANDS, 0, &object_info_buffers_memory_data[i]);

        createOnlyBuffer(
            sizeof(index_wrapper) * MAX_COMMANDS,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            cluster_size_buffers[i]);
        allocateDeviceMemory(
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            cluster_size_buffers[i],
            cluster_size_buffers_memory[i]);
        vkMapMemory(device, cluster_size_buffers_memory[i], 0, sizeof(index_wrapper) * MAX_COMMANDS, 0, &cluster_size_buffers_memory_data[i]);
    }
}

void vk::createUniformBuffers()
{
    MVP_buffers.resize(swapChainImages.size());
    MVP_buffers_memory.resize(swapChainImages.size());
    MVP_buffers_memory_data.resize(swapChainImages.size());

    lightGlobalInfo_buffers.resize(swapChainImages.size());
    lightGlobalInfo_buffers_memory.resize(swapChainImages.size());
    lightGlobalInfo_buffers_memory_data.resize(swapChainImages.size());

    ui_uniform_buffers.resize(swapChainImages.size() * twoDObjects.size());
    ui_uniform_buffers_memory.resize(swapChainImages.size() * twoDObjects.size());
    ui_uniform_buffers_memory_data.resize(swapChainImages.size() * twoDObjects.size());

    physicInfoIn_buffers.resize(swapChainImages.size());
    physicInfoIn_buffers_memory.resize(swapChainImages.size());
    physicInfoIn_buffers_memory_data.resize(swapChainImages.size());

    cameraInfo_buffers.resize(swapChainImages.size());
    cameraInfo_buffers_memory.resize(swapChainImages.size());
    cameraInfo_buffers_memory_data.resize(swapChainImages.size());

    light_source_for_render_uniform_buffers.resize(swapChainImages.size() * lightSourcesInfo.size());
    light_source_for_render_uniform_buffers_memory.resize(swapChainImages.size() * lightSourcesInfo.size());
    light_source_for_render_uniform_buffers_memory_data.resize(swapChainImages.size() * lightSourcesInfo.size());

    for (int i = 0; i < swapChainImages.size(); i++) {
        createBuffer(sizeof(physicInfo), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, physicInfoIn_buffers[i], physicInfoIn_buffers_memory[i]);
        vkMapMemory(device, physicInfoIn_buffers_memory[i], 0, sizeof(physicInfo), 0, &physicInfoIn_buffers_memory_data[i]);
        createBuffer(sizeof(ProjViewMatrix), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, MVP_buffers[i], MVP_buffers_memory[i]);
        vkMapMemory(device, MVP_buffers_memory[i], 0, sizeof(ProjViewMatrix), 0, &MVP_buffers_memory_data[i]);
        createBuffer(sizeof(lightGlobalInfo), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, lightGlobalInfo_buffers[i], lightGlobalInfo_buffers_memory[i]);
        vkMapMemory(device, lightGlobalInfo_buffers_memory[i], 0, sizeof(lightGlobalInfo), 0, &lightGlobalInfo_buffers_memory_data[i]);
        createBuffer(sizeof(cameraInfo), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, cameraInfo_buffers[i], cameraInfo_buffers_memory[i]);
        vkMapMemory(device, cameraInfo_buffers_memory[i], 0, sizeof(cameraInfo), 0, &cameraInfo_buffers_memory_data[i]);
    }

    for (int light_source_index = 0; light_source_index < lightSourcesInfo.size(); light_source_index++) {
        createBuffer(sizeof(lightSourceInfo), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, light_source_for_render_uniform_buffers[light_source_index], light_source_for_render_uniform_buffers_memory[light_source_index]);
        vkMapMemory(device, light_source_for_render_uniform_buffers_memory[light_source_index], 0, sizeof(lightSourceInfo), 0, &light_source_for_render_uniform_buffers_memory_data[light_source_index]);
        memcpy(light_source_for_render_uniform_buffers_memory_data[light_source_index], &lightSourcesInfo[light_source_index], sizeof(lightSourceInfo));//rewrite
    }

    createBuffer(sizeof(lightSourceInfo) * lightSourcesInfo.size(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, light_source_uniform_buffer, light_source_uniform_buffer_memory);
    vkMapMemory(device, light_source_uniform_buffer_memory, 0, sizeof(lightSourceInfo) * lightSourcesInfo.size(), 0, &light_source_uniform_buffer_memory_data);
    memcpy(light_source_uniform_buffer_memory_data, lightSourcesInfo.data(), sizeof(lightSourceInfo) * lightSourcesInfo.size());

    for (int objectIndex = 0; objectIndex < twoDObjects.size(); objectIndex++) {
        for (int i = 0; i < swapChainImages.size(); i++) {
            createBuffer(sizeof(twoDObjectInfo), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, ui_uniform_buffers[i + objectIndex * swapChainImages.size()], ui_uniform_buffers_memory[i + objectIndex * swapChainImages.size()]);
            vkMapMemory(device, ui_uniform_buffers_memory[i + objectIndex * swapChainImages.size()], 0, sizeof(twoDObjectInfo), 0, &ui_uniform_buffers_memory_data[i + objectIndex * swapChainImages.size()]);
        }
    }
}

void vk::createShadowDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(all_meshes.size() * lightSourcesInfo.size() * 1);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(all_meshes.size() * lightSourcesInfo.size() * 1);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(all_meshes.size() * lightSourcesInfo.size());

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &shadowDescriptorPool) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void vk::createClickDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainFramebuffers.size() * 1);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainFramebuffers.size() * 3);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size() * all_meshes.size());

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &clickDescriptorPool) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void vk::createThreeDDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 4> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size() * 4);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImages.size() * all_meshes.size() * 9);
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[2].descriptorCount = static_cast<uint32_t>(swapChainImages.size() * all_meshes.size() * 20);
    poolSizes[3].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    poolSizes[3].descriptorCount = static_cast<uint32_t>(swapChainImages.size() * all_meshes.size() * 2);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size() * all_meshes.size());

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &threeDDescriptorPool) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void vk::createTwoDDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size() * twoDObjects.size());
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImages.size() * twoDObjects.size());

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size() * twoDObjects.size());

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &twoDDescriptorPool) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void vk::createPhysicComputeDescriptorPool() {
    std::array<VkDescriptorPoolSize, 1> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size() * 4);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &physicComputeDescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void vk::createShadowDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(lightSourcesInfo.size(), shadowDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = shadowDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(lightSourcesInfo.size());
    allocInfo.pSetLayouts = layouts.data();

    shadowDescriptorSets.resize(lightSourcesInfo.size());
    if (vkAllocateDescriptorSets(device, &allocInfo, shadowDescriptorSets.data()) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    //for (int objectIndex = 0; objectIndex < all_meshes.size(); objectIndex++) {
        for (size_t light_source_index = 0; light_source_index < lightSourcesInfo.size(); light_source_index++) {
            VkDescriptorBufferInfo lightSourceBufferInfo{};
            lightSourceBufferInfo.buffer = light_source_for_render_uniform_buffers[light_source_index];
            lightSourceBufferInfo.offset = 0;
            lightSourceBufferInfo.range = sizeof(lightSourceInfo);

            VkDescriptorBufferInfo physicalObjectsInfoDevice{};
            physicalObjectsInfoDevice.buffer = physicalObjectsInfoDevice_buffers[0];
            physicalObjectsInfoDevice.offset = 0;
            physicalObjectsInfoDevice.range = sizeof(physicalObjectInfo) * MAX_COMMANDS;

            VkDescriptorBufferInfo bonesMatricesBufferInfo{};
            bonesMatricesBufferInfo.buffer = finalBonesMatricesBuffers[0];
            bonesMatricesBufferInfo.offset = 0;
            bonesMatricesBufferInfo.range = sizeof(finalBonesMatrices);

            VkDescriptorBufferInfo objectInfoBufferInfo{};
            objectInfoBufferInfo.buffer = object_info_buffers[0];//REWRITE
            objectInfoBufferInfo.offset = 0;
            objectInfoBufferInfo.range = sizeof(index_wrapper) * MAX_COMMANDS;

            std::array<VkWriteDescriptorSet, 4> descriptorWrites{};
            int index = 0;
            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = shadowDescriptorSets[light_source_index];
            descriptorWrites[index].dstBinding = index;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[index].descriptorCount = 1;
            descriptorWrites[index].pBufferInfo = &lightSourceBufferInfo;

            index++;//1
            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = shadowDescriptorSets[light_source_index];
            descriptorWrites[index].dstBinding = index;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[index].descriptorCount = 1;
            descriptorWrites[index].pBufferInfo = &physicalObjectsInfoDevice;

            index++;//2
            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = shadowDescriptorSets[light_source_index];
            descriptorWrites[index].dstBinding = index;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[index].descriptorCount = 1;
            descriptorWrites[index].pBufferInfo = &bonesMatricesBufferInfo;

            index++;//3
            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = shadowDescriptorSets[light_source_index];
            descriptorWrites[index].dstBinding = index;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[index].descriptorCount = 1;
            descriptorWrites[index].pBufferInfo = &objectInfoBufferInfo;

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    //}
}

void vk::createClickDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size() * all_meshes.size(), clickDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = clickDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size() * all_meshes.size());
    allocInfo.pSetLayouts = layouts.data();

    clickDescriptorSets.resize(swapChainImages.size() * all_meshes.size());
    if (vkAllocateDescriptorSets(device, &allocInfo, clickDescriptorSets.data()) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        for (int objectIndex = 0; objectIndex < all_meshes.size(); objectIndex++){
            VkDescriptorBufferInfo uboBufferInfo{};
            uboBufferInfo.buffer = MVP_buffers[i];
            uboBufferInfo.offset = 0;
            uboBufferInfo.range = sizeof(ProjViewMatrix);

            VkDescriptorBufferInfo physicalObjectsInfoDevice{};
            physicalObjectsInfoDevice.buffer = physicalObjectsInfoDevice_buffers[i];
            physicalObjectsInfoDevice.offset = 0;
            physicalObjectsInfoDevice.range = sizeof(physicalObjectInfo) * MAX_COMMANDS;

            VkDescriptorBufferInfo bonesMatricesBufferInfo{};
            bonesMatricesBufferInfo.buffer = finalBonesMatricesBuffers[i];
            bonesMatricesBufferInfo.offset = 0;
            bonesMatricesBufferInfo.range = sizeof(finalBonesMatrices);

            VkDescriptorBufferInfo objectInfoBufferInfo{};
            objectInfoBufferInfo.buffer = object_info_buffers[i];//REWRITE
            objectInfoBufferInfo.offset = 0;
            objectInfoBufferInfo.range = sizeof(index_wrapper) * MAX_COMMANDS;

            std::array<VkWriteDescriptorSet, 4> descriptorWrites{};
            int index = 0;
            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = clickDescriptorSets[objectIndex + i * all_meshes.size()];
            descriptorWrites[index].dstBinding = index;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[index].descriptorCount = 1;
            descriptorWrites[index].pBufferInfo = &uboBufferInfo;

            index++;//1
            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = clickDescriptorSets[objectIndex + i * all_meshes.size()];
            descriptorWrites[index].dstBinding = index;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[index].descriptorCount = 1;
            descriptorWrites[index].pBufferInfo = &physicalObjectsInfoDevice;

            index++;//2
            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = clickDescriptorSets[objectIndex + i * all_meshes.size()];
            descriptorWrites[index].dstBinding = index;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[index].descriptorCount = 1;
            descriptorWrites[index].pBufferInfo = &bonesMatricesBufferInfo;

            index++;//3
            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = clickDescriptorSets[objectIndex + i * all_meshes.size()];
            descriptorWrites[index].dstBinding = index;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[index].descriptorCount = 1;
            descriptorWrites[index].pBufferInfo = &objectInfoBufferInfo;

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }
}

void vk::createThreeDDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size() * all_meshes.size(), threeDDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = threeDDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size() * all_meshes.size());
    allocInfo.pSetLayouts = layouts.data();

    threeDDescriptorSets.resize(swapChainImages.size() * all_meshes.size());
    if (vkAllocateDescriptorSets(device, &allocInfo, threeDDescriptorSets.data()) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        for (int objectIndex = 0; objectIndex < all_meshes.size(); objectIndex++)
        {
            VkDescriptorBufferInfo physicalObjectsInfoDevice{};
            physicalObjectsInfoDevice.buffer = physicalObjectsInfoDevice_buffers[0];
            physicalObjectsInfoDevice.offset = 0;
            physicalObjectsInfoDevice.range = sizeof(physicalObjectInfo) * MAX_COMMANDS;

            VkDescriptorBufferInfo uboBufferInfo{};
            uboBufferInfo.buffer = MVP_buffers[i];
            uboBufferInfo.offset = 0;
            uboBufferInfo.range = sizeof(ProjViewMatrix);

            VkDescriptorBufferInfo lightGlobalInfo_buffer{};
            lightGlobalInfo_buffer.buffer = lightGlobalInfo_buffers[i];
            lightGlobalInfo_buffer.offset = 0;
            lightGlobalInfo_buffer.range = sizeof(lightGlobalInfo);

            VkDescriptorBufferInfo objectInfoBufferInfo{};
            objectInfoBufferInfo.buffer = object_info_buffers[i];//REWRITE
            objectInfoBufferInfo.offset = 0;
            objectInfoBufferInfo.range = sizeof(index_wrapper) * MAX_COMMANDS;

            VkDescriptorBufferInfo bonesMatricesBufferInfo{};
            bonesMatricesBufferInfo.buffer = finalBonesMatricesBuffers[i];
            bonesMatricesBufferInfo.offset = 0;
            bonesMatricesBufferInfo.range = sizeof(finalBonesMatrices);

            VkDescriptorBufferInfo lightSourceBufferInfo{};
            lightSourceBufferInfo.buffer = light_source_uniform_buffer;
            lightSourceBufferInfo.offset = 0;
            lightSourceBufferInfo.range = sizeof(lightSourceInfo) * lightSourcesInfo.size();

            std::vector<VkDescriptorImageInfo> imagesInfo;
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = textureImageView[all_meshes[objectIndex]->textures.id];
            imageInfo.sampler = textureSampler[all_meshes[objectIndex]->textures.id];
            imagesInfo.push_back(imageInfo);
            if(all_meshes[objectIndex]->normal_maps.id >= 0){
                imageInfo.imageView = normalMapsView[all_meshes[objectIndex]->normal_maps.id];
                imagesInfo.push_back(imageInfo);
            }
            else {
                imagesInfo.push_back(imageInfo);
            }

            VkDescriptorBufferInfo cameraInfoBufferInfo{};
            cameraInfoBufferInfo.buffer = cameraInfo_buffers[i];
            cameraInfoBufferInfo.offset = 0;
            cameraInfoBufferInfo.range = sizeof(cameraInfo);

            VkDescriptorImageInfo shadowMapBufferInfo{};
            shadowMapBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            shadowMapBufferInfo.imageView = shadowDepthImageViewArray;
            shadowMapBufferInfo.sampler = shadowDepthSampler;

            unsigned int index = 0;

            std::array<VkWriteDescriptorSet, 9> descriptorWrites{};

            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = threeDDescriptorSets[objectIndex + i * all_meshes.size()];
            descriptorWrites[index].dstBinding = index;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[index].descriptorCount = 1;
            descriptorWrites[index].pBufferInfo = &physicalObjectsInfoDevice;

            index++;//1
            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = threeDDescriptorSets[objectIndex + i * all_meshes.size()];
            descriptorWrites[index].dstBinding = index;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[index].descriptorCount = 1;
            descriptorWrites[index].pBufferInfo = &uboBufferInfo;

            index++;//2
            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = threeDDescriptorSets[objectIndex + i * all_meshes.size()];
            descriptorWrites[index].dstBinding = index;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[index].descriptorCount = 1;
            descriptorWrites[index].pBufferInfo = &objectInfoBufferInfo;

            index++;//3
            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = threeDDescriptorSets[objectIndex + i * all_meshes.size()];
            descriptorWrites[index].dstBinding = index;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[index].descriptorCount = 1;
            descriptorWrites[index].pBufferInfo = &bonesMatricesBufferInfo;

            index++;//4
            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = threeDDescriptorSets[objectIndex + i * all_meshes.size()];
            descriptorWrites[index].dstBinding = index;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[index].descriptorCount = 1;
            descriptorWrites[index].pBufferInfo = &lightSourceBufferInfo;

            index++;//5
            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = threeDDescriptorSets[objectIndex + i * all_meshes.size()];
            descriptorWrites[index].dstBinding = index;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[index].descriptorCount = 2;
            descriptorWrites[index].pImageInfo = imagesInfo.data();

            index++;//6
            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = threeDDescriptorSets[objectIndex + i * all_meshes.size()];
            descriptorWrites[index].dstBinding = index;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[index].descriptorCount = 1;
            descriptorWrites[index].pBufferInfo = &cameraInfoBufferInfo;

            index++;//7
            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = threeDDescriptorSets[objectIndex + i * all_meshes.size()];
            descriptorWrites[index].dstBinding = index;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[index].descriptorCount = 1;
            descriptorWrites[index].pImageInfo = &shadowMapBufferInfo;

            index++;//8
            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = threeDDescriptorSets[objectIndex + i * all_meshes.size()];
            descriptorWrites[index].dstBinding = index;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[index].descriptorCount = 1;
            descriptorWrites[index].pBufferInfo = &lightGlobalInfo_buffer;

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }
}

void vk::createTwoDDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size() * twoDObjects.size(), twoDDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = twoDDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size() * twoDObjects.size());
    allocInfo.pSetLayouts = layouts.data();

    twoDDescriptorSets.resize(swapChainImages.size() * twoDObjects.size());
    if (vkAllocateDescriptorSets(device, &allocInfo, twoDDescriptorSets.data()) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for(size_t object_index = 0; object_index < twoDObjects.size(); object_index++) {
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkDescriptorBufferInfo uboBufferInfo{};
            uboBufferInfo.buffer = ui_uniform_buffers[i + object_index * swapChainImages.size()];
            uboBufferInfo.offset = 0;
            uboBufferInfo.range = sizeof(twoDObjectInfo);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = textureImageView[twoDObjects[object_index]->texIndex];
            imageInfo.sampler = UISampler;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = twoDDescriptorSets[i + object_index * swapChainImages.size()];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &uboBufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = twoDDescriptorSets[i + object_index * swapChainImages.size()];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }
}

void vk::createPhysicComputeDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), physicComputeDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = physicComputeDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
    allocInfo.pSetLayouts = layouts.data();

    physicComputeDescriptorSets.resize(swapChainImages.size());
    if (vkAllocateDescriptorSets(device, &allocInfo, physicComputeDescriptorSets.data()) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        VkDescriptorBufferInfo physicalObjectsInfoDevice{};
        physicalObjectsInfoDevice.buffer = physicalObjectsInfoDevice_buffers[0];
        physicalObjectsInfoDevice.offset = 0;
        physicalObjectsInfoDevice.range = sizeof(physicalObjectInfo) * MAX_COMMANDS;

        VkDescriptorBufferInfo physicalObjectsInfoHost{};
        physicalObjectsInfoHost.buffer = physicalObjectsInfoHost_buffers[0];
        physicalObjectsInfoHost.offset = 0;
        physicalObjectsInfoHost.range = sizeof(physicalObjectInfo) * MAX_COMMANDS;

        VkDescriptorBufferInfo objectInfoBufferInfo{};
        objectInfoBufferInfo.buffer = object_info_buffers[i];//REWRITE
        objectInfoBufferInfo.offset = 0;
        objectInfoBufferInfo.range = sizeof(index_wrapper) * MAX_COMMANDS;

        VkDescriptorBufferInfo clusterSizeBufferInfo{};
        clusterSizeBufferInfo.buffer = cluster_size_buffers[i];//REWRITE
        clusterSizeBufferInfo.offset = 0;
        clusterSizeBufferInfo.range = sizeof(index_wrapper) * MAX_COMMANDS;

        unsigned int index = 0;

        VkWriteDescriptorSet descriptorWrites[4];
        descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[index].dstSet = physicComputeDescriptorSets[i];
        descriptorWrites[index].dstBinding = index;
        descriptorWrites[index].dstArrayElement = 0;
        descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[index].descriptorCount = 1;
        descriptorWrites[index].pBufferInfo = &physicalObjectsInfoDevice;
        descriptorWrites[index].pNext = 0;

        ++index;
        descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[index].dstSet = physicComputeDescriptorSets[i];
        descriptorWrites[index].dstBinding = index;
        descriptorWrites[index].dstArrayElement = 0;
        descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[index].descriptorCount = 1;
        descriptorWrites[index].pBufferInfo = &physicalObjectsInfoHost;
        descriptorWrites[index].pNext = 0;

        ++index;
        descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[index].dstSet = physicComputeDescriptorSets[i];
        descriptorWrites[index].dstBinding = index;
        descriptorWrites[index].dstArrayElement = 0;
        descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[index].descriptorCount = 1;
        descriptorWrites[index].pBufferInfo = &objectInfoBufferInfo;
        descriptorWrites[index].pNext = 0;

        ++index;
        descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[index].dstSet = physicComputeDescriptorSets[i];
        descriptorWrites[index].dstBinding = index;
        descriptorWrites[index].dstArrayElement = 0;
        descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[index].descriptorCount = 1;
        descriptorWrites[index].pBufferInfo = &clusterSizeBufferInfo;
        descriptorWrites[index].pNext = 0;

        vkUpdateDescriptorSets(device, static_cast<uint32_t>(sizeof(descriptorWrites) / sizeof(descriptorWrites[0])), descriptorWrites, 0, nullptr);
    }
}

void vk::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void vk::createOnlyBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create buffer!");
    }
}

//returns buffers size
uint32_t vk::allocateDeviceMemory(VkMemoryPropertyFlags properties, std::vector<VkBuffer>& buffers, VkDeviceMemory& bufferMemory)
{
    VkDeviceSize bufferOffset = 0;
    VkDeviceSize bufferSize = 0;
    VkMemoryRequirements memRequirements;
    for (unsigned int bufferIndex = 0; bufferIndex < buffers.size(); bufferIndex++) {
        vkGetBufferMemoryRequirements(device, buffers[bufferIndex], &memRequirements);
        bufferSize += memRequirements.size;
    }

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = bufferSize;//maxSize * buffers.size();
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    for (unsigned int bufferIndex = 0; bufferIndex < buffers.size(); bufferIndex++) {
        vkGetBufferMemoryRequirements(device, buffers[bufferIndex], &memRequirements);
        vkBindBufferMemory(device, buffers[bufferIndex], bufferMemory, bufferOffset);//CHECK THIS, NOT SURE AB OFFSET
        bufferOffset += memRequirements.size; //maxSize;
    }

    return bufferSize;
}

uint32_t vk::allocateDeviceMemory(VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkDeviceSize bufferOffset = 0;
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
    VkDeviceSize bufferSize = memRequirements.size;

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = bufferSize;//maxSize * buffers.size();
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, buffer, bufferMemory, bufferOffset);

    return bufferSize;
}

VkCommandBuffer vk::beginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void vk::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void vk::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

uint32_t vk::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    assert(0);
    throw std::runtime_error("failed to find suitable memory type!");
}

void vk::createShadowGraphicsCommandBuffersMultiThread()
{
    shadowGraphicsCommandBuffers_multiThread.resize(p_init_storage->max_threads_available);
    for (int threadIndex = 0; threadIndex < p_init_storage->max_threads_available; threadIndex++) {
        shadowGraphicsCommandBuffers_multiThread[threadIndex].resize(lightSourcesInfo.size() / p_init_storage->max_threads_available);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPoolMultiThreads[threadIndex];
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = uint32_t(lightSourcesInfo.size() / p_init_storage->max_threads_available);

        if (vkAllocateCommandBuffers(device, &allocInfo, shadowGraphicsCommandBuffers_multiThread[threadIndex].data()) != VK_SUCCESS) {
            assert(0);
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }
}

void vk::createShadowGraphicsCommandBuffers()
{
    shadowGraphicsCommandBuffers.resize(lightSourcesInfo.size());
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = uint32_t(shadowGraphicsCommandBuffers.size());

    if (vkAllocateCommandBuffers(device, &allocInfo, shadowGraphicsCommandBuffers.data()) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void vk::createClickCommandBuffers()
{
    clickGraphicsCommandBuffers.resize(swapChainImages.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)clickGraphicsCommandBuffers.size();

    if (vkAllocateCommandBuffers(device, &allocInfo, clickGraphicsCommandBuffers.data()) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void vk::recordShadowGraphicsCommandBuffer(const VkCommandBuffer& commandBuffer, uint32_t lightSourceIndex)//add light_source_index and replace NULL
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = shadowRenderPass;
    renderPassInfo.framebuffer = shadowFramebuffer[lightSourceIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = shadowExtent;

    VkClearValue clearValues;
    clearValues.depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(1);
    renderPassInfo.pClearValues = &clearValues;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowGraphicsPipeline);
    VkDeviceSize offsets[] = { 0 };
    //shadow render
    //for (int obj_index = 0; obj_index < all_meshes.size(); obj_index++) {
    //    //vkCmdSetDepthBias(commandBuffer, depthBiasConstant, 0.0f, depthBiasSlope);
    //    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vectorThreeDVertexBuffers[obj_index], offsets);
    //    vkCmdBindIndexBuffer(commandBuffer, vectorThreeDIndexBuffers[obj_index], 0, VK_INDEX_TYPE_UINT32);
    //    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipelineLayout, 0, 1, &shadowDescriptorSets[lightSourceIndex + obj_index * lightSourcesInfo.size()], 0, nullptr);
    //    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(all_meshes[obj_index]->indices.size()), 1, 0, 0, 0);
    //}
    
    //shadow
    for (auto draw : draws) {
        for (uint32_t mesh_index = 0; mesh_index < draw->mesh_count; ++mesh_index) {
            uint32_t index = draw->firstMeshIndex + mesh_index;
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vectorThreeDVertexBuffers[index], offsets);
            vkCmdBindIndexBuffer(commandBuffer, vectorThreeDIndexBuffers[index], 0, VK_INDEX_TYPE_UINT32);
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipelineLayout, 0, 1, &shadowDescriptorSets[lightSourceIndex], 0, nullptr);
            VkDeviceSize indirect_offset = (draw->instanceIndex) * sizeof(VkDrawIndexedIndirectCommand);
            uint32_t draw_stride = sizeof(VkDrawIndexedIndirectCommand);
            vkCmdDrawIndexedIndirect(commandBuffer, drawIndexedIndirectCommand_buffers[(imageIndex % MAX_FRAMES_IN_FLIGHT)], indirect_offset, *draw->instance_count, draw_stride);
            //print_r(*draw->instance_count);
        }
        //vkCmdDrawIndexedIndirectCount();
    }

    for (auto draw_cluster : draws_cluster) {
        for (uint32_t mesh_index = 0; mesh_index < draw_cluster->mesh_count; ++mesh_index) {
            uint32_t index = draw_cluster->firstMeshIndex + mesh_index;
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vectorThreeDVertexBuffers[index], offsets);
            vkCmdBindIndexBuffer(commandBuffer, vectorThreeDIndexBuffers[index], 0, VK_INDEX_TYPE_UINT32);
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipelineLayout, 0, 1, &shadowDescriptorSets[lightSourceIndex], 0, nullptr);
            VkDeviceSize indirect_offset = (draw_cluster->instanceIndex) * sizeof(VkDrawIndexedIndirectCommand);
            uint32_t draw_stride = sizeof(VkDrawIndexedIndirectCommand);
            vkCmdDrawIndexedIndirect(commandBuffer, drawIndexedIndirectCommand_buffers[(imageIndex % MAX_FRAMES_IN_FLIGHT)], indirect_offset, *draw_cluster->instance_count, draw_stride);
        }
    }

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to record command buffer!");
    }
}

void vk::recordClickGraphicsCommandBuffer(const VkCommandBuffer& commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = clickRenderPass;
    renderPassInfo.framebuffer = clickFramebuffer[0];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainExtent;

    VkClearValue clearValues[2];
    clearValues[0].color = {0, 0, 0, 0};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(2);
    renderPassInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, clickGraphicsPipeline);
    VkDeviceSize offsets[] = { 0 };
    //shadow render
    //for (int obj_index = 0; obj_index < all_meshes.size(); obj_index++) {
    //    //vkCmdSetDepthBias(commandBuffer, depthBiasConstant, 0.0f, depthBiasSlope);
    //    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vectorThreeDVertexBuffers[obj_index], offsets);
    //    vkCmdBindIndexBuffer(commandBuffer, vectorThreeDIndexBuffers[obj_index], 0, VK_INDEX_TYPE_UINT32);
    //    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipelineLayout, 0, 1, &shadowDescriptorSets[lightSourceIndex + obj_index * lightSourcesInfo.size()], 0, nullptr);
    //    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(all_meshes[obj_index]->indices.size()), 1, 0, 0, 0);
    //}

    //shadow
    for (auto draw : draws) {
        for (uint32_t mesh_index = 0; mesh_index < draw->mesh_count; ++mesh_index) {
            uint32_t index = draw->firstMeshIndex + mesh_index;
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vectorThreeDVertexBuffers[index], offsets);
            vkCmdBindIndexBuffer(commandBuffer, vectorThreeDIndexBuffers[index], 0, VK_INDEX_TYPE_UINT32);
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, clickPipelineLayout, 0, 1, &clickDescriptorSets[imageIndex], 0, nullptr);
            VkDeviceSize indirect_offset = (draw->instanceIndex) * sizeof(VkDrawIndexedIndirectCommand);
            uint32_t draw_stride = sizeof(VkDrawIndexedIndirectCommand);
            vkCmdDrawIndexedIndirect(commandBuffer, drawIndexedIndirectCommand_buffers[(imageIndex % MAX_FRAMES_IN_FLIGHT)], indirect_offset, *draw->instance_count, draw_stride);
            //print_r(*draw->instance_count);
        }
        //vkCmdDrawIndexedIndirectCount();
    }

    for (auto draw_cluster : draws_cluster) {
        for (uint32_t mesh_index = 0; mesh_index < draw_cluster->mesh_count; ++mesh_index) {
            uint32_t index = draw_cluster->firstMeshIndex + mesh_index;
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vectorThreeDVertexBuffers[index], offsets);
            vkCmdBindIndexBuffer(commandBuffer, vectorThreeDIndexBuffers[index], 0, VK_INDEX_TYPE_UINT32);
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, clickPipelineLayout, 0, 1, &clickDescriptorSets[imageIndex], 0, nullptr);
            VkDeviceSize indirect_offset = (draw_cluster->instanceIndex) * sizeof(VkDrawIndexedIndirectCommand);
            uint32_t draw_stride = sizeof(VkDrawIndexedIndirectCommand);
            vkCmdDrawIndexedIndirect(commandBuffer, drawIndexedIndirectCommand_buffers[(imageIndex % MAX_FRAMES_IN_FLIGHT)], indirect_offset, *draw_cluster->instance_count, draw_stride);
        }
    }

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to record command buffer!");
    }
}

void vk::createGraphicsCommandBuffers()
{
    graphicsCommandBuffers.resize(swapChainImages.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)graphicsCommandBuffers.size();

    if (vkAllocateCommandBuffers(device, &allocInfo, graphicsCommandBuffers.data()) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void vk::recordGraphicsCommandBuffer(const VkCommandBuffer& commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainExtent;

    VkClearValue clearValues[2];
    clearValues[0].color = { .1f, .1f, .1f, 1.f };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(2);
    renderPassInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, threeDGraphicsPipeline);
    VkDeviceSize offsets[] = { 0 };
    //3d render
    //int counter = 0;
    //for (int obj_index = 0; obj_index < all_meshes.size(); obj_index++) {
    //    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vectorThreeDVertexBuffers[obj_index], offsets);
    //    vkCmdBindIndexBuffer(commandBuffer, vectorThreeDIndexBuffers[obj_index], 0, VK_INDEX_TYPE_UINT32);
    //    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, threeDPipelineLayout, 0, 1, &threeDDescriptorSets[obj_index], 0, nullptr);
    //    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(all_meshes[obj_index]->indices.size()), 1, 0, 0, counter);
    //    counter++;
    //}
    //int counter = 0;
    for (auto draw : draws) {
        for (uint32_t mesh_index = 0; mesh_index < draw->mesh_count; ++mesh_index) {
            uint32_t index = draw->firstMeshIndex + mesh_index;
            //if (index != 1) continue;
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vectorThreeDVertexBuffers[index], offsets);

            vkCmdBindIndexBuffer(commandBuffer, vectorThreeDIndexBuffers[index], 0, VK_INDEX_TYPE_UINT32);

            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, threeDPipelineLayout, 0, 1, &threeDDescriptorSets[index + (imageIndex % MAX_FRAMES_IN_FLIGHT) * all_meshes.size()], 0, nullptr);

            VkDeviceSize indirect_offset = (draw->instanceIndex) * sizeof(VkDrawIndexedIndirectCommand);
            uint32_t draw_stride = sizeof(VkDrawIndexedIndirectCommand);
            vkCmdDrawIndexedIndirect(commandBuffer, drawIndexedIndirectCommand_buffers[(imageIndex % MAX_FRAMES_IN_FLIGHT)], indirect_offset, *draw->instance_count, draw_stride);
            //print_r(*draw->instance_count);
            //assert(0);
            //vkCmdDrawIndexedIndirectCount();
            //counter++;
        }
    }

    for (auto draw_cluster : draws_cluster) {
        for (uint32_t mesh_index = 0; mesh_index < draw_cluster->mesh_count; ++mesh_index) {
            uint32_t index = draw_cluster->firstMeshIndex + mesh_index;
            //if (index != 1) continue;
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vectorThreeDVertexBuffers[index], offsets);

            vkCmdBindIndexBuffer(commandBuffer, vectorThreeDIndexBuffers[index], 0, VK_INDEX_TYPE_UINT32);

            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, threeDPipelineLayout, 0, 1, &threeDDescriptorSets[index + (imageIndex % MAX_FRAMES_IN_FLIGHT) * all_meshes.size()], 0, nullptr);

            VkDeviceSize indirect_offset = (draw_cluster->instanceIndex) * sizeof(VkDrawIndexedIndirectCommand);
            uint32_t draw_stride = sizeof(VkDrawIndexedIndirectCommand);
            vkCmdDrawIndexedIndirect(commandBuffer, drawIndexedIndirectCommand_buffers[(imageIndex % MAX_FRAMES_IN_FLIGHT)], indirect_offset, *draw_cluster->instance_count, draw_stride);
            //print_r(*draw->instance_count);
            //assert(0);
            //vkCmdDrawIndexedIndirectCount();
            //counter++;
        }
    }

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, twoDGraphicsPipeline);

    //2d render
    for (int obj_index = 0; obj_index < twoDObjects.size(); obj_index++) {
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vectorTwoDVertexBuffers[obj_index], &offsets[0]);

        vkCmdBindIndexBuffer(commandBuffer, vectorTwoDIndexBuffers[obj_index], 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, twoDPipelineLayout, 0, 1, &twoDDescriptorSets[(imageIndex % MAX_FRAMES_IN_FLIGHT) + obj_index * swapChainImages.size()], 0, nullptr);

        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(twoDObjects[obj_index]->indices.size()), 1, 0, 0, 0);
    }

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to record command buffer!");
    }
}

void vk::createComputeCommandBuffers(std::vector<VkCommandBuffer>* commandBuffer) {
    commandBuffer->resize(swapChainImages.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffer->size();

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffer->data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate compute command buffers!");
    }
}

void vk::recordComputeCommandBuffer(VkDescriptorSet* descriptorSet, VkCommandBuffer* commandBuffer, VkPipeline* computePipeline, VkPipelineLayout* computePipelineLayout, int x, int y, int z) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(*commandBuffer, &beginInfo) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to begin recording compute command buffer!");
    }

    vkCmdBindPipeline(*commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *computePipeline);

    vkCmdBindDescriptorSets(*commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *computePipelineLayout, 0, 1, descriptorSet, 0, nullptr);

    vkCmdDispatch(*commandBuffer, x, y, z);

    if (vkEndCommandBuffer(*commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record compute command buffer!");
    }
}

void vk::createSyncObjects()
{
    imageAvailableSemaphores.resize(swapChainImages.size());
    shadowAvailableSemaphores.resize(swapChainImages.size());
    clickAvailableSemaphores.resize(swapChainImages.size());
    computeAvailableSemaphores.resize(swapChainImages.size());

    renderFinishedSemaphores.resize(swapChainImages.size());
    computeFinishedSemaphores.resize(swapChainImages.size());
    shadowRenderFinishedSemaphores.resize(swapChainImages.size());
    clickRenderFinishedSemaphores.resize(swapChainImages.size());

    inFlightFences.resize(swapChainImages.size());
    imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);
    shadowInFlightFences.resize(swapChainImages.size(), VK_NULL_HANDLE);
    clickInFlightFences.resize(swapChainImages.size(), VK_NULL_HANDLE);
    physicComputeInFlightFences.resize(swapChainImages.size());

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &shadowAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &shadowRenderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &clickRenderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &shadowInFlightFences[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &clickInFlightFences[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
            assert(0);
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &computeAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &computeFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &physicComputeInFlightFences[i]) != VK_SUCCESS) {
            assert(0);
            throw std::runtime_error("failed to create compute synchronization objects for a frame!");
        }
        vkResetFences(device, 1, &physicComputeInFlightFences[i]);
        vkResetFences(device, 1, &shadowInFlightFences[i]);
        vkResetFences(device, 1, &clickInFlightFences[i]);
    }
}

void vk::updateUniformBuffer()//this func is for misc shit that must be rewritten
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - time_from_last_frame).count();;
    time_from_last_frame = std::chrono::high_resolution_clock::now();

    physicInfo myPhysicInfo{};
    myPhysicInfo.deltaTime = glm::vec4(deltaTime);

    memcpy(physicInfoIn_buffers_memory_data[currentFrame], &myPhysicInfo, (size_t)sizeof(physicInfo));
}

void vk::updateCameraInfoBuffers()
{
    cameraInfo current_lightInfo;
    current_lightInfo.flashlight_diffused[0] = 1;
    current_lightInfo.flashlight_diffused[1] = 1;
    current_lightInfo.cameraPos = p_camera->position;
    current_lightInfo.viewDir = p_camera->direction;
    current_lightInfo.viewPos = p_camera->position;

    memcpy(cameraInfo_buffers_memory_data[currentFrame], &current_lightInfo, sizeof(current_lightInfo));
}

void vk::updateCameraBuffer()
{
    p_camera->updateProjViewMatrix();
    memcpy(MVP_buffers_memory_data[currentFrame], ubo.data, sizeof(ProjViewMatrix));
}

void vk::updateShadowMapBuffers()
{
    for (int light_source_index = 0; light_source_index < current_light_sources; light_source_index++) {
        memcpy(light_source_for_render_uniform_buffers_memory_data[light_source_index], &lightSourcesInfo[light_source_index], sizeof(lightSourceInfo));
    }

    memcpy(light_source_uniform_buffer_memory_data, lightSourcesInfo.data(), sizeof(lightSourceInfo) * current_light_sources);

    lightGlobalInfo current_globalInfo;
    current_globalInfo.lightSourcesCount = current_light_sources;
    memcpy(lightGlobalInfo_buffers_memory_data[currentFrame], &current_globalInfo, sizeof(lightGlobalInfo));
}

void vk::updateClickBuffers()
{

}

void vk::retrievePhysicalObjectsInfo()
{
    vkInvalidateMappedMemoryRanges(device, 1, &physicalObjectsInfoHostbuffers_memory_mapped_range);
    memcpy(physicalObjectsInfo.data(), physicalObjectsInfoHost_buffers_memory_data[currentFrame], sizeof(physicalObjectInfo) * physicalObjectsInfo.size());
    memcpy(clusters_continuous.data(), cluster_size_buffers_memory_data[currentFrame], sizeof(index_wrapper) * clusters_continuous.size());
}

void vk::transferPhysicalObjectsInfo()
{
    memcpy(physicalObjectsInfoHost_buffers_memory_data[currentFrame], physicalObjectsInfo.data(), sizeof(physicalObjectInfo) * physicalObjectsInfo.size());
    memcpy(cluster_size_buffers_memory_data[currentFrame], clusters_continuous.data(), sizeof(index_wrapper) * clusters_continuous.size());
}

void vk::transferAnimationInfo()
{
    memcpy(finalBonesMatricesBuffersMemory_data[currentFrame], transforms.data(), sizeof(glm::mat4) * transforms.size());
    //print_r(sizeof(finalBonesMatrices));
    //print_b(sizeof(glm::mat4) * transforms.size());
}

void vk::transferDrawIndexedIndirectBuffer()
{
    memcpy(drawIndexedIndirectCommand_buffers_memory_data[currentFrame], drawIndexedIndirectCommand.data(), sizeof(VkDrawIndexedIndirectCommand) * drawIndexedIndirectCommand.size());
}

void vk::retrieveDrawIndexedIndirectBuffer()
{
    memcpy(drawIndexedIndirectCommand.data(), drawIndexedIndirectCommand_buffers_memory_data[currentFrame], sizeof(VkDrawIndexedIndirectCommand) * drawIndexedIndirectCommand.size());
}

//std::vector<indirectBatch> vk::compact_draws()
//{
//    std::vector<indirectBatch> draws;
//
//    indirectBatch firstDraw;
//    firstDraw.p_mesh = all_meshes[0];
//    firstDraw.firstMesh = 0;
//    firstDraw.count = 1;
//
//    draws.push_back(std::move(firstDraw));
//    for (int i = 1; i < all_meshes.size(); i++) // this is a single-thread
//    {
//        if (all_meshes[i] == draws.back().p_mesh)
//        {
//            draws.back().count++;
//        }
//        else
//        {
//            indirectBatch new_draw;
//            new_draw.p_mesh = all_meshes[i];
//            new_draw.firstMesh = i;
//            new_draw.count = 1;
//
//            draws.push_back(std::move(new_draw));
//        }
//    }
//
//    return draws;
//}

void vk::runPhysics()
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    vkResetCommandBuffer(physicComputeCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordComputeCommandBuffer(&physicComputeDescriptorSets[currentFrame], &physicComputeCommandBuffers[currentFrame], &physicComputePipeline, &physicComputePipelineLayout, physical_object_amount, 1, 1);
    //std::cout << render_object_amount << '\n';
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &physicComputeCommandBuffers[currentFrame];

    if (vkQueueSubmit(computeQueue, 1, &submitInfo, physicComputeInFlightFences[currentFrame]) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to submit physic compute command buffer!");
    };
}

void vk::waitForPhysicsFinish()
{
    vkWaitForFences(device, 1, &physicComputeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &physicComputeInFlightFences[currentFrame]);
}

void vk::prepareRender()
{
    resultOf_vkAcquireNextImageKHR = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &inFlightFences[currentFrame]);
    //retrieveDrawIndexedIndirectBuffer();
    if (draw_object_count_changed) {
        draw_object_count_changed = false;
        verts_in_render = 0;
        drawIndexedIndirectCommand.resize(render_object_amount);
        uint32_t out_of_render = 0;
        uint32_t current_index = 0;
        //std::cout << "BATCH INFO\n";
        for (decltype((*draws.begin())->PO_global_indexes->begin()) global_index_iterator; indirectBatch* draw : draws)
        {                
            draw->instanceIndex = current_index;
            global_index_iterator = draw->PO_global_indexes->begin();
            for (uint32_t instance_index = 0; instance_index < *draw->instance_count; ++instance_index, ++global_index_iterator) {
                //if (physicalObjectsInfo[*global_index_iterator].should_be_deleted) [[unlikely]] {
                //    out_of_render += draw->mesh_count;
                //    continue;
                //}
                for (uint32_t mesh_index = 0; mesh_index < draw->mesh_count; ++mesh_index) {
                    PO_continuous_indexes[current_index].index = (*global_index_iterator);
                    drawIndexedIndirectCommand[current_index].indexCount = static_cast<uint32_t>(draw->p_model->meshes[mesh_index]->indices.size());
                    drawIndexedIndirectCommand[current_index].instanceCount = 1; /*draws[i].count*/
                    drawIndexedIndirectCommand[current_index].firstIndex = 0;
                    drawIndexedIndirectCommand[current_index].firstInstance = (*global_index_iterator);//i
                    drawIndexedIndirectCommand[current_index++].vertexOffset = 0;/*sizeof(Vertex) * static_cast<uint32_t>(draws[i].p_mesh->vertices.size())*/
                    verts_in_render += draw->p_model->meshes[mesh_index]->vertices.size();
                }
            }
        }

        for (decltype((*draws.begin())->PO_global_indexes->begin()) global_index_iterator; auto draw_cluster : draws_cluster)
        {
            draw_cluster->instanceIndex = current_index;
            for (auto cluster_PO_global_indexes : *(draw_cluster->clustered_PO_global_indexes)) {
                global_index_iterator = cluster_PO_global_indexes->begin();
                while (global_index_iterator != cluster_PO_global_indexes->end()) {
                    //if (physicalObjectsInfo[*global_index_iterator].should_be_deleted) [[unlikely]] {
                    //    std::cout << *global_index_iterator << '\n';
                    //    out_of_render += draw_cluster->mesh_count;
                    //    ++global_index_iterator;
                    //    continue;
                    //}
                    for (uint32_t mesh_index = 0; mesh_index < draw_cluster->mesh_count; ++mesh_index) {
                        PO_continuous_indexes[current_index].index = (*global_index_iterator);
                        drawIndexedIndirectCommand[current_index].indexCount = static_cast<uint32_t>(draw_cluster->p_model->meshes[mesh_index]->indices.size());
                        drawIndexedIndirectCommand[current_index].instanceCount = 1; /*draws[i].count*/
                        drawIndexedIndirectCommand[current_index].firstIndex = 0;
                        drawIndexedIndirectCommand[current_index].firstInstance = (*global_index_iterator);//i
                        drawIndexedIndirectCommand[current_index++].vertexOffset = 0;/*sizeof(Vertex) * static_cast<uint32_t>(draws[i].p_mesh->vertices.size())*/
                        verts_in_render += draw_cluster->p_model->meshes[mesh_index]->vertices.size();
                        ++global_index_iterator;
                    }
                }
            }
        }
        //std::cout << "END OF BATCH INFO\n";
        memcpy(physicalObjectsInfoHost_buffers_memory_data[currentFrame], physicalObjectsInfo.data(), sizeof(physicalObjectInfo) * physicalObjectsInfo.size());
        memcpy(object_info_buffers_memory_data[currentFrame], PO_continuous_indexes.data(), sizeof(index_wrapper) * (render_object_amount - out_of_render));
        transferDrawIndexedIndirectBuffer();
    }
}

void vk::drawShadowMapsMultiThread()
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    std::function<void(int threadIndex)> func = [&](int threadIndex) {
        for (int lightSourceIndex = 0; lightSourceIndex < lightSourcesInfo.size() / p_init_storage->max_threads_available; lightSourceIndex++) {
            int true_lightSourceIndex = threadIndex * (lightSourcesInfo.size() / p_init_storage->max_threads_available);
            vkResetCommandBuffer(shadowGraphicsCommandBuffers_multiThread[threadIndex][lightSourceIndex], /*VkCommandBufferResetFlagBits*/ 0);
            recordShadowGraphicsCommandBuffer(shadowGraphicsCommandBuffers_multiThread[threadIndex][lightSourceIndex], true_lightSourceIndex);
            true_lightSourceIndex++;
        }
    };

    std::vector<std::thread> threads;
    int size = lightSourcesInfo.size();
    for (int threadIndex = 0; threadIndex < p_init_storage->max_threads_available; threadIndex++) {
        //concurrency::parallel_for(0, size, [&](int lightSourceIndex) {
        std::thread current_thread(func, threadIndex);
        threads.push_back(std::move(current_thread));
    }

    threads[0].join();
    std::vector<VkCommandBuffer> shadowGraphicsCommandBuffers(shadowGraphicsCommandBuffers_multiThread[0]);
    for (int threadIndex = 1; threadIndex < p_init_storage->max_threads_available; threadIndex++) {
        threads[threadIndex].join();
        shadowGraphicsCommandBuffers.insert(shadowGraphicsCommandBuffers.end(), shadowGraphicsCommandBuffers_multiThread[threadIndex].begin(), shadowGraphicsCommandBuffers_multiThread[threadIndex].end());
    }

    submitInfo.commandBufferCount = shadowGraphicsCommandBuffers.size();
    submitInfo.pCommandBuffers = shadowGraphicsCommandBuffers.data();
    //submitInfo.signalSemaphoreCount = 1;
    //submitInfo.pSignalSemaphores = &shadowAvailableSemaphores[currentFrame];

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, shadowInFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit shadow mt command buffer!");
    };

}

void vk::drawShadowMaps()
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    for (int lightSourceIndex = 0; lightSourceIndex < current_light_sources; ++lightSourceIndex) {
        vkResetCommandBuffer(shadowGraphicsCommandBuffers[lightSourceIndex], /*VkCommandBufferResetFlagBits*/ 0);
        recordShadowGraphicsCommandBuffer(shadowGraphicsCommandBuffers[lightSourceIndex], lightSourceIndex);
    }

    submitInfo.commandBufferCount = current_light_sources;
    submitInfo.pCommandBuffers = shadowGraphicsCommandBuffers.data();
    //submitInfo.signalSemaphoreCount = 1;
    //submitInfo.pSignalSemaphores = &shadowAvailableSemaphores[currentFrame];

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, shadowInFlightFences[currentFrame]) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to submit shadow command buffer!");
    };
}

void vk::saveRenderToFile()
{
    VkDeviceSize size = mWindow->WIDTH * mWindow->HEIGHT * 4;
    VkBuffer dstBuffer;
    VkDeviceMemory dstMemory;

    createBuffer(
        size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        dstBuffer,
        dstMemory);
    VkCommandBuffer copyCmd = beginSingleTimeCommands();

    // depth format -> VK_FORMAT_D32_SFLOAT_S8_UINT
    VkBufferImageCopy region = {};
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = VkOffset3D{ 0, 0, 0 };
    region.imageExtent = VkExtent3D{ swapChainExtent.width, swapChainExtent.height, 1 };

    VkImageBlit screen_region{};
    screen_region.dstSubresource.layerCount = 1;
    screen_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    screen_region.srcSubresource.layerCount = 1;
    screen_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    screen_region.dstOffsets[1] = VkOffset3D{ static_cast<int>(swapChainExtent.width), static_cast<int>(swapChainExtent.height), 1 };
    screen_region.srcOffsets[1] = VkOffset3D{ static_cast<int>(swapChainExtent.width), static_cast<int>(swapChainExtent.height), 1 };

    VkImage srcImage = swapChainImages[0];
    VkImageMemoryBarrier src_barrier{};
    src_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    src_barrier.image = srcImage;
    src_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    src_barrier.oldLayout =  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    src_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    src_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    src_barrier.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, {}, 0, nullptr, 0, nullptr, 1, &src_barrier);

    VkImage blitedImage;
    VkDeviceMemory blitedImageMemory;
    createImage(swapChainExtent.width, swapChainExtent.height, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, blitedImage, blitedImageMemory, VK_IMAGE_TYPE_2D, 1);
    VkImageMemoryBarrier undefined_to_dst_barrier{};
    undefined_to_dst_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    undefined_to_dst_barrier.image = blitedImage;
    undefined_to_dst_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    undefined_to_dst_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    undefined_to_dst_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    undefined_to_dst_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    undefined_to_dst_barrier.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, {}, 0, nullptr, 0, nullptr, 1, &undefined_to_dst_barrier);

    vkCmdBlitImage(copyCmd, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, blitedImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &screen_region, VK_FILTER_NEAREST);
    
    VkImageMemoryBarrier dst_barrier{};
    dst_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    dst_barrier.image = blitedImage;
    dst_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    dst_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    dst_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    dst_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    dst_barrier.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    
    vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, {}, 0, nullptr, 0, nullptr, 1, &dst_barrier);
    //VkImage dstImage;
    //VkDeviceMemory dstImageMemory;
    //createImage(swapChainExtent.width, swapChainExtent.height, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, dstImage, dstImageMemory, VK_IMAGE_TYPE_2D, 1);
    //vkCmdBlitImage(copyCmd, swapChainImages[0], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, blitedImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &screen_region, VK_FILTER_NEAREST);
    
    vkCmdCopyImageToBuffer(
        copyCmd,
        blitedImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        dstBuffer,
        1,
        &region
    );

    VkImageMemoryBarrier back_to_origin_barrier{};
    back_to_origin_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    back_to_origin_barrier.image = srcImage;
    back_to_origin_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    back_to_origin_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    back_to_origin_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    back_to_origin_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    back_to_origin_barrier.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, {}, 0, nullptr, 0, nullptr, 1, &back_to_origin_barrier);

    endSingleTimeCommands(copyCmd);

    // Map image memory so we can start copying from it
    void* data;
    vkMapMemory(device, dstMemory, 0, size, 0, &data);
    const char* name = "render.bmp";
    stbi_write_bmp(name, swapChainExtent.width, swapChainExtent.height, 4, const_cast<const void*>(data));

    // Clean up resources
    vkDestroyImage(device, blitedImage, nullptr);
    vkUnmapMemory(device, dstMemory);
    vkFreeMemory(device, dstMemory, nullptr);
    vkDestroyBuffer(device, dstBuffer, nullptr);
    vkFreeMemory(device, blitedImageMemory, nullptr);
}

void vk::waitForShadowMapRenderFinish()
{
    vkWaitForFences(device, 1, &shadowInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &shadowInFlightFences[currentFrame]);
}

void vk::drawClickMap()
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    vkResetCommandBuffer(clickGraphicsCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordClickGraphicsCommandBuffer(clickGraphicsCommandBuffers[currentFrame], imageIndex);

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &clickGraphicsCommandBuffers[currentFrame];

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, clickInFlightFences[currentFrame]) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to submit click command buffer!");
    };
}

void vk::waitForClickMapRenderFinish()
{
    vkWaitForFences(device, 1, &clickInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &clickInFlightFences[currentFrame]);
}

void vk::drawFrame()
{
    if (resultOf_vkAcquireNextImageKHR == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        recreateClickSwapChain();
        return;
    }
    else if (resultOf_vkAcquireNextImageKHR != VK_SUCCESS && resultOf_vkAcquireNextImageKHR != VK_SUBOPTIMAL_KHR) {
        assert(0);
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    if (screenshot_should_be_saved) {
        saveRenderToFile();
        screenshot_should_be_saved = false;
    }

    vkResetCommandBuffer(graphicsCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordGraphicsCommandBuffer(graphicsCommandBuffers[currentFrame], imageIndex);

    //if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
    //    vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX); // idk for what is that but i think for async rendering
    //}
    //imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    VkSemaphore* signalSemaphores = &renderFinishedSemaphores[currentFrame];
    submitInfo.signalSemaphoreCount = 1;

    //if (calc_is_performing) {
    //    calc_is_performing = false;
    //    waitSemaphores.push_back(computeAvailableSemaphores[currentFrame]);
    //    signalSemaphores.push_back(computeFinishedSemaphores[currentFrame]);
    //    waitStages.push_back(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
    //    submitInfo.waitSemaphoreCount++;
    //    submitInfo.signalSemaphoreCount++;
    //}
    //if (shadow_is_casting) {
    //    shadow_is_casting = false;
    //    waitSemaphores.push_back(shadowAvailableSemaphores[currentFrame]);
    //    signalSemaphores.push_back(shadowRenderFinishedSemaphores[currentFrame]);
    //    waitStages.push_back(/*VK_PIPELINE_STAGE_VERTEX_INPUT_BIT */VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    //    submitInfo.waitSemaphoreCount++;
    //    submitInfo.signalSemaphoreCount++;
    //}

    submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &graphicsCommandBuffers[currentFrame];

    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = submitInfo.signalSemaphoreCount;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    resultOf_vkQueuePresentKHR = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (resultOf_vkQueuePresentKHR == VK_ERROR_OUT_OF_DATE_KHR || resultOf_vkQueuePresentKHR == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        glfwGetFramebufferSize(window, &mWindow->WIDTH, &mWindow->HEIGHT);
        for (button* m_button : buttons) {
            m_button->change_aspect();
            m_button->calculate_pixel_pos();
        }
        recreateAspectDependentObjects();
        recreateSwapChain();
        recreateClickSwapChain();
        p_camera->updateProjectionAspect(static_cast<float>(swapChainExtent.height) / swapChainExtent.width);
    }
    else if (resultOf_vkQueuePresentKHR != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to present swap chain image!");
    }

    //currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

VkShaderModule vk::createShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        assert(0);
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

VkSurfaceFormatKHR vk::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR vk::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    //for (const auto& availablePresentMode : availablePresentModes) {
    //    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
    //        return availablePresentMode;
    //    }
    //}

    return VK_PRESENT_MODE_FIFO_KHR/*VK_PRESENT_MODE_IMMEDIATE_KHR*//*VK_PRESENT_MODE_MAILBOX_KHR*/;
}

VkExtent2D vk::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

vk::SwapChainSupportDetails vk::querySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

bool vk::isDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

bool vk::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

vk::QueueFamilyIndices vk::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                indices.graphicsAndComputeFamily = i;
            }
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

std::vector<const char*> vk::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

bool vk::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::vector<char> vk::readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        assert(0);
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VKAPI_ATTR VkBool32 VKAPI_CALL vk::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    std::string msg(pCallbackData->pMessage);
    std::string epic_game = "Epic Games";
    if (msg.find(epic_game) == std::string::npos) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    }

    return VK_FALSE;
}
