

// windows.cpp



#define WINDOWS 1
//#define IMGUI 1

#include <windows.h>



#include <iostream>
#include <GLFW/glfw3.h>

#include "data_types.h"
#include "math/my_math.h"

//#include "../external/imgui/imgui.h"
//#include "../external/imgui/backends/imgui_impl_glfw.h"
//#include "../external/imgui/backends/imgui_impl_vulkan.h"

//#include "imgui.h"
//#include "backends/imgui_impl_glfw.h"
//#include "backends/imgui_impl_vulkan.h"  



#include "zayn.h"
#include "My_IMGUI.h"


#include "my_memory.h"
#include "dynamic_array.h"
#include "input.h"
#include "time.h"
#include "camera.h"




void framebuffer_size_callback(GLFWwindow* window, int width, int height);


struct WindowsPlatform
{
	bool running;
};

#include "globals.h" 
#include "../Engine.h"


ZaynMemory* Zayn = NULL;
InputManager* Input = NULL;


int main()
{
    
    
 /*   Zayn1::Engine engine = {};
    engine.Run();*/
    


    
    


    
    char* sdkPath = nullptr;
    size_t len = 0;

	WindowsPlatform platform = {};

	ZaynMemory zaynMemory = {};
	Zayn = &zaynMemory;

	AllocateMemoryArena(&Zayn->permanentMemArena, Megabytes(256));
	AllocateMemoryArena(&Zayn->frameMemArena, Megabytes(32));

    InputManager* inputManager = &Zayn->inputManager;


    AllocateInputManager(inputManager, &Zayn->permanentMemArena, 4);
    
    Zayn->keyboard = &inputManager->devices[0];
    AllocateInputDevice(Zayn->keyboard, InputDeviceType_Keyboard, Input_KeyboardDiscreteCount, 0);
    
    InitializeKeyMap();

    /* Initialize the library */
    if (!glfwInit())
    {

        return -1;
    }

#if OPENGL
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#elif VULKAN   
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    //Zayn->vulkan = {};
#endif


    Zayn->windowSize.x = 1000;
    Zayn->windowSize.y = 800;

    Zayn->window = glfwCreateWindow(Zayn->windowSize.x, Zayn->windowSize.y, "Zayn Engine", NULL, NULL);
   
    if (!Zayn->window)
    {
        glfwTerminate();
        return -1;
    }


    /* Make the window's context current */
    glfwMakeContextCurrent(Zayn->window);
    glfwSetWindowUserPointer(Zayn->window, Zayn);
    glfwSetFramebufferSizeCallback(Zayn->window, framebuffer_size_callback);


    LARGE_INTEGER startSystemTime;
    LARGE_INTEGER systemTime;
    LARGE_INTEGER systemFrequency;
    QueryPerformanceFrequency(&systemFrequency);
    QueryPerformanceCounter(&systemTime);

    startSystemTime = systemTime;
    SeedRand(startSystemTime.QuadPart);
    Zayn->time.systemTime = (real32)systemTime.QuadPart;
    

    // MAIN INIT CODE FOR RENDERER & GAME
    ZaynInit(Zayn);
    // MAIN INIT CODE FOR RENDERER & GAME


    Zayn->time.startTime = 0.0f;

	platform.running = true;

     
    Zayn->time.systemTime = (real32)systemTime.QuadPart;


    Camera* cam = &Zayn->camera;
    cam->rotationSpeed = 100.0f;
    cam->targetPos = V3(0.0f, 0.0f, 0.0f);

    cam->currentSpeed = 0;
    cam->targetSpeed = 20.0f;
    cam->targetTurnSpeed = 160.0f;

    cam->pos = V3(0, 0, 0.5f);
    cam->front = V3(-1, 0, 0);
    cam->up = V3(0, 0, 1);
    cam->right = V3(0, -1, 0);

#if IMGUI
    ZaynEngine::InitIMGUI(Zayn);
#endif
   
    while (!glfwWindowShouldClose(Zayn->window) && platform.running)
    {
        LARGE_INTEGER prevSystemTime = systemTime;
        int32 error = QueryPerformanceCounter(&systemTime);

        Zayn->time.deltaTime = ((real64)systemTime.QuadPart - (real64)prevSystemTime.QuadPart) / (real64)systemFrequency.QuadPart;

        Zayn->time.totalTime += Zayn->time.deltaTime;
        Zayn->time.frameCount = Zayn->time.frameCount + 1;
        //std::cout << "framecnf: " << Zayn->time.frameCount << std::endl;

         
        Zayn->time.fpsTimer += Zayn->time.deltaTime;

    
        if (Zayn->time.fpsTimer > 0.5f)
        {

            float avgFPS = Zayn->time.frameCount / Zayn->time.fpsTimer;
            std::cout << avgFPS << std::endl;

            Zayn->time.fpsTimer = 0.0f;
            Zayn->time.frameCount = 0;

        }
        InputUpdate(Zayn, &Zayn->inputManager);

        InputCameraMovement(Zayn);
       // ZaynEngine::RenderIMGUI(Zayn);r

        ZaynUpdateAndRender(Zayn);
        
        glfwSwapBuffers(Zayn->window);

        ClearInputManager(inputManager);
    }
}





void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // window = glfwCreateWindow(width, height, "Hello World", nullptr, nullptr);

    // glfwGetWindowUserPointer(window)
    // auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
    auto zaynMem = reinterpret_cast<ZaynMemory*>(glfwGetWindowUserPointer(window));
    std::cout << "fbscb" << std::endl;
    zaynMem->vulkan.vkFramebufferResized = true;
    // auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
        // app->framebufferResized = true;
}