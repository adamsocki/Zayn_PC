

#include "render.h"

#include "render_vulkan.h"
#include "../zayn.h"
#include "../globals.h"





void InitRender(ZaynMemory* zaynMem)
{
#if VULKAN
	zaynMem->rendererType = RENDERER_VULKAN;


	//// Example: Create a material with a texture
	//TextureCreateInfo texInfo = {};
	//texInfo.path = getTexturePath("viking_room.png");
	//texInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	//texInfo.generateMipmaps = true;

		//matInfo.type = MATERIAL_PBR;
	//matInfo.color[0] = 1.0f; // Red
	//matInfo.color[1] = 1.0f; // Green
	//matInfo.color[2] = 1.0f; // Blue
	//matInfo.color[3] = 1.0f; // Alpha
	//matInfo.roughness = 0.5f;
	//matInfo.metallic = 0.1f;

	MaterialCreateInfo matInfo = {};
	Material objectMaterial;
	matInfo.texture = &zaynMem->texture_001;
	CreateMaterial(zaynMem, &matInfo, &objectMaterial);

	zaynMem->gameObject.material = &objectMaterial;
	zaynMem->gameObject.mesh = &zaynMem->mesh_001;
	zaynMem->gameObject.transform = glm::mat4(1.0f);

	

	InitRender_Vulkan(zaynMem);

	mat4 transform1 = TRS((V3(10.0f, 11.0f, -11.0f)), AxisAngle(V3(0.0f, 0.2f, 0.20f), 0.0f), V3(1.0f, 1.0f, 1.0f));
	CreateGameObject_v1(zaynMem, &zaynMem->gameObject, transform1, "viking_room.obj", "viking_room.png");


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
