// zayn.h
#pragma once



#include <GLFW/glfw3.h>
#include "data_types.h"
#include "math/my_math.h"
#include "my_memory.h"
#include "input.h"
#include "time.h"
#include <iostream>
#include "render/render.h"
#include "render/render_vulkan.h" 
#include "camera.h"



#include <vulkan/vulkan.h>
#include <vector>

//struct RendererType;
struct Vertex;

struct MyVulkanData
{
	bool vkFramebufferResized;
	VkInstance vkInstance;
	VkDebugUtilsMessengerEXT vkDebugMessenger;
	VkSurfaceKHR vkSurface;

	VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
	VkDevice vkDevice;
	VkQueue vkGraphicsQueue;

	VkQueue vkPresentQueue;


	VkSwapchainKHR vkSwapChain;

	std::vector<VkImage> vkSwapChainImages;
	std::vector<VkImageView> vkSwapChainImageViews;

	VkExtent2D vkSwapChainExtent;
	VkFormat vkSwapChainImageFormat;
	VkRenderPass vkRenderPass;

	VkCommandPool vkCommandPool;

	VkPhysicalDeviceMemoryProperties vkMemProperties;

	std::vector<VkImage> vkDepthImages;
	std::vector<VkDeviceMemory> vkDepthImageMemorys;
	std::vector<VkImageView> vkDepthImageViews;

	std::vector<VkFramebuffer> vkSwapChainFramebuffers;

	std::vector<VkCommandBuffer> vkCommandBuffers;

	std::vector<VkSemaphore> vkImageAvailableSemaphores;
	std::vector<VkSemaphore> vkRenderFinishedSemaphores;
	std::vector<VkFence> vkInFlightFences;
	std::vector<VkFence> vkImagesInFlight;

	VkImage vkDepthImage;
	VkDeviceMemory vkDepthImageMemory;
	VkImageView vkDepthImageView;

	uint32_t vkQueueFamilyCount = -1;






	// CUSTOM CODE FOR RENDERS @TODO need to make this simpler
	VkDescriptorSetLayout vkDescriptorSetLayout;
	VkDescriptorSetLayout vkDescriptorSetLayout_blank;
	std::vector<VkPushConstantRange> vkPushConstantRanges;
	VkPipeline vkGraphicsPipeline;
	VkPipeline vkGraphicsPipeline_blank;
	VkPipelineLayout vkPipelineLayout;
	VkShaderModule vkVertShaderModule;
	VkShaderModule vkFragShaderModule;
	uint32_t vkMipLevels;
	VkImage vkTextureImage;
	VkDeviceMemory vkTextureImageMemory;
	VkImageView vkTextureImageView;
	VkSampler vkTextureSampler;
	std::vector<Vertex> vkVertices;
	std::vector<uint32_t> vkIndices;
	VkBuffer vkVertexBuffer;
	VkDeviceMemory vkVertexBufferMemory;
	VkBuffer vkIndexBuffer;
	VkDeviceMemory vkIndexBufferMemory;
	std::vector<VkBuffer> vkUniformBuffers;
	std::vector<VkDeviceMemory> vkUniformBuffersMemory;
	std::vector<void*> vkUniformBuffersMapped;
	VkDescriptorPool vkDescriptorPool;
	std::vector<VkDescriptorSet> vkDescriptorSets;
	uint32_t vkCurrentFrame = 0;
	uint32 vkCurrentImageIndex;
	bool vkIsFrameStarted = false;
};


struct ZaynMemory 
{
	vec2 test_vec_Zayn;
	uint64 testuint;

	MemoryArena permanentMemArena;
	MemoryArena frameMemArena;

	InputManager inputManager;
	InputDevice* keyboard;

	vec2 windowSize;
	GLFWwindow* window;

	MyVulkanData vulkan;

	ZaynTime time;

	RendererType rendererType;
	Camera camera;
};

void ZaynInit(ZaynMemory* zaynMemory);
void ZaynUpdateAndRender(ZaynMemory* zaynMemory);