

#include "render.h"

#include "render_vulkan.h"
#include "../zayn.h"
#include "../globals.h"





void InitRender(ZaynMemory* zaynMem)
{
#if VULKAN
	zaynMem->rendererType = RENDERER_VULKAN;
	InitRender_Vulkan(zaynMem);
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
