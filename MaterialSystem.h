#pragma once

#include <vulkan/vulkan.h>


const int MAX_FRAMES_IN_FLIGHT = 2;

class MaterialSystem
{


private:


	VkDevice* vkDevice;
	//std::



public:


	MaterialSystem(VkDevice* device);

	~MaterialSystem();  


	//void CreateDescriptorSet



};

