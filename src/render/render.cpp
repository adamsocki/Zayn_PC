

#include "render.h"

#include "render_vulkan.h"
#include "../zayn.h"
#include "../globals.h"





void InitRender(ZaynMemory* zaynMem)
{
#if VULKAN
	zaynMem->rendererType = RENDERER_VULKAN;
	InitRender_Vulkan(zaynMem);


	// Example: Create a material with a texture
	TextureCreateInfo texInfo = {};
	texInfo.path = getTexturePath("viking_room.png");
	texInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	texInfo.generateMipmaps = true;

	//Texture objectTexture;
	CreateTexture(zaynMem, &texInfo, &zaynMem->texture_001);

	MaterialCreateInfo matInfo = {};
	matInfo.type = MATERIAL_PBR;
	matInfo.texture = &zaynMem->texture_001;
	matInfo.color[0] = 1.0f; // Red
	matInfo.color[1] = 1.0f; // Green
	matInfo.color[2] = 1.0f; // Blue
	matInfo.color[3] = 1.0f; // Alpha
	matInfo.roughness = 0.5f;
	matInfo.metallic = 0.1f;

	//Material objectMaterial;
	CreateMaterial(zaynMem, &matInfo, &zaynMem->material_001);

	zaynMem->gameObject.material = &zaynMem->material_001;
	zaynMem->gameObject.mesh = &zaynMem->mesh_001;


	zaynMem->gameObject.transform = glm::mat4(1.0f);


	//std::cout << "VULKAN RENDERER" << std::endl;
#endif





}





void UpdateRender(ZaynMemory* zaynMem)
{
	switch (zaynMem->rendererType)
	{
		case RENDERER_VULKAN:
		{
			UpdateRender_Vulkan(zaynMem);
			break;
		}
		default:
		{
			std::cout << "missing renderer set: " << zaynMem->rendererType << std::endl;
		}
	}
}
