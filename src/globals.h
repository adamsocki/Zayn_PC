#pragma once
#include "zayn.h"

#define VULKAN 1



#define OPENGL 0

#define WINDOWS 1

#define IMGUI 1



#if VULKAN
//const int MAX_FRAMES_IN_FLIGHT = 2;

#define STB_IMAGE_IMPLEMENTATION
//#include "include/stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL

#endif  

//static ImGui_ImplVulkanH_Window g_MainWindowData;



extern ZaynMemory* Zayn;
extern InputManager* Input;