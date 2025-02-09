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
#include "../MaterialSystem.h"

#include "../assets/fonts/stb_font_consolas_24_latin1.inl"
#include "../external/imgui/imgui.h"
#include "../LevelEditor.h"

//struct RendererType;
//struct GameObject;


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
	VkDescriptorPool vkDescriptorPool_blank;
	std::vector<VkDescriptorSet> vkDescriptorSets;
	uint32_t vkCurrentFrame = 0;
	uint32 vkCurrentImageIndex;
	bool vkIsFrameStarted = false;
};




// --- Texture --- 
struct TextureCreateInfo
{
	std::string path;
	VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
	bool generateMipmaps;
	//VkFilter filter;
	VkSamplerAddressMode addressMode;
};

struct Texture
{
	VkImage image;
	VkDeviceMemory memory;
	VkImageView view;
	VkSampler sampler;
	uint32_t width;
	uint32_t height;
	uint32_t mipLevels = 1;
};

enum MaterialType
{
	MATERIAL_PBR,
	MATERIAL_UNLIT
};

struct MaterialCreateInfo
{
	MaterialType type = MATERIAL_PBR;
	Texture* texture; 
	std::string name;
	float color[4];
	float roughness;
	float metallic;
};

struct Material_old
{
	MaterialType type;
	std::vector<VkDescriptorSet> descriptorSets;
	Texture* texture; 
	std::string name;
	float color[4];
	float metallic;
	float roughness;
};

struct Mesh
{
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	uint32_t indexCount;
	uint32_t vertexCount;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

struct GameObject
{
	Material_old* material;
	Mesh* mesh;
	glm::mat4 transform;

	std::string name;
	
    ModelPushConstant pushConstantData;
};

struct MyIMGUI
{
	VkDescriptorPool imGuiDescriptorPool;
	VkRenderPass imGuiRenderPass;
	std::vector<VkCommandBuffer> imGuiCommandBuffers;
	std::vector<VkFramebuffer> imGuiFrameBuffers;
	VkCommandPool imGuiCommandPool;
};



struct ZaynMemory 
{
	vec2 test_vec_Zayn;
	uint64 testuint;

	MyIMGUI myIMGUI;

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

	ImVector<const char*> extensions;


	Texture texture_001;
	Material_old material_001;
	Material_old material_002;
	Material_old material_003;


	std::vector<Material_old> materials;

	Texture texture_002;
	Texture texture_003;


	MaterialSystem *materialSystem;

	Mesh mesh_001;
	Mesh mesh_002;
	std::vector<Mesh> meshes;

	GameObject gameObject;
	GameObject gameObject2;
	GameObject gameObject3; 


	std::vector<GameObject> gameObjects;

	LevelEditor le;

	
};

void ZaynInit(ZaynMemory* zaynMemory);
void ZaynUpdateAndRender(ZaynMemory* zaynMemory);