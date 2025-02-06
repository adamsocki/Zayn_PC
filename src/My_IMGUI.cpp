#include "My_IMGUI.h"





//#include <imgui_impl_vulkan.h>
//#include <imgui_impl_glfw.h>
#include <stdexcept>



struct ZaynMemory;
#include "zayn.h"
#include "../external/imgui/imgui.h"
#include "../external/imgui/backends/imgui_impl_glfw.h"
#include "../external/imgui/backends/imgui_impl_vulkan.h"
#include "render/render_vulkan.h"


void ZaynEngine::InitIMGUI(ZaynMemory* zaynMem)
{


    VkDescriptorPoolCreateInfo poolInfo{};
    // ImGui Descriptor pool
    VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000} };
    poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 1;
    poolInfo.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    poolInfo.pPoolSizes = pool_sizes;
    if (vkCreateDescriptorPool(zaynMem->vulkan.vkDevice, &poolInfo, nullptr,
        &zaynMem->myIMGUI.imGuiDescriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create imgui descriptor pool!");
    }

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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // enable mouse cursor for ImGui
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.WantCaptureMouse = true;
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForVulkan(zaynMem->window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = zaynMem->vulkan.vkInstance;
    init_info.PhysicalDevice = zaynMem->vulkan.vkPhysicalDevice;
    init_info.Device = zaynMem->vulkan.vkDevice;
    init_info.QueueFamily = zaynMem->vulkan.vkQueueFamilyCount;
    init_info.Queue = zaynMem->vulkan.vkGraphicsQueue;
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = zaynMem->myIMGUI.imGuiDescriptorPool;
    init_info.Allocator = nullptr;
    init_info.RenderPass = zaynMem->myIMGUI.imGuiRenderPass;

    {
        // Already sized to the minImageCount + 1 &&  < MaxImageCount
        std::uint32_t imageCount = static_cast<uint32_t>(zaynMem->vulkan.vkSwapChainImages.size());
        init_info.MinImageCount = imageCount;
        init_info.ImageCount = imageCount;
    }
    // init_info.MinImageCount = g_MinImageCount;
    // init_info.ImageCount = wd->ImageCount;

    //// init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info);

    // Create dedicated command pool/buffer for font upload
    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = zaynMem->vulkan.vkQueueFamilyCount;
    vkCreateCommandPool(zaynMem->vulkan.vkDevice, &pool_info, nullptr, &zaynMem->myIMGUI.imGuiFontCommandPool);

    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = zaynMem->myIMGUI.imGuiFontCommandPool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;
    vkAllocateCommandBuffers(zaynMem->vulkan.vkDevice, &alloc_info, &zaynMem->myIMGUI.imGuiFontCommandBuffer);

    // Begin command buffer
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(zaynMem->myIMGUI.imGuiFontCommandBuffer, &begin_info);

    // Create font texture with our dedicated command buffer
    ImGui_ImplVulkan_CreateFontsTexture();

    // End and submit
    vkEndCommandBuffer(zaynMem->myIMGUI.imGuiFontCommandBuffer);
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &zaynMem->myIMGUI.imGuiFontCommandBuffer;
    vkQueueSubmit(zaynMem->vulkan.vkGraphicsQueue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(zaynMem->vulkan.vkGraphicsQueue);

    // Cleanup
    vkFreeCommandBuffers(zaynMem->vulkan.vkDevice, zaynMem->myIMGUI.imGuiFontCommandPool, 1, &zaynMem->myIMGUI.imGuiFontCommandBuffer);
    vkDestroyCommandPool(zaynMem->vulkan.vkDevice, zaynMem->myIMGUI.imGuiFontCommandPool, nullptr);

}

void ZaynEngine::RenderIMGUI(ZaynMemory* zaynMem)
{

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();


    ImGui::NewFrame();

    ImGui::ShowDemoWindow();


    // Level Editor Controlls
    ImGui::Begin("Level Editor");

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    if (ImGui::CollapsingHeader("Level Info"))
    {

        static int clicked = 0;
        ImGui::SeparatorText("Level Data");
        static char str0[128] = "no level loaded";
        ImGui::LabelText(str0, "Current Level");
        if (ImGui::Button("Save Level Edits"))
        {
            clicked++;
        }
        if (clicked & 1)
        {
            ImGui::SameLine();
            ImGui::Text("Saved!");
        }
        // Load New Level

        char str1[128] = "/Users/socki/dev/zayn/data/levelFiles/level0.txt";
        char str2[128] = "/Users/socki/dev/zayn/data/levelFiles/blender/scene_data.json";
        ImGui::InputText("Level Path", str1, IM_ARRAYSIZE(str1));
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.1f));
        if (ImGui::Button("Load Level"))
        {
           // LoadLevel_IMGUI(str2);
        }
        ImGui::PopStyleColor();

    }



    ImGui::PopStyleVar();
    ImGui::End();
}
