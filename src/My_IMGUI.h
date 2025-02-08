#pragma once

#include <vulkan/vulkan.h>
#include "zayn.h"

//#include "../external/imgui/imgui.h"
//#include "../external/imgui/backends/imgui_impl_glfw.h"
//#include "../external/imgui/backends/imgui_impl_vulkan.h"
//struct ZaynMemory;

namespace ZaynEngine
{
	struct My_IMGUI
	{



	};


	void InitIMGUI(ZaynMemory* zaynMem);
	void LevelEditorIMGUI(ZaynMemory* zaynMem);
	void RenderIMGUI(ZaynMemory* zaynMem);

}
