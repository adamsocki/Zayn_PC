#include "My_IMGUI.h"



//#include <imgui_impl_vulkan.h>
//#include <imgui_impl_glfw.h>
#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>



struct ZaynMemory;
#include "zayn.h"
#include "../external/imgui/imgui.h"
#include "../external/imgui/backends/imgui_impl_glfw.h"
#include "../external/imgui/backends/imgui_impl_vulkan.h"
#include "render/render_vulkan.h"
#include <string>


void ZaynEngine::InitIMGUI(ZaynMemory* zaynMem)
{
    VkDescriptorPoolSize pool_sizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;



    //VkDescriptorPool imguiPool;
    vkCreateDescriptorPool(zaynMem->vulkan.vkDevice, &pool_info, nullptr, &zaynMem->myIMGUI.imGuiDescriptorPool);
     {
        // Attachment
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = zaynMem->vulkan.vkSwapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // No MSAA
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // Color VkAttachmentReference our render pass needs.
        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // subpass
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        // synchronization and dependency
        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0; // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        info.attachmentCount = 1;
        info.pAttachments = &colorAttachment;
        info.subpassCount = 1;
        info.pSubpasses = &subpass;
        info.dependencyCount = 1;
        info.pDependencies = &dependency;
        if (vkCreateRenderPass(zaynMem->vulkan.vkDevice, &info, nullptr, &zaynMem->myIMGUI.imGuiRenderPass) !=
            VK_SUCCESS) {
            throw std::runtime_error("Could not create Dear ImGui's render pass");
        }
    }
    
    // 2: initialize imgui library

    // this initializes the core structures of imgui

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // enable mouse cursor for ImGui
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.WantCaptureMouse = true;

    io.WantCaptureKeyboard = true;


    // this initializes imgui for SDL

    ImGui_ImplGlfw_InitForVulkan(zaynMem->window, true);

    // this initializes imgui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = zaynMem->vulkan.vkInstance;
    init_info.PhysicalDevice = zaynMem->vulkan.vkPhysicalDevice;
    init_info.Device = zaynMem->vulkan.vkDevice;
    init_info.Queue = zaynMem->vulkan.vkGraphicsQueue;
    init_info.DescriptorPool = zaynMem->myIMGUI.imGuiDescriptorPool;

    init_info.RenderPass = zaynMem->myIMGUI.imGuiRenderPass;
    {
        // Already sized to the minImageCount + 1 &&  < MaxImageCount
        std::uint32_t imageCount = static_cast<uint32_t>(zaynMem->vulkan.vkSwapChainImages.size());
        init_info.MinImageCount = imageCount;
        init_info.ImageCount = imageCount;
    }

    ImGui_ImplVulkan_Init(&init_info);

    ImGui_ImplVulkan_CreateFontsTexture();

}  

void CheckCanAddObject(ZaynMemory* zaynMem)
{ 
    if (zaynMem->le.materialSelected)
    {
        zaynMem->le.canAddObject = true;
    }
    else 
    {
        zaynMem->le.canAddObject = false;
    }
}


void ZaynEngine::LevelEditorIMGUI(ZaynMemory* zaynMem)
{
    ImGui::Begin("Level Editor");
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

    {

        ImGui::BeginChild("Scene Hierarchy", ImVec2(200, 300), true);
        for (int i = 0; i < zaynMem->gameObjects.size(); i++)
        {
            bool isSelected;
            if (zaynMem->le.selectedSceneObjectIndex == i)
            {
                isSelected = true;
            }
            else
            {
                isSelected = false;
            }

            if (ImGui::Selectable(zaynMem->gameObjects[i].name.c_str(), isSelected))
            {
                zaynMem->le.selectedSceneObjectIndex = i;
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginGroup();
        {
            ImGui::BeginChild("Object Properties", ImVec2(300, 300), true);
            
            if (zaynMem->le.selectedSceneObjectIndex >= 0 && 
                zaynMem->le.selectedSceneObjectIndex < zaynMem->gameObjects.size())
            {
                GameObject& selectedObj = zaynMem->gameObjects[zaynMem->le.selectedSceneObjectIndex];
                
                ImGui::SeparatorText("Transform");
                ImGui::DragFloat3("Position", glm::value_ptr(selectedObj.transform[3]), 0.1f);
                
                static glm::vec3 rotation;
                ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 1.0f, -180.0f, 180.0f);
                
                static glm::vec3 scale{1.0f};
                ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f);
                
                selectedObj.transform = glm::mat4(1.0f);
                selectedObj.transform = glm::translate(selectedObj.transform, glm::vec3(selectedObj.transform[3]));
                selectedObj.transform = glm::rotate(selectedObj.transform, glm::radians(rotation.x), glm::vec3(1,0,0));
                
                ImGui::SeparatorText("Material");
                ImGui::Text("Current: %s", selectedObj.material->name.c_str());
                if (ImGui::Button("Change Material"))
                {
                    zaynMem->le.selectedNewMaterialIndex = zaynMem->le.selectedSceneObjectIndex;
                    zaynMem->le.materialSelected = true;
                }
            }
            else
            {
                ImGui::Text("Select an object to edit properties");
            }
            
            ImGui::EndChild();
        }
        ImGui::EndGroup();

        ImGui::SeparatorText("New Object Maker");

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
        //ImGui::BeginChild("ChildR", ImVec2(0, 260), ImGuiChildFlags_Borders);


        for (int i = 0; i < zaynMem->materials.size(); i++)
        {
            bool isSelected;
            if (zaynMem->le.selectedNewMaterialIndex == i)
            {
                isSelected = true;
            }
            else
            {
                isSelected = false;
            }

            if (ImGui::Selectable(zaynMem->materials[i].name.c_str(), isSelected))
            {
                zaynMem->le.selectedNewMaterialIndex = i;
                zaynMem->le.materialSelected = true;
            }
        }

        CheckCanAddObject(zaynMem);

        ImGui::Separator();
        ImGui::Text("Create New Object");
        static char newObjectName[128] = "New Object";

        ImGui::InputText("Name", newObjectName, IM_ARRAYSIZE(newObjectName));
        ImGui::BeginDisabled(!zaynMem->le.canAddObject);
        if (ImGui::Button("Add Object") && zaynMem->le.materialSelected)
        {
            GameObject newObj;
            newObj.name = newObjectName;
            newObj.material = &zaynMem->materials[zaynMem->le.selectedNewMaterialIndex];
            newObj.transform = glm::mat4(1.0f);
            newObj.mesh = &zaynMem->mesh_001;
            
            zaynMem->gameObjects.push_back(newObj);
            
            memset(newObjectName, 0, sizeof(newObjectName));
            strcpy_s(newObjectName, "New Object");
        }
        ImGui::EndDisabled();

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Save Level"))
                {
                    // TODO: Implement save functionality
                }
                if (ImGui::MenuItem("Load Level"))
                {
                    // TODO: Implement load functionality
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
        ImGui::End();

    }
}

void ZaynEngine::RenderIMGUI(ZaynMemory* zaynMem)
{

    //ImGui_ImplVulkan_NewFrame();
    //ImGui_ImplGlfw_NewFrame();


    //ImGui::NewFrame();

    ////ImGui::ShowDemoWindow();


    //// Level Editor Controlls
    //ImGui::Begin("Level Editor");

    //ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    //if (ImGui::CollapsingHeader("Level Info"))
    //{

    ////    static int clicked = 0;
    ////    ImGui::SeparatorText("Level Data");
    ////    static char str0[128] = "no level loaded";
    ////    ImGui::LabelText(str0, "Current Level");
    ////    if (ImGui::Button("Save Level Edits"))
    ////    {
    ////        clicked++;
    ////    }
    ////    if (clicked & 1)
    ////    {
    ////        ImGui::SameLine();
    ////        ImGui::Text("Saved!");
    ////    }
    ////    // Load New Level

    ////    char str1[128] = "/Users/socki/dev/zayn/data/levelFiles/level0.txt";
    ////    char str2[128] = "/Users/socki/dev/zayn/data/levelFiles/blender/scene_data.json";
    ////    ImGui::InputText("Level Path", str1, IM_ARRAYSIZE(str1));
    ////    ImGui::SameLine();
    ////    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.1f));
    ////    if (ImGui::Button("Load Level"))
    ////    {
    ////       // LoadLevel_IMGUI(str2);
    ////    }
    ////    ImGui::PopStyleColor();

    //}



    //ImGui::PopStyleVar();
    //ImGui::End();

    // Recording ImGui Command Buffer
    {
        // ImGui Render command
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        //some imgui UI to test
        ImGui::ShowDemoWindow();
        
        ImGui::Begin("Debug Tools");
        if (ImGui::CollapsingHeader("Render Stats"))
        {
            ImGui::Text("Objects: %d", zaynMem->gameObjects.size());
            ImGui::Text("Materials: %d", zaynMem->materials.size());
        }
        ImGui::End();
        
        LevelEditorIMGUI(zaynMem);
        ImGui::Render();

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VkResult err =
            vkBeginCommandBuffer(zaynMem->myIMGUI.imGuiCommandBuffers[zaynMem->vulkan.vkCurrentFrame], &info);
        //check_vk_result(err);

        // Render pass
        {
            VkClearValue clearValue = {};
            clearValue.color = { {0.0f, 0.0f, 0.0f, 1.0f} };
            // Copy passed clear color
            memcpy(&clearValue.color.float32[0], &clear_color, 4 * sizeof(float));

            VkRenderPassBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            info.renderPass = zaynMem->myIMGUI.imGuiRenderPass;
            info.framebuffer = zaynMem->myIMGUI.imGuiFrameBuffers[zaynMem->vulkan.vkCurrentImageIndex];
            // swapChainFramebuffers[currentFrame];
            info.renderArea.extent.width = zaynMem->vulkan.vkSwapChainExtent.width;
            info.renderArea.extent.height = zaynMem->vulkan.vkSwapChainExtent.height;
            info.clearValueCount = 1;
            info.pClearValues = &clearValue;
            vkCmdBeginRenderPass(zaynMem->myIMGUI.imGuiCommandBuffers[zaynMem->vulkan.vkCurrentFrame], &info,
                VK_SUBPASS_CONTENTS_INLINE);
        }

        // ImGui Render command
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), zaynMem->myIMGUI.imGuiCommandBuffers[zaynMem->vulkan.vkCurrentFrame]);

        // Submit command buffer
        vkCmdEndRenderPass(zaynMem->myIMGUI.imGuiCommandBuffers[zaynMem->vulkan.vkCurrentFrame]);
        {
            err = vkEndCommandBuffer(zaynMem->myIMGUI.imGuiCommandBuffers[zaynMem->vulkan.vkCurrentFrame]);
            //check_vk_result(err);
        }
    }
}
