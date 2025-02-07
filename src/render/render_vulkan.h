#pragma once
#include <iostream>
#include <optional>
#include <vector>
#include <array>
#include <vulkan/vulkan.h>
#include "../math/matrix.h"
#include <glm/glm.hpp>

//#include "../zayn.h" 



struct ZaynMemory; 
struct TextureCreateInfo;
struct Texture;
struct MaterialCreateInfo;
struct Material_old;
struct GameObject;
//struct MyVulkanData;

struct InstancedData
{
    mat4 modelMatrix;
};

struct ModelPushConstant
{
    mat4 model_1;
};

struct UniformBufferObject
{
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal;

    mat4 modelMatrix;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputBindingDescription, 2>  getBindingDescriptions_instanced()
    {
        std::array<VkVertexInputBindingDescription, 2> bindingDescriptions{};
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        bindingDescriptions[1].binding = 1;
        bindingDescriptions[1].stride = sizeof(InstancedData);
        bindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        return bindingDescriptions;
    }

    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, normal);

        return attributeDescriptions;
    }

    static std::array<VkVertexInputAttributeDescription, 8> getAttributeDescriptions_instanced()
    {
        std::array<VkVertexInputAttributeDescription, 8> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, normal);

        attributeDescriptions[4].binding = 1; // Assuming binding 1 for instance data
        attributeDescriptions[4].location = 4;
        attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[4].offset = offsetof(InstancedData, modelMatrix) + sizeof(glm::vec4) * 0;

        attributeDescriptions[5].binding = 1; // Assuming binding 1 for instance data
        attributeDescriptions[5].location = 5;
        attributeDescriptions[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[5].offset = offsetof(InstancedData, modelMatrix) + sizeof(glm::vec4) * 1;

        attributeDescriptions[6].binding = 1; // Assuming binding 1 for instance data
        attributeDescriptions[6].location = 6;
        attributeDescriptions[6].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[6].offset = offsetof(InstancedData, modelMatrix) + sizeof(glm::vec4) * 2;

        attributeDescriptions[7].binding = 1; // Assuming binding 1 for instance data
        attributeDescriptions[7].location = 7;
        attributeDescriptions[7].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[7].offset = offsetof(InstancedData, modelMatrix) + sizeof(glm::vec4) * 3;

        return attributeDescriptions;
    }

    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

template <typename T>
void CreatePushConstant(std::vector<VkPushConstantRange>& pushConstantRanges)
{
    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(T);

    pushConstantRanges.push_back(pushConstantRange);
}




struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};


std::string getTexturePath(const std::string& filename);
void CreateTexture(ZaynMemory* zaynMem, TextureCreateInfo* info, Texture* outTexture);
void CreateMaterial(ZaynMemory* zaynMem, MaterialCreateInfo* info, Material_old* outMaterial);
void CreateTexture_v1(ZaynMemory* zaynMem, TextureCreateInfo* info, Texture* texture);

void CreateGameObject_v1(ZaynMemory* zaynMem, GameObject* gameObj, mat4 transform, std::string objRelativePath, std::string textureRelativePath);  
void CreateGameObject_v2(ZaynMemory* zaynMem, Texture* texture, GameObject* gameObj, mat4 transform, std::string objRelativePath);
void CreateMaterial_v1(ZaynMemory* zaynMem, MaterialCreateInfo* info, Material_old* outMaterial);
void EndSingleTimeCommands(ZaynMemory* zaynMem, VkCommandBuffer commandBuffer);
VkCommandBuffer BeginSingleTimeCommands(ZaynMemory* zaynMem);

void InitRender_Vulkan(ZaynMemory* zaynMem);
void UpdateRender_Vulkan(ZaynMemory* zaynMem);