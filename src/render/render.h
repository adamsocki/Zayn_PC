#pragma once

//#include "render_vulkan.h"
//#include "../zayn.h"
#include <glm/glm.hpp>


struct ZaynMemory;

enum RendererType 
{
    RENDERER_OPENGL,
    RENDERER_VULKAN_2D,
    RENDERER_VULKAN
};





void InitRender(ZaynMemory* zaynMem);
void UpdateRender(ZaynMemory* zaynMem);