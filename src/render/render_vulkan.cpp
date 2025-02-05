#include "render_vulkan.h"

#include "../zayn.h"
#include "../globals.h"
#include <vector>
#include <set>
#include <algorithm>
#include <fstream>
#include "../include/stb_image.h"
#include "../include/tiny_obj_loader.h"

#include <map>
#include <unordered_map>
#include <glm/gtx/hash.hpp>


#ifdef NDEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = true;
#endif


const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

#if WINDOWS
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
#elif __APPLE__
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset" };
#endif

std::string getShaderPath(const std::string& filename) {
#ifdef WINDOWS
    return "src/render/shaders/" + filename;
    //return "/src/renderer/shaders/" + filename;
#else
    return "/Users/socki/dev/zayn2/src/renderer/shaders/" + filename;
#endif
}


std::string getTexturePath(const std::string& filename) {
#ifdef WINDOWS
    return "src/render/textures/" + filename;
    //return "C:/dev_c/zayn/models/textures/" + filename;
#else
    return "/Users/socki/dev/zayn2/models/textures/" + filename;
#endif
}

std::string getModelPath(const std::string& filename) {
#ifdef _WIN32
    return "src/render/models/" + filename;
#else
    return "/Users/socki/dev/zayn2/models/" + filename;
#endif
}


bool checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    VkLayerProperties* availableLayers = (VkLayerProperties*)malloc(layerCount * sizeof(VkLayerProperties));

    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    for (const char* layerName : validationLayers)
    {
        bool layerFound = false;

        for (int j = 0; j < layerCount; j++)
        {
            if (strcmp(layerName, availableLayers[j].layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            free(availableLayers);
            return false;
        }
    }
    free(availableLayers);
    return true;
}

std::vector<const char*> getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#ifdef __APPLE__
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
    }
#ifdef __APPLE__
    uint32_t instanceExtensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    std::vector<VkExtensionProperties> availableInstanceExtensions(instanceExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, availableInstanceExtensions.data());
    for (const auto& extension : availableInstanceExtensions)
    {
        if (strcmp(extension.extensionName, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) == 0)
        {
            extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
            break;
        }
    }
#endif
    return extensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional
}

void PrintGPUName(VkInstance instance)
{
    // Enumerate physical devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // Print the name of each physical device
    for (const auto& device : devices)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        std::cout << "GPU: " << deviceProperties.deviceName << std::endl;
    }
}
 

void CreateVKInstance(ZaynMemory* zaynMem)
{
    if (enableValidationLayers && !checkValidationLayerSupport())
    {
        std::cerr << "Validation layers requested, but not available!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Zayn";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Zayn Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

       populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &zaynMem->vulkan.vkInstance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
    PrintGPUName(zaynMem->vulkan.vkInstance);
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void SetupDebugMessenger(ZaynMemory* zaynMem)
{
    if (!enableValidationLayers)
    {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    // populateDebugMessengerCreateInfo(createInfo);
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional


    VkAllocationCallbacks pAllocator = {};


#ifdef __APPLE__
    if (CreateDebugUtilsMessengerEXT(Zayn->vkInstance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), &pAllocator, &Zayn->vkDebugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
#else
    if (CreateDebugUtilsMessengerEXT(zaynMem->vulkan.vkInstance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr, &zaynMem->vulkan.vkDebugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
#endif

}

void CreateSurface(ZaynMemory* zaynMem)
{
    if (glfwCreateWindowSurface(zaynMem->vulkan.vkInstance, zaynMem->window, nullptr, &zaynMem->vulkan.vkSurface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, ZaynMemory* zaynMem)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &zaynMem->vulkan.vkQueueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(zaynMem->vulkan.vkQueueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &zaynMem->vulkan.vkQueueFamilyCount, queueFamilies.data());

    int i{};
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, zaynMem->vulkan.vkSurface, &presentSupport);

        if (presentSupport)
        {
            indices.presentFamily = i;
        }

        if (indices.isComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}

bool checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, ZaynMemory* zaynMem)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, zaynMem->vulkan.vkSurface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, zaynMem->vulkan.vkSurface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, zaynMem->vulkan.vkSurface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, zaynMem->vulkan.vkSurface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, zaynMem->vulkan.vkSurface, &presentModeCount, details.presentModes.data());
    }

    return details;
}


bool isDeviceSuitable(VkPhysicalDevice device, ZaynMemory* zaynMem)
{
    QueueFamilyIndices indices = findQueueFamilies(device, zaynMem);
    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, zaynMem);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    //VkPhysicalDeviceFeatures supportedFeatures;
    //vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    //return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

void PickPhysicalDevice(ZaynMemory* zaynMem)
{

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(zaynMem->vulkan.vkInstance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(zaynMem->vulkan.vkInstance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        // printDeviceExtensions(device);
        if (isDeviceSuitable(device, zaynMem))
        {
            zaynMem->vulkan.vkPhysicalDevice = device;
            break;
        }
    }

    if (zaynMem->vulkan.vkPhysicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void CreateLogicalDevice(ZaynMemory* zaynMem)
{
    QueueFamilyIndices indices = findQueueFamilies(zaynMem->vulkan.vkPhysicalDevice, zaynMem);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.sampleRateShading = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(zaynMem->vulkan.vkPhysicalDevice, &createInfo, nullptr, &zaynMem->vulkan.vkDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(zaynMem->vulkan.vkDevice, indices.graphicsFamily.value(), 0, &zaynMem->vulkan.vkGraphicsQueue);
    vkGetDeviceQueue(zaynMem->vulkan.vkDevice, indices.presentFamily.value(), 0, &zaynMem->vulkan.vkPresentQueue);
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    std::cout << "Present mode: V-Sync?" << std::endl;

    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            std::cout << "Present mode: Mailbox" << std::endl;
            return availablePresentMode;
        }
        // UNCOMMENT BELOW IF YOU WANT TO NOT HAVE VSYNC ON
       /* else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
        {
            std::cout << "Present mode: Immediate (No VSync)" << std::endl;
            return availablePresentMode;
        }*/
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, ZaynMemory* zaynMem)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(zaynMem->window, &width, &height);

        VkExtent2D actualExtent =
        {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height) 
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}




VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, ZaynMemory* zaynMem)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(zaynMem->vulkan.vkPhysicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

VkFormat FindDepthFormat(ZaynMemory* zaynMem)
{
    return FindSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, zaynMem);
}


void CreateRenderPass(ZaynMemory* zaynMem)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = zaynMem->vulkan.vkSwapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;


    // changed for imgui
#if IMGUI
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
#else
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
#endif



    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = FindDepthFormat(zaynMem);
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

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(zaynMem->vulkan.vkDevice, &renderPassInfo, nullptr, &zaynMem->vulkan.vkRenderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
    else {
        std::cout << "Render pass created successfully." << std::endl;
    }

#if IMGUI


    // Attachment
    colorAttachment = {};
    colorAttachment.format = Zayn->vkSwapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // No MSAA
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Color VkAttachmentReference our render pass needs.
    colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // subpass
    subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // synchronization and dependency
    dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0; // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments = &colorAttachment;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;
    if (vkCreateRenderPass(Zayn->vkDevice, &info, nullptr, &Zayn->imGuiRenderPass) !=
        VK_SUCCESS) {
        throw std::runtime_error("Could not create Dear ImGui's render pass");
    }

#endif
}

void CreateSwapChain(ZaynMemory* zaynMem)
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(zaynMem->vulkan.vkPhysicalDevice, zaynMem);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, zaynMem);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = zaynMem->vulkan.vkSurface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(zaynMem->vulkan.vkPhysicalDevice, zaynMem);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
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

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(zaynMem->vulkan.vkDevice, &createInfo, nullptr, &zaynMem->vulkan.vkSwapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(zaynMem->vulkan.vkDevice, zaynMem->vulkan.vkSwapChain, &imageCount, nullptr);
    zaynMem->vulkan.vkSwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(zaynMem->vulkan.vkDevice, zaynMem->vulkan.vkSwapChain, &imageCount, zaynMem->vulkan.vkSwapChainImages.data());

    zaynMem->vulkan.vkSwapChainImageFormat = surfaceFormat.format;
    zaynMem->vulkan.vkSwapChainExtent = extent;
}

void CreateImageViews(ZaynMemory* zaynMem)
{
    zaynMem->vulkan.vkSwapChainImageViews.resize(zaynMem->vulkan.vkSwapChainImages.size());

    for (size_t i = 0; i < zaynMem->vulkan.vkSwapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = zaynMem->vulkan.vkSwapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = zaynMem->vulkan.vkSwapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(zaynMem->vulkan.vkDevice, &createInfo, nullptr, &zaynMem->vulkan.vkSwapChainImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

VkShaderModule createShaderModule(const std::vector<char>& code, ZaynMemory* zaynMem) 
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(zaynMem->vulkan.vkDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

static std::vector<char> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}


void CreateCommandPool(ZaynMemory* zaynMem)
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(zaynMem->vulkan.vkPhysicalDevice, Zayn);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    // poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(zaynMem->vulkan.vkDevice, &poolInfo, nullptr, &zaynMem->vulkan.vkCommandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }

#if IMGUI

    if (vkCreateCommandPool(Zayn->vkDevice, &poolInfo, nullptr, &Zayn->imGuiCommandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }

#endif
}

uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, ZaynMemory* zaynMem)
{
    // VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(zaynMem->vulkan.vkPhysicalDevice, &zaynMem->vulkan.vkMemProperties);
    for (uint32_t i = 0; i < zaynMem->vulkan.vkMemProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (zaynMem->vulkan.vkMemProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type!");
}

void CreateImage(uint32_t width,
    uint32_t height,
    uint32_t mipLevels,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImage& image,
    VkDeviceMemory& imageMemory,
    ZaynMemory* zaynMem)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(zaynMem->vulkan.vkDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(zaynMem->vulkan.vkDevice, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties, zaynMem);

    if (vkAllocateMemory(zaynMem->vulkan.vkDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(zaynMem->vulkan.vkDevice, image, imageMemory, 0);
}

VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, ZaynMemory* zaynMem)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(zaynMem->vulkan.vkDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}


void CreateDepthResources(ZaynMemory* zaynMem)
{
    VkFormat depthFormat = FindDepthFormat(zaynMem);

    CreateImage(zaynMem->vulkan.vkSwapChainExtent.width, zaynMem->vulkan.vkSwapChainExtent.height, 1, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, zaynMem->vulkan.vkDepthImage, zaynMem->vulkan.vkDepthImageMemory, zaynMem);
    zaynMem->vulkan.vkDepthImageView = CreateImageView(zaynMem->vulkan.vkDepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, zaynMem);
}


void CreateFrameBuffers(ZaynMemory* zaynMem)
{
    zaynMem->vulkan.vkSwapChainFramebuffers.resize(zaynMem->vulkan.vkSwapChainImageViews.size());

#if IMGUI 

    Zayn->imGuiFrameBuffers.resize(Zayn->vkSwapChainImageViews.size());

#endif

    for (size_t i = 0; i < zaynMem->vulkan.vkSwapChainImageViews.size(); i++)
    {
        std::array<VkImageView, 2> attachments = {
            zaynMem->vulkan.vkSwapChainImageViews[i],
            zaynMem->vulkan.vkDepthImageView };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = zaynMem->vulkan.vkRenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width  = zaynMem->vulkan.vkSwapChainExtent.width;
        framebufferInfo.height = zaynMem->vulkan.vkSwapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(zaynMem->vulkan.vkDevice, &framebufferInfo, nullptr, &zaynMem->vulkan.vkSwapChainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }

#if IMGUI

        // Imgui framebuffer
        VkImageView imgui_attachment[1];
        framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = Zayn->imGuiRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = imgui_attachment;
        framebufferInfo.width = Zayn->vkSwapChainExtent.width;
        framebufferInfo.height = Zayn->vkSwapChainExtent.height;
        framebufferInfo.layers = 1;
        imgui_attachment[0] = Zayn->vkSwapChainImageViews[i];
        if (vkCreateFramebuffer(Zayn->vkDevice, &framebufferInfo, nullptr,
            &Zayn->imGuiFrameBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create ImGui framebuffer!");
        }

#endif
    }
}


void CreateCommandBuffers(ZaynMemory* zaynMem)
{
    // Zayn->vkCommandBuffers = (VkCommandBuffer *)malloc(sizeof(VkCommandBuffer) * MAX_FRAMES_IN_FLIGHT);
    zaynMem->vulkan.vkCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = zaynMem->vulkan.vkCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    if (vkAllocateCommandBuffers(zaynMem->vulkan.vkDevice, &allocInfo, zaynMem->vulkan.vkCommandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
#if IMGUI
    // ImGui Command Buffer
    // Zayn->imGuiCommandBuffers.resize(    
    //     Zayn->vkSwapChainImageViews.size()); // or MAX_FRAMES_IN_FLIGHT?
    Zayn->imGuiCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    // create command buffers
    allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = Zayn->imGuiCommandPool;
    allocInfo.commandBufferCount =
        static_cast<uint32_t>(Zayn->imGuiCommandBuffers.size());
    vkAllocateCommandBuffers(Zayn->vkDevice, &allocInfo, Zayn->imGuiCommandBuffers.data());
#endif
}

void CreateSyncObjects(ZaynMemory* zaynMem)
{
    zaynMem->vulkan.vkImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    zaynMem->vulkan.vkRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    zaynMem->vulkan.vkInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    zaynMem->vulkan.vkImagesInFlight.resize(zaynMem->vulkan.vkSwapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;


    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(zaynMem->vulkan.vkDevice, &semaphoreInfo, nullptr, &zaynMem->vulkan.vkImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(zaynMem->vulkan.vkDevice, &semaphoreInfo, nullptr, &zaynMem->vulkan.vkRenderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(zaynMem->vulkan.vkDevice, &fenceInfo, nullptr, &zaynMem->vulkan.vkInFlightFences[i]) != VK_SUCCESS)
        {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}



void CreateDescriptorSetLayout(VkDescriptorSetLayout* descriptorSetLayout, bool hasImage, ZaynMemory* zaynMem)
{
    std::vector<VkDescriptorSetLayoutBinding> bindings = {};
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    bindings.push_back(uboLayoutBinding);

    if (hasImage)
    {
        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings.push_back(samplerLayoutBinding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(zaynMem->vulkan.vkDevice, &layoutInfo, nullptr, descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void CreateGraphicsPipeline(ZaynMemory* zaynMem, VkPipeline* pipeline, const std::string& vertShaderFilePath, const std::string& fragShaderFilePath, std::vector<VkPushConstantRange> pushConstants, VkDescriptorSetLayout* descriptorSetLayout, VkPipelineLayout* pipelineLayout)
{
   auto vertShaderCode = readFile(vertShaderFilePath);
   auto fragShaderCode = readFile(fragShaderFilePath);
  
   VkShaderModule vertShaderModule = createShaderModule(vertShaderCode, zaynMem);
   VkShaderModule fragShaderModule = createShaderModule(fragShaderCode, zaynMem);
  
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
  
   VkPipelineViewportStateCreateInfo viewportState = {};
   viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
   viewportState.viewportCount = 1;
   viewportState.scissorCount = 1;
  
   VkPipelineRasterizationStateCreateInfo rasterizer = {};
   rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
   rasterizer.depthClampEnable = VK_FALSE;
   rasterizer.rasterizerDiscardEnable = VK_FALSE;
   rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
   rasterizer.lineWidth = 1.0f;
   rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
   rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
   rasterizer.depthBiasEnable = VK_FALSE;
  
   VkPipelineMultisampleStateCreateInfo multisampling = {};
   multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
   multisampling.sampleShadingEnable = VK_FALSE;
   multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  
   VkPipelineDepthStencilStateCreateInfo depthStencil = {};
   depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
   depthStencil.depthTestEnable = VK_TRUE;
   depthStencil.depthWriteEnable = VK_TRUE;
   depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
   depthStencil.depthBoundsTestEnable = VK_FALSE;
   depthStencil.minDepthBounds = 0.0f; // Optional
   depthStencil.maxDepthBounds = 1.0f; // Optional
  
   depthStencil.stencilTestEnable = VK_FALSE;
   depthStencil.front = {}; // Optional
   depthStencil.back = {};  // Optional
  
   VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
   colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
   colorBlendAttachment.blendEnable = VK_FALSE;
  
   VkPipelineColorBlendStateCreateInfo colorBlending = {};
   colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
   colorBlending.logicOpEnable = VK_FALSE;
   colorBlending.logicOp = VK_LOGIC_OP_COPY;
   colorBlending.attachmentCount = 1;
   colorBlending.pAttachments = &colorBlendAttachment;
   colorBlending.blendConstants[0] = 0.0f;
   colorBlending.blendConstants[1] = 0.0f;
   colorBlending.blendConstants[2] = 0.0f;
   colorBlending.blendConstants[3] = 0.0f;
  
   std::vector<VkDynamicState> dynamicStates = {
       VK_DYNAMIC_STATE_VIEWPORT,
       VK_DYNAMIC_STATE_SCISSOR };
  
   VkPipelineDynamicStateCreateInfo dynamicState = {};
   dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
   dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
   dynamicState.pDynamicStates = dynamicStates.data();
  
   VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
   pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
   pipelineLayoutInfo.setLayoutCount = 1;
   pipelineLayoutInfo.pSetLayouts = descriptorSetLayout;
  
   VkPushConstantRange pushConstantRange = {};
   pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
   pushConstantRange.offset = 0;
   pushConstantRange.size = sizeof(ModelPushConstant);
   pipelineLayoutInfo.pushConstantRangeCount = 1;
   pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
  
   if (vkCreatePipelineLayout(zaynMem->vulkan.vkDevice, &pipelineLayoutInfo, nullptr, pipelineLayout) != VK_SUCCESS)
   {
       throw std::runtime_error("failed to create pipeline layout!");
   }
  
   VkGraphicsPipelineCreateInfo pipelineInfo = {};
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
   pipelineInfo.pDynamicState = &dynamicState;
   pipelineInfo.layout = *pipelineLayout;
   pipelineInfo.renderPass = zaynMem->vulkan.vkRenderPass;
   pipelineInfo.subpass = 0;
   pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  
   if (vkCreateGraphicsPipelines(zaynMem->vulkan.vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, pipeline) != VK_SUCCESS)
   {
       throw std::runtime_error("failed to create graphics pipeline!");
   }
  
   vkDestroyShaderModule(zaynMem->vulkan.vkDevice, fragShaderModule, nullptr);
   vkDestroyShaderModule(zaynMem->vulkan.vkDevice, vertShaderModule, nullptr);
}

void CreateGraphicsPipeline_Instances(ZaynMemory* zaynMem, VkPipeline* pipeline, const std::string& vertShaderFilePath, const std::string& fragShaderFilePath, std::vector<VkPushConstantRange> pushConstants, VkDescriptorSetLayout* descriptorSetLayout, VkPipelineLayout* pipelineLayout)
{
    auto vertShaderCode = readFile(vertShaderFilePath);
    auto fragShaderCode = readFile(fragShaderFilePath);

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode, zaynMem);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode, zaynMem);

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

    auto bindingDescriptions = Vertex::getBindingDescriptions_instanced();
    auto attributeDescriptions = Vertex::getAttributeDescriptions_instanced();

    vertexInputInfo.vertexBindingDescriptionCount = 2;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional

    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {};  // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayout;

    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(ModelPushConstant);
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(zaynMem->vulkan.vkDevice, &pipelineLayoutInfo, nullptr, pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
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
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = *pipelineLayout;
    pipelineInfo.renderPass = zaynMem->vulkan.vkRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(zaynMem->vulkan.vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(zaynMem->vulkan.vkDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(zaynMem->vulkan.vkDevice, vertShaderModule, nullptr);
}

VkCommandBuffer BeginSingleTimeCommands(ZaynMemory* zaynMem)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = zaynMem->vulkan.vkCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(zaynMem->vulkan.vkDevice, &allocInfo, &commandBuffer);

    //     #ifdef IMGUI
    //     Zayn->imGuiCommandBuffers.resize(
    //         Zayn->vkSwapChainImageViews.size());

    //         allocInfo = {};
    //         allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    //         allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    //         allocInfo.commandPool = Zayn->imGuiCommandPool;
    //         allocInfo.commandBufferCount =
    //             static_cast<uint32_t>(Zayn->imGuiCommandBuffers.size());
    //         vkAllocateCommandBuffers(Zayn->vkDevice, &allocInfo, Zayn->imGuiCommandBuffers.data());

    // #endif

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void EndSingleTimeCommands(ZaynMemory* zaynMem, VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(zaynMem->vulkan.vkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(zaynMem->vulkan.vkGraphicsQueue);

    vkFreeCommandBuffers(zaynMem->vulkan.vkDevice, zaynMem->vulkan.vkCommandPool, 1, &commandBuffer);
}

void createBuffer(ZaynMemory* zaynMem, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage | VK_BUFFER_CREATE_SPARSE_BINDING_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(zaynMem->vulkan.vkDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(zaynMem->vulkan.vkDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties, zaynMem);

    if (vkAllocateMemory(zaynMem->vulkan.vkDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(zaynMem->vulkan.vkDevice, buffer, bufferMemory, 0);
}

void TransitionImageLayout(ZaynMemory* zaynMem, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(zaynMem);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    EndSingleTimeCommands(zaynMem, commandBuffer);
}

void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, ZaynMemory* zaynMem)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(zaynMem);

    VkBufferImageCopy region = {};
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
        1 };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);

    EndSingleTimeCommands(zaynMem, commandBuffer);
}


template <typename T>
void CreatePushConstant(ZaynMemory* zaynMem)
{
    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(T);

    zaynMem->vulkan.vkPushConstantRanges.push_back(pushConstantRange);
}

void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, ZaynMemory* zaynMem)
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(zaynMem->vulkan.vkPhysicalDevice, imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
    {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(zaynMem);

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

    for (uint32_t i = 1; i < mipLevels; i++)
    {
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

        if (mipWidth > 1)
            mipWidth /= 2;
        if (mipHeight > 1)
            mipHeight /= 2;
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

    EndSingleTimeCommands(zaynMem, commandBuffer);
}

void CreateTextureImage(ZaynMemory* zaynMem, uint32_t& mipLevels, VkImage* textureImage, VkDeviceMemory* textureImageMemory, const std::string texturePath, VkFormat format)
{
  
    int texWidth, texHeight, texChannels; // added
    stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha); // added
    VkDeviceSize imageSize = texWidth * texHeight * 4; // added
    mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

    if (!pixels) // added
    { // added
        throw std::runtime_error("failed to load texture image!"); // added
    } // added

    VkBuffer stagingBuffer;    // added
    VkDeviceMemory stagingBufferMemory; // added

    createBuffer(zaynMem, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    void* data;    // added
    vkMapMemory(zaynMem->vulkan.vkDevice, stagingBufferMemory, 0, imageSize, 0, &data);    // added
    memcpy(data, pixels, static_cast<size_t>(imageSize));    // added
    vkUnmapMemory(zaynMem->vulkan.vkDevice, stagingBufferMemory);    // added

    stbi_image_free(pixels);

    CreateImage(texWidth, texHeight, mipLevels, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *textureImage, *textureImageMemory, Zayn); // added

    // TransitionImageLayout(zaynMem->vkTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, zaynMem);
    TransitionImageLayout(zaynMem, *textureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
    CopyBufferToImage(stagingBuffer, *textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), Zayn);

    vkDestroyBuffer(zaynMem->vulkan.vkDevice, stagingBuffer, nullptr);
    vkFreeMemory(zaynMem->vulkan.vkDevice, stagingBufferMemory, nullptr);
    GenerateMipmaps(*textureImage, format, texWidth, texHeight, mipLevels, Zayn);
}

void CreateTextureImageView(ZaynMemory* zaynMem, uint32_t& mipLevels, VkImage* textureImage, VkImageView* textureImageView)
{
    *textureImageView = CreateImageView(*textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, zaynMem);
}

void CreateTextureSampler(ZaynMemory* zaynMem, uint32_t& mipLevels, VkSampler* textureSampler)
{
    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(zaynMem->vulkan.vkPhysicalDevice, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(mipLevels);
    samplerInfo.mipLodBias = 0.0f;

    if (vkCreateSampler(zaynMem->vulkan.vkDevice, &samplerInfo, nullptr, textureSampler) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, ZaynMemory* zaynMem)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(zaynMem);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    EndSingleTimeCommands(zaynMem, commandBuffer);
}

void CreateVertexBuffer(ZaynMemory* zaynMem, std::vector<Vertex>& vertices, VkBuffer* vertexBuffer, VkDeviceMemory* vertexBufferMemory)
{
    if (vertices.empty())
    {
        return;
    }

    VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();
    // STAGING BUFFER - CPU accessible memory to upload the data from the vertex array to.
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(zaynMem, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(zaynMem->vulkan.vkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(zaynMem->vulkan.vkDevice, stagingBufferMemory);

    createBuffer(zaynMem, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *vertexBuffer, *vertexBufferMemory);

    copyBuffer(stagingBuffer, *vertexBuffer, bufferSize, zaynMem);

    vkDestroyBuffer(zaynMem->vulkan.vkDevice, stagingBuffer, nullptr);
    vkFreeMemory(zaynMem->vulkan.vkDevice, stagingBufferMemory, nullptr);
}

namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

void LoadModel(const std::string modelPath, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str()))
    {
        throw std::runtime_error(warn + err);
    }
    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2] };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1] };

            vertex.color = { 0.3f, 1.0f, 0.6f };

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices->size());
                vertices->push_back(vertex);
            }

            indices->push_back(uniqueVertices[vertex]);
        }
    }
}

void CreateIndexBuffer(ZaynMemory* zaynMem, std::vector<uint32_t> indices, VkBuffer* indexBuffer, VkDeviceMemory* indexBufferMemory)
{
    if (indices.empty())
    {
        return;
    }
    VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(zaynMem, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(zaynMem->vulkan.vkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(zaynMem->vulkan.vkDevice, stagingBufferMemory);

    createBuffer(zaynMem,bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *indexBuffer, *indexBufferMemory);

    copyBuffer(stagingBuffer, *indexBuffer, bufferSize, zaynMem);

    vkDestroyBuffer(zaynMem->vulkan.vkDevice, stagingBuffer, nullptr);
    vkFreeMemory(zaynMem->vulkan.vkDevice, stagingBufferMemory, nullptr);
}


void CreateUniformBuffer(ZaynMemory* zaynMem, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void*>& uniformBuffersMapped)
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        createBuffer(zaynMem, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
        vkMapMemory(zaynMem->vulkan.vkDevice, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
    }
}

void CreateDescriptorPool(ZaynMemory* zaynMem, VkDescriptorPool* descriptorPool, bool hasImage)
{
    std::vector<VkDescriptorPoolSize> poolSizes = {};
    VkDescriptorPoolSize poolSize_1{};
    poolSize_1.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize_1.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 20);
    poolSizes.push_back(poolSize_1);
    if (hasImage)
    {
        VkDescriptorPoolSize poolSize_2 = {};
        poolSize_2.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize_2.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 20);
        poolSizes.push_back(poolSize_2);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 20);

    if (vkCreateDescriptorPool(zaynMem->vulkan.vkDevice, &poolInfo, nullptr, descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}


void MaterialCreateDescriptorPool(ZaynMemory* zaynMem, VkDescriptorPool* descriptorPool)
{
    std::vector<VkDescriptorPoolSize> poolSizes(2);
    
    //VkDescriptorPoolSize poolSize_1{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = (MAX_FRAMES_IN_FLIGHT * 20);

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 20);


    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 20);

    if (vkCreateDescriptorPool(zaynMem->vulkan.vkDevice, &poolInfo, nullptr, descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void CreateDescriptorSets(ZaynMemory* zaynMem, bool hasImage, size_t uniformBufferSize, std::vector<VkBuffer>& uniformBuffers, VkDescriptorSetLayout* descriptorSetLayout, VkDescriptorPool* descriptorPool, std::vector<VkDescriptorSet>& descriptorSets, VkImageView* textureImageView, VkSampler* textureSampler)
{
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, *descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = *descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(zaynMem->vulkan.vkDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = uniformBufferSize;

        std::vector<VkWriteDescriptorSet> descriptorWrites{};

        VkWriteDescriptorSet descriptorWrite_uniform{};
        descriptorWrite_uniform.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite_uniform.dstSet = descriptorSets[i];
        descriptorWrite_uniform.dstBinding = 0;
        descriptorWrite_uniform.dstArrayElement = 0;
        descriptorWrite_uniform.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite_uniform.descriptorCount = 1;
        descriptorWrite_uniform.pBufferInfo = &bufferInfo;
        descriptorWrites.push_back(descriptorWrite_uniform);

        if (hasImage)
        {
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = *textureImageView;
            imageInfo.sampler = *textureSampler;

            VkWriteDescriptorSet descriptorWrite_image{};
            descriptorWrite_image.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite_image.dstSet = descriptorSets[i];
            descriptorWrite_image.dstBinding = 1;
            descriptorWrite_image.dstArrayElement = 0;
            descriptorWrite_image.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite_image.descriptorCount = 1;
            descriptorWrite_image.pImageInfo = &imageInfo;
            descriptorWrites.push_back(descriptorWrite_image);
        }

        vkUpdateDescriptorSets(zaynMem->vulkan.vkDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}




void StartRender_Init(ZaynMemory* zaynMem)
{
   CreateVKInstance(zaynMem);
   SetupDebugMessenger(zaynMem);
   CreateSurface(zaynMem);
   PickPhysicalDevice(zaynMem);    // <-- @TODO: May need to modify to account for other machines like rPi
   CreateLogicalDevice(zaynMem);   // is SAME

   CreateSwapChain(zaynMem);
   CreateImageViews(zaynMem);

   CreateRenderPass(zaynMem);
  
   CreateCommandPool(zaynMem);
   CreateDepthResources(zaynMem);
   CreateFrameBuffers(zaynMem);
}


void EndRender_Init(ZaynMemory* zaynMem)
{

    CreateCommandBuffers(zaynMem);
    CreateSyncObjects(zaynMem);
}


void UploadImageData(ZaynMemory* zaynMem, stbi_uc* pixels, uint32_t width, uint32_t height, Texture* texture)
{
    VkDeviceSize imageSize = width * height * 4;

    // a. Create Staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer(zaynMem, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    // b. copy data from pixels into staging buffer
    void* data;
    vkMapMemory(zaynMem->vulkan.vkDevice, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(zaynMem->vulkan.vkDevice, stagingBufferMemory);


    TransitionImageLayout(zaynMem, texture->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, texture->mipLevels);
   
    vkDestroyBuffer(zaynMem->vulkan.vkDevice, stagingBuffer, nullptr);
    vkFreeMemory(zaynMem->vulkan.vkDevice, stagingBufferMemory, nullptr);
    GenerateMipmaps(texture->image, VK_FORMAT_R8G8B8A8_SRGB, width, width, texture->mipLevels, Zayn);
}


void CreateTexture_v1(ZaynMemory* zaynMem, TextureCreateInfo* info, Texture* texture)
{

    CreateTextureImage(zaynMem, texture->mipLevels, &texture->image, &texture->memory, getTexturePath(info->path), info->format);
    
    CreateTextureImageView(zaynMem, texture->mipLevels, &texture->image, &texture->view);

    CreateTextureSampler(zaynMem, texture->mipLevels, &texture->sampler);
}


void CreateTexture(ZaynMemory* zaynMem, TextureCreateInfo* info, Texture* outTexture)
{
    // Load image data
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(info->path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    if (!pixels) {
        fprintf(stderr, "Failed to load texture image: %s\n", info->path);
        exit(EXIT_FAILURE);
    }

    outTexture->width = (uint32_t)texWidth;
    outTexture->height = (uint32_t)texHeight;

    // Mipmap calculation
    if (info->generateMipmaps) {
        int maxDim;
        if (texWidth > texHeight) {
            maxDim = texWidth;
        }
        else {
            maxDim = texHeight;
        }
        outTexture->mipLevels = (uint32_t)(floor(log2(maxDim)) + 1);
    }
    else {
        outTexture->mipLevels = 1;
    } 


    // Create Vulkan image
    CreateImage(
        texWidth, texHeight,
        outTexture->mipLevels,
        info->format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        outTexture->image,
        outTexture->memory,
        zaynMem
    );

    // Upload image data and generate mipmaps
    UploadImageData(Zayn, pixels, (uint32_t)texWidth, (uint32_t)texHeight, outTexture);
    stbi_image_free(pixels);

   
    // Create image view
    outTexture->view = CreateImageView( outTexture->image, info->format, VK_IMAGE_ASPECT_COLOR_BIT, outTexture->mipLevels, zaynMem);


    CreateTextureSampler(zaynMem, outTexture->mipLevels, &outTexture->sampler);

}

bool AllocateMaterialDescriptorSet(ZaynMemory* zaynMem, Material_old* material, uint32_t frameIndex) {
    // Validate required components
    if (!material->texture || !material->texture->view || !material->texture->sampler) {
        fprintf(stderr, "Material missing valid albedo texture\n");
        return false;
    }

    VkDescriptorSetLayout layout = material->texture
        ? zaynMem->vulkan.vkDescriptorSetLayout
        : zaynMem->vulkan.vkDescriptorSetLayout_blank;

    VkDescriptorPool pool = material->texture
        ? zaynMem->vulkan.vkDescriptorPool
        : zaynMem->vulkan.vkDescriptorPool_blank;
    // Descriptor set allocation
    VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };

    allocInfo.descriptorPool = zaynMem->vulkan.vkDescriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &zaynMem->vulkan.vkDescriptorSetLayout;

    VkResult result = vkAllocateDescriptorSets(zaynMem->vulkan.vkDevice, &allocInfo, &material->descriptorSets[frameIndex]);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to allocate material descriptor set: %d\n", result);
        return false;
    }

    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = zaynMem->vulkan.vkUniformBuffers[frameIndex]; // Use frame's uniform buffer
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    // Bind the material's texture to the descriptor set
    VkDescriptorImageInfo imageInfo{};
    imageInfo.sampler = material->texture->sampler;
    imageInfo.imageView = material->texture->view;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = material->descriptorSets[frameIndex];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = material->descriptorSets[frameIndex];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(zaynMem->vulkan.vkDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);


    return true;
}  

void CreateMaterial(ZaynMemory* zaynMem, MaterialCreateInfo* info, Material_old* outMaterial)
{
    memset(outMaterial, 0, sizeof(Material_old));
    outMaterial->type = info->type;
    memcpy(outMaterial->color, info->color, sizeof(float) * 4);
    outMaterial->roughness = info->roughness;
    outMaterial->metallic = info->metallic;
    outMaterial->texture = info->texture;

    outMaterial->descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        AllocateMaterialDescriptorSet(zaynMem, outMaterial, i);
    }
}

void CreateMesh(ZaynMemory* zaynMem, const std::string modelPath, Mesh* mesh) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    LoadModel(modelPath.c_str(), &vertices, &indices);

    CreateVertexBuffer(zaynMem, vertices, &mesh->vertexBuffer, &mesh->vertexBufferMemory);
    CreateIndexBuffer(zaynMem, indices, &mesh->indexBuffer, &mesh->indexBufferMemory);
    mesh->indexCount = static_cast<uint32_t>(indices.size());
}



//void CreateGameObject(ZaynMemory* zaynMem, GameObject* gameObj)
//{
//    CreateTextureImage(zaynMem, zaynMem->vulkan.vkMipLevels, &zaynMem->vulkan.vkTextureImage, &zaynMem->vulkan.vkTextureImageMemory, getTexturePath("viking_room.png"));
//    CreateTextureImageView(zaynMem, zaynMem->vulkan.vkMipLevels, &zaynMem->vulkan.vkTextureImage, &zaynMem->vulkan.vkTextureImageView);
//    CreateTextureSampler(zaynMem, zaynMem->vulkan.vkMipLevels, &zaynMem->vulkan.vkTextureSampler);
//    LoadModel(getModelPath("viking_room.obj"), &zaynMem->vulkan.vkVertices, &zaynMem->vulkan.vkIndices);
//    CreateVertexBuffer(zaynMem, zaynMem->vulkan.vkVertices, &zaynMem->vulkan.vkVertexBuffer, &zaynMem->vulkan.vkVertexBufferMemory);
//    CreateIndexBuffer(zaynMem, zaynMem->vulkan.vkIndices, &zaynMem->vulkan.vkIndexBuffer, &zaynMem->vulkan.vkIndexBufferMemory);
//}

void RenderGameObject(ZaynMemory* zaynMem, GameObject* gameObject, VkCommandBuffer commandBuffer)
{
    uint32_t dynamicOffset = zaynMem->vulkan.vkCurrentFrame * sizeof(UniformBufferObject);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, zaynMem->vulkan.vkGraphicsPipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, zaynMem->vulkan.vkPipelineLayout, 0, 1, &zaynMem->vulkan.vkDescriptorSets[zaynMem->vulkan.vkCurrentFrame], 0, nullptr);

    // Push constants for the transform
    ModelPushConstant pushConstant = {};
    pushConstant.model_1 = TRS((V3(0.0f, 1.0f, -1.0f)), AxisAngle(V3(0.0f, 0.2f, 0.20f), 0.0f), V3(1.0f, 1.0f, 1.0f));


    vkCmdPushConstants(commandBuffer, zaynMem->vulkan.vkPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ModelPushConstant), &pushConstant);

    // Bind the vertex and index buffers
    VkBuffer vertexBuffers[] = { gameObject->mesh->vertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, gameObject->mesh->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    //// Draw the mesh
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(gameObject->mesh->indices.size()), 1, 0, 0, 0);
}


void RenderGameObjects(ZaynMemory* zaynMem, VkCommandBuffer commandBuffer)
{
    GameObject& gameObj = zaynMem->gameObject;
    uint32_t dynamicOffset = zaynMem->vulkan.vkCurrentFrame * sizeof(UniformBufferObject);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, zaynMem->vulkan.vkGraphicsPipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, zaynMem->vulkan.vkPipelineLayout, 0, 1, &zaynMem->vulkan.vkDescriptorSets[zaynMem->vulkan.vkCurrentFrame], 0, nullptr);
    
    // Push constants for the transform
    ModelPushConstant pushConstant = {};
    pushConstant.model_1 = TRS((V3(0.0f, 1.0f, -1.0f)), AxisAngle(V3(0.0f, 0.2f, 0.20f), 0.0f), V3(1.0f, 1.0f, 1.0f));


    vkCmdPushConstants(commandBuffer, zaynMem->vulkan.vkPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ModelPushConstant), &pushConstant);

    // Bind the vertex and index buffers
    VkBuffer vertexBuffers[] = { zaynMem->mesh_001.vertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, zaynMem->mesh_001.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    // Draw the mesh
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(zaynMem->mesh_001.indices.size()), 1, 0, 0, 0);
}
    
void CreateGameObject(ZaynMemory* zaynMem)
{
    CreateTextureImage(zaynMem, zaynMem->gameObject.material->texture->mipLevels, &zaynMem->gameObject.material->texture->image, &zaynMem->gameObject.material->texture->memory, getTexturePath("viking_room.png"), VK_FORMAT_R8G8B8A8_SRGB);
    CreateTextureImageView(zaynMem, zaynMem->gameObject.material->texture->mipLevels, &zaynMem->gameObject.material->texture->image, &zaynMem->gameObject.material->texture->view);
    CreateTextureSampler(zaynMem, zaynMem->gameObject.material->texture->mipLevels, &zaynMem->gameObject.material->texture->sampler);
    LoadModel(getModelPath("viking_room.obj"), &zaynMem->gameObject.mesh->vertices, &zaynMem->gameObject.mesh->indices);
    CreateVertexBuffer(zaynMem, zaynMem->gameObject.mesh->vertices, &zaynMem->gameObject.mesh->vertexBuffer, &zaynMem->gameObject.mesh->vertexBufferMemory);
    CreateIndexBuffer(zaynMem, zaynMem->gameObject.mesh->indices, &zaynMem->gameObject.mesh->indexBuffer, &zaynMem->gameObject.mesh->indexBufferMemory);
}

void CreateGameObject_v1(ZaynMemory* zaynMem, GameObject* gameObj, mat4 transform, std::string objRelativePath, std::string textureRelativePath)
{  
    gameObj->pushConstantData.model_1 = transform;
    
    CreateTextureImage(zaynMem, gameObj->material->texture->mipLevels, &gameObj->material->texture->image, &gameObj->material->texture->memory, getTexturePath(textureRelativePath), VK_FORMAT_R8G8B8A8_SRGB);
    CreateTextureImageView(zaynMem, gameObj->material->texture->mipLevels, &gameObj->material->texture->image, &gameObj->material->texture->view);
    CreateTextureSampler(zaynMem, gameObj->material->texture->mipLevels, &gameObj->material->texture->sampler);
    LoadModel(getModelPath(objRelativePath), &gameObj->mesh->vertices, &gameObj->mesh->indices);
    CreateVertexBuffer(zaynMem, gameObj->mesh->vertices, &gameObj->mesh->vertexBuffer, &gameObj->mesh->vertexBufferMemory);
    CreateIndexBuffer(zaynMem, gameObj->mesh->indices, &gameObj->mesh->indexBuffer, &gameObj->mesh->indexBufferMemory);
}

void CreateGameObject_v2(ZaynMemory* zaynMem, Texture* texture, GameObject* gameObj, mat4 transform, std::string objRelativePath)
{
    gameObj->material->texture;
    gameObj->pushConstantData.model_1 = transform;

    LoadModel(getModelPath(objRelativePath), &gameObj->mesh->vertices, &gameObj->mesh->indices);
    CreateVertexBuffer(zaynMem, gameObj->mesh->vertices, &gameObj->mesh->vertexBuffer, &gameObj->mesh->vertexBufferMemory);
    CreateIndexBuffer(zaynMem, gameObj->mesh->indices, &gameObj->mesh->indexBuffer, &gameObj->mesh->indexBufferMemory);
}

// ENTRY POINT FOR VULKAN RENDERER INIT

void InitRender_Vulkan(ZaynMemory* zaynMem)
{

	std::cout << "InitRender_Vulkan()" << std::endl;
    // I DONT THINK ANY GAME SPECIFIC THINGS OCCUR HERE
    StartRender_Init(zaynMem);
    std::cout << "after InitRender_Vulkan()" << std::endl;



    // INIT MATERIAL SYSTEM
    zaynMem->materialSystem = new MaterialSystem(&zaynMem->vulkan.vkDevice);



    // CUSTOM CODE FOR RENDERS @TODO need to make this simpler
    CreateDescriptorSetLayout(&zaynMem->vulkan.vkDescriptorSetLayout, true, zaynMem); // this one is for those that have texures attached
    CreateDescriptorPool(zaynMem, &zaynMem->vulkan.vkDescriptorPool, true);      // <---- CAN POTENTIAL BE RESUSED BETWEEN ENTITIES THAT HAVE THE SAME TYPES OF THINGS BEING SHARED
    

    CreatePushConstant<ModelPushConstant>(zaynMem);
    
    CreateGraphicsPipeline(zaynMem, &zaynMem->vulkan.vkGraphicsPipeline, getShaderPath("vkShader_3d_INIT_vert.spv"), getShaderPath("vkShader_3d_INIT_frag.spv"), zaynMem->vulkan.vkPushConstantRanges, &zaynMem->vulkan.vkDescriptorSetLayout, &zaynMem->vulkan.vkPipelineLayout);
    


    // for new system  1. 
    CreateTextureImage(zaynMem, zaynMem->texture_001.mipLevels, &zaynMem->texture_001.image, &zaynMem->texture_001.memory, getTexturePath("viking_room.png"), VK_FORMAT_R8G8B8A8_SRGB);
    CreateTextureImageView(zaynMem, zaynMem->texture_001.mipLevels, &zaynMem->texture_001.image, &zaynMem->texture_001.view);
    CreateTextureSampler(zaynMem, zaynMem->texture_001.mipLevels, &zaynMem->texture_001.sampler); 
    //CreateMesh(zaynMem, getModelPath("viking_room.obj"), zaynMem->gameObject.mesh);
    LoadModel(getModelPath("viking_room.obj"), &zaynMem->mesh_001.vertices, &zaynMem->mesh_001.indices);
    CreateVertexBuffer(zaynMem, zaynMem->mesh_001.vertices, &zaynMem->mesh_001.vertexBuffer, &zaynMem->mesh_001.vertexBufferMemory);
    CreateIndexBuffer(zaynMem, zaynMem->mesh_001.indices, &zaynMem->mesh_001.indexBuffer, &zaynMem->mesh_001.indexBufferMemory);



    CreateGameObject(zaynMem);

   
   


    CreateTextureImage(zaynMem, zaynMem->vulkan.vkMipLevels, &zaynMem->vulkan.vkTextureImage, &zaynMem->vulkan.vkTextureImageMemory, getTexturePath("viking_room.png"), VK_FORMAT_R8G8B8A8_SRGB);
    CreateTextureImageView(zaynMem, zaynMem->vulkan.vkMipLevels, &zaynMem->vulkan.vkTextureImage, &zaynMem->vulkan.vkTextureImageView);
    CreateTextureSampler(zaynMem, zaynMem->vulkan.vkMipLevels, &zaynMem->vulkan.vkTextureSampler);     
    LoadModel(getModelPath("viking_room.obj"), &zaynMem->vulkan.vkVertices, &zaynMem->vulkan.vkIndices);
    CreateVertexBuffer(zaynMem, zaynMem->vulkan.vkVertices, &zaynMem->vulkan.vkVertexBuffer, &zaynMem->vulkan.vkVertexBufferMemory);
    CreateIndexBuffer(zaynMem, zaynMem->vulkan.vkIndices, &zaynMem->vulkan.vkIndexBuffer, &zaynMem->vulkan.vkIndexBufferMemory);
    
    CreateUniformBuffer(zaynMem, zaynMem->vulkan.vkUniformBuffers, zaynMem->vulkan.vkUniformBuffersMemory, zaynMem->vulkan.vkUniformBuffersMapped);
    
    
    CreateDescriptorSets(zaynMem, true, sizeof(UniformBufferObject), zaynMem->vulkan.vkUniformBuffers, &zaynMem->vulkan.vkDescriptorSetLayout, &zaynMem->vulkan.vkDescriptorPool, zaynMem->vulkan.vkDescriptorSets, &zaynMem->vulkan.vkTextureImageView, &zaynMem->vulkan.vkTextureSampler);


    
    EndRender_Init(zaynMem);
}

void CleanUpSwapChain(ZaynMemory* zaynMem)
{
    vkDestroyImageView(zaynMem->vulkan.vkDevice, zaynMem->vulkan.vkDepthImageView, nullptr);
    vkDestroyImage(zaynMem->vulkan.vkDevice, zaynMem->vulkan.vkDepthImage, nullptr);
    vkFreeMemory(zaynMem->vulkan.vkDevice, zaynMem->vulkan.vkDepthImageMemory, nullptr);

    for (size_t i = 0; i < zaynMem->vulkan.vkSwapChainFramebuffers.size(); i++)
    {
        vkDestroyFramebuffer(zaynMem->vulkan.vkDevice, zaynMem->vulkan.vkSwapChainFramebuffers[i], nullptr);
    }

    for (size_t i = 0; i < zaynMem->vulkan.vkSwapChainImageViews.size(); i++)
    {
        vkDestroyImageView(zaynMem->vulkan.vkDevice, zaynMem->vulkan.vkSwapChainImageViews[i], nullptr);
    }
    
    vkDestroySwapchainKHR(zaynMem->vulkan.vkDevice, zaynMem->vulkan.vkSwapChain, nullptr);
}

void RecreateSwapChain(ZaynMemory* zaynMem)
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(zaynMem->window, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(zaynMem->window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(zaynMem->vulkan.vkDevice);

    CleanUpSwapChain(zaynMem);
    CreateSwapChain(zaynMem);
    CreateImageViews(zaynMem);
    CreateDepthResources(zaynMem);
    CreateFrameBuffers(zaynMem);
}

VkResult AcquireNextImage(ZaynMemory* zaynMem, uint32_t* imageIndex)
{
    vkWaitForFences(zaynMem->vulkan.vkDevice, 1, &zaynMem->vulkan.vkInFlightFences[zaynMem->vulkan.vkCurrentFrame], VK_TRUE, UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(
        zaynMem->vulkan.vkDevice,
        zaynMem->vulkan.vkSwapChain,
        UINT64_MAX,
        zaynMem->vulkan.vkImageAvailableSemaphores[zaynMem->vulkan.vkCurrentFrame], // must be a not signaled semaphore
        VK_NULL_HANDLE,
        imageIndex);

    return result;
}

bool BeginFrame(ZaynMemory* zaynMem)
{
    assert(!zaynMem->vulkan.vkIsFrameStarted && "cannot call begin frame when frame buffer is already in progress");
   auto result = AcquireNextImage(zaynMem, &zaynMem->vulkan.vkCurrentImageIndex);
   
   if (result == VK_ERROR_OUT_OF_DATE_KHR)
   {
       RecreateSwapChain(zaynMem);
       return false;
   }
   else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
   {
       throw std::runtime_error("failed to acquire swap chain image!");
   }
   
   zaynMem->vulkan.vkIsFrameStarted = true;
   
   // vkResetFences(Zayn->vkDevice, 1, &Zayn->vkInFlightFences[Zayn->vkCurrentFrame]);
   // vkResetCommandBuffer(Zayn->vkCommandBuffers[Zayn->vkCurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);
   
   VkCommandBufferBeginInfo beginInfo{};
   beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
   if (vkBeginCommandBuffer(zaynMem->vulkan.vkCommandBuffers[zaynMem->vulkan.vkCurrentFrame], &beginInfo) != VK_SUCCESS)
   {
       throw std::runtime_error("failed to begin recording command buffer!");
       return false;
   }

    return true;
}




void UpdateUniformBuffer(uint32_t currentImage, ZaynMemory* zaynMem)
{
    // static auto startTime = std::chrono::high_resolution_clock::now();

    // auto currentTime = std::chrono::high_resolution_clock::now();
    // float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

   
    Camera* cam = &Zayn->camera;

    UniformBufferObject ubo = {};
    // ubo.model = TRS(V3(0.0f, 0.0f, 0.0f), AxisAngle(V3(0.0f, 0.2f, 0.20f), time * DegToRad(10.0f)), V3(1.0f, 1.0f, 1.0f));

    // apply view based on camera rotation

    cam->front.x = cosf(DegToRad(cam->yaw)) * cosf(DegToRad(cam->pitch));
    cam->front.y = sinf(DegToRad(cam->pitch));
    cam->front.z = sinf(DegToRad(cam->yaw)) * cosf(DegToRad(cam->pitch));
    cam->front = Normalize(cam->front);

    glm::vec3 camPos = glm::vec3(cam->pos.x, cam->pos.y, cam->pos.z);
    glm::vec3 camFront = glm::vec3(cam->front.x, cam->front.y, cam->front.z);
    glm::vec3 camUp = glm::vec3(cam->up.x, cam->up.y, cam->up.z);

    ubo.view = glm::lookAt(camPos, camPos + camFront, camUp);
    // ubo.view = glm::lookAt(cam->pos, cam->pos + cam->front, cam->up);

    ubo.proj = glm::perspective(glm::radians(60.0f), zaynMem->vulkan.vkSwapChainExtent.width / (float)zaynMem->vulkan.vkSwapChainExtent.height, 0.1f, 1000.0f);
    ubo.proj[1][1] *= -1;


    memcpy(zaynMem->vulkan.vkUniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void BeginSwapChainRenderPass(ZaynMemory* zaynMem, VkCommandBuffer commandBuffer)
{
    assert(zaynMem->vulkan.vkIsFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == zaynMem->vulkan.vkCommandBuffers[zaynMem->vulkan.vkCurrentFrame] && "Can't begin render pass on command buffer from a different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = zaynMem->vulkan.vkRenderPass;
    renderPassInfo.framebuffer = zaynMem->vulkan.vkSwapChainFramebuffers[zaynMem->vulkan.vkCurrentImageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = zaynMem->vulkan.vkSwapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(zaynMem->vulkan.vkSwapChainExtent.width);
    viewport.height = static_cast<float>(zaynMem->vulkan.vkSwapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{ {0, 0}, zaynMem->vulkan.vkSwapChainExtent };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void RenderEntity_notYetEntity(ZaynMemory* zaynMem, VkCommandBuffer imageBuffer, VkPipeline* pipeline, VkPipelineLayout* pipelineLayout, std::vector<VkDescriptorSet>& descriptorSets, VkBuffer* vertexBuffer, VkBuffer* indexBuffer, std::vector<uint32_t>& indices, ModelPushConstant* pushConstant)
{
    vkCmdBindPipeline(imageBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);
    vkCmdBindDescriptorSets(imageBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipelineLayout, 0, 1, &descriptorSets[zaynMem->vulkan.vkCurrentFrame], 0, nullptr);

    vkCmdPushConstants(imageBuffer, *pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ModelPushConstant), pushConstant);

    

    VkBuffer vertexBuffers[] = { *vertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(imageBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(imageBuffer, *indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(imageBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

}

void EndSwapChainRenderPass(ZaynMemory* zaynMem, VkCommandBuffer commandBuffer)
{
    assert(zaynMem->vulkan.vkIsFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == zaynMem->vulkan.vkCommandBuffers[zaynMem->vulkan.vkCurrentFrame] && "Can't begin render pass on command buffer from a different frame");

    vkCmdEndRenderPass(commandBuffer);
}

VkResult SubmitCommandBuffers(ZaynMemory* zaynMem, std::vector<VkCommandBuffer> buffers, uint32_t* imageIndex)
{

    if (zaynMem->vulkan.vkImagesInFlight[*imageIndex] != VK_NULL_HANDLE)
    {
        vkWaitForFences(zaynMem->vulkan.vkDevice, 1, &zaynMem->vulkan.vkImagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
    }
    zaynMem->vulkan.vkImagesInFlight[*imageIndex] = zaynMem->vulkan.vkImagesInFlight[zaynMem->vulkan.vkCurrentFrame];
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { zaynMem->vulkan.vkImageAvailableSemaphores[zaynMem->vulkan.vkCurrentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = static_cast<uint32_t>(buffers.size());
    // submitInfo.commandBufferCount
    submitInfo.pCommandBuffers = buffers.data();

    VkSemaphore signalSemaphores[] = { zaynMem->vulkan.vkRenderFinishedSemaphores[zaynMem->vulkan.vkCurrentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(zaynMem->vulkan.vkDevice, 1, &zaynMem->vulkan.vkInFlightFences[zaynMem->vulkan.vkCurrentFrame]);
    if (vkQueueSubmit(zaynMem->vulkan.vkGraphicsQueue, 1, &submitInfo, zaynMem->vulkan.vkInFlightFences[zaynMem->vulkan.vkCurrentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { zaynMem->vulkan.vkSwapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = imageIndex;
    // presentInfo.pImageIndices = &Zayn->vkCurrentImageIndex;

    auto result = vkQueuePresentKHR(zaynMem->vulkan.vkPresentQueue, &presentInfo);

    zaynMem->vulkan.vkCurrentFrame = (zaynMem->vulkan.vkCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;

}

void EndFrame(ZaynMemory* zaynMem)
{
    assert(zaynMem->vulkan.vkIsFrameStarted && "Can't call endFrame while frame is not in progress");

    std::vector<VkCommandBuffer> submitCommandBuffers = {};

    submitCommandBuffers.push_back(zaynMem->vulkan.vkCommandBuffers[zaynMem->vulkan.vkCurrentFrame]);

#if IMGUI    
    submitCommandBuffers.push_back(zaynMem->vulkan.imGuiCommandBuffers[zaynMem->vulkan.vkCurrentFrame]);
#endif

    if (vkEndCommandBuffer(submitCommandBuffers[0]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }

    auto result = SubmitCommandBuffers(zaynMem, submitCommandBuffers, &zaynMem->vulkan.vkCurrentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || zaynMem->vulkan.vkFramebufferResized)
    {
        zaynMem->vulkan.vkFramebufferResized = false;
        RecreateSwapChain(zaynMem);
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }
    zaynMem->vulkan.vkIsFrameStarted = false;
}
void RenderGameObjects_v2(ZaynMemory* zaynMem, VkCommandBuffer commandBuffer)
{
    GameObject& gameObj = zaynMem->gameObject;
    uint32_t dynamicOffset = zaynMem->vulkan.vkCurrentFrame * sizeof(UniformBufferObject);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, zaynMem->vulkan.vkGraphicsPipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, zaynMem->vulkan.vkPipelineLayout, 0, 1, &zaynMem->vulkan.vkDescriptorSets[zaynMem->vulkan.vkCurrentFrame], 0, nullptr);

    


    vkCmdPushConstants(commandBuffer, zaynMem->vulkan.vkPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ModelPushConstant), &gameObj.pushConstantData);

    // Bind the vertex and index buffers
    VkBuffer vertexBuffers[] = { gameObj.mesh->vertexBuffer};
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, gameObj.mesh->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    // Draw the mesh
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(gameObj.mesh->indices.size()), 1, 0, 0, 0);
}

void RenderGameObject_v3(ZaynMemory* zaynMem, VkCommandBuffer commandBuffer)
{
    GameObject& gameObj = zaynMem->gameObject2;
    uint32_t dynamicOffset = zaynMem->vulkan.vkCurrentFrame * sizeof(UniformBufferObject);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, zaynMem->vulkan.vkGraphicsPipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, zaynMem->vulkan.vkPipelineLayout, 0, 1, &zaynMem->vulkan.vkDescriptorSets[zaynMem->vulkan.vkCurrentFrame], 0, nullptr);

    vkCmdPushConstants(commandBuffer, zaynMem->vulkan.vkPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ModelPushConstant), &gameObj.pushConstantData);

    // Bind the vertex and index buffers
    VkBuffer vertexBuffers[] = { gameObj.mesh->vertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, gameObj.mesh->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    // Draw the mesh
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(gameObj.mesh->indices.size()), 1, 0, 0, 0);
}

vec3 posModel1 = V3(0.0f, 0.0f, -0.0f);


void UpdateRender_Vulkan(ZaynMemory* zaynMem)
{
    ModelPushConstant pushConstantData1 = {};
    pushConstantData1.model_1 = TRS((posModel1), AxisAngle(V3(30.0f, 0.2f, 0.20f), 30.0f), V3(1.0f, 1.0f, 1.0f));
   
   
   if (BeginFrame(zaynMem))
   {
       UpdateUniformBuffer(zaynMem->vulkan.vkCurrentFrame, Zayn);
   
       BeginSwapChainRenderPass(zaynMem, zaynMem->vulkan.vkCommandBuffers[zaynMem->vulkan.vkCurrentFrame]);
   
       for (int i = 0; i < 5; i++)
       {
            //Render
       }
       
       //RenderEntity_notYetEntity(zaynMem, zaynMem->vulkan.vkCommandBuffers[zaynMem->vulkan.vkCurrentFrame], &zaynMem->vulkan.vkGraphicsPipeline, &zaynMem->vulkan.vkPipelineLayout, zaynMem->vulkan.vkDescriptorSets, &zaynMem->vulkan.vkVertexBuffer, &zaynMem->vulkan.vkIndexBuffer, zaynMem->vulkan.vkIndices, &pushConstantData1);
       //RenderGameObjects(zaynMem, zaynMem->vulkan.vkCommandBuffers[zaynMem->vulkan.vkCurrentFrame]);
       RenderGameObjects_v2(zaynMem, zaynMem->vulkan.vkCommandBuffers[zaynMem->vulkan.vkCurrentFrame]);
       RenderGameObject_v3(zaynMem, zaynMem->vulkan.vkCommandBuffers[zaynMem->vulkan.vkCurrentFrame]);
    
   }
   
   EndSwapChainRenderPass(zaynMem, zaynMem->vulkan.vkCommandBuffers[zaynMem->vulkan.vkCurrentFrame]);
   
   EndFrame(zaynMem);

}



void VkCleanup(ZaynMemory* zaynMem) 
{
    for (auto imageView : zaynMem->vulkan.vkSwapChainImageViews) {
        vkDestroyImageView(zaynMem->vulkan.vkDevice, imageView, nullptr);
    }

    vkDestroySwapchainKHR(zaynMem->vulkan.vkDevice, zaynMem->vulkan.vkSwapChain, nullptr);
    vkDestroyDevice(zaynMem->vulkan.vkDevice, nullptr);

    if (enableValidationLayers) {
        //DestroyDebugUtilsMessengerEXT(zaynMem->vulkan.vkInstance, zaynMem->vulkan.vkDebugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(zaynMem->vulkan.vkInstance, zaynMem->vulkan.vkSurface, nullptr);
    vkDestroyInstance(zaynMem->vulkan.vkInstance, nullptr);

    glfwDestroyWindow(zaynMem->window);

    glfwTerminate();
}
