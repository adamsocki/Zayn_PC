// input.cpp
#include <iostream>

#include "input.h"
#include "zayn.h"
#include "globals.h"
#include "dynamic_array.h"



InputKeyboardDiscrete glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LAST + 1];
//struct ZaynMemory;


void InitializeKeyMap() {
    // Initialize all keys to Input_KeyboardDiscreteCount (invalid key)
    for (int i = 0; i <= GLFW_KEY_LAST; ++i) {
        glfwKeyToInputKeyboardDiscrete[i] = Input_KeyboardDiscreteCount;
    }

    // Map GLFW keys to InputKeyboardDiscrete enum values
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_A] = Input_A;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_B] = Input_B;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_C] = Input_C;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_D] = Input_D;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_E] = Input_E;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F] = Input_F;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_G] = Input_G;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_H] = Input_H;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_I] = Input_I;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_J] = Input_J;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_K] = Input_K;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_L] = Input_L;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_M] = Input_M;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_N] = Input_N;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_O] = Input_O;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_P] = Input_P;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_Q] = Input_Q;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_R] = Input_R;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_S] = Input_S;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_T] = Input_T;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_U] = Input_U;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_V] = Input_V;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_W] = Input_W;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_X] = Input_X;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_Y] = Input_Y;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_Z] = Input_Z;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_0] = Input_0;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_1] = Input_1;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_2] = Input_2;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_3] = Input_3;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_4] = Input_4;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_5] = Input_5;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_6] = Input_6;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_7] = Input_7;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_8] = Input_8;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_9] = Input_9;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_EQUAL] = Input_Equal;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_MINUS] = Input_Minus;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT_BRACKET] = Input_RightBracket;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT_BRACKET] = Input_LeftBracket;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_APOSTROPHE] = Input_Quote;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_SEMICOLON] = Input_Semicolon;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_BACKSLASH] = Input_Backslash;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_COMMA] = Input_Comma;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_SLASH] = Input_Slash;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_PERIOD] = Input_Period;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_ENTER] = Input_Return;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_TAB] = Input_Tab;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_SPACE] = Input_Space;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_BACKSPACE] = Input_Backspace;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_ESCAPE] = Input_Escape;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_GRAVE_ACCENT] = Input_Tick;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT_SUPER] = Input_Win;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT_SHIFT] = Input_Shift;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_CAPS_LOCK] = Input_CapsLock;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT_ALT] = Input_Alt;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT_CONTROL] = Input_Control;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT_SUPER] = Input_RightWin;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT_SHIFT] = Input_RightShift;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT_ALT] = Input_RightAlt;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT_CONTROL] = Input_RightControl;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F1] = Input_F1;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F2] = Input_F2;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F3] = Input_F3;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F4] = Input_F4;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F5] = Input_F5;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F6] = Input_F6;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F7] = Input_F7;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F8] = Input_F8;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F9] = Input_F9;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F10] = Input_F10;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F11] = Input_F11;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F12] = Input_F12;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F13] = Input_F13;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F14] = Input_F14;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F15] = Input_F15;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F16] = Input_F16;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F17] = Input_F17;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F18] = Input_F18;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F19] = Input_F19;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F20] = Input_F20;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F21] = Input_F21;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F22] = Input_F22;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F23] = Input_F23;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F24] = Input_F24;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_HOME] = Input_Home;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_INSERT] = Input_Insert;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_PAGE_UP] = Input_PageUp;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_DELETE] = Input_ForwardDelete;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_END] = Input_End;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_PAGE_DOWN] = Input_PageDown;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT] = Input_LeftArrow;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT] = Input_RightArrow;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_DOWN] = Input_DownArrow;
    glfwKeyToInputKeyboardDiscrete[GLFW_KEY_UP] = Input_UpArrow;
}


void AllocateInputManager(InputManager* inputManager, MemoryArena* arena, int32 deviceCapacity)
{
    inputManager->events = MakeDynamicArray<InputEvent>(arena, deviceCapacity);

    inputManager->deviceCount = deviceCapacity;
    inputManager->devices = (InputDevice*)malloc(sizeof(InputDevice) * deviceCapacity);
    memset(inputManager->devices, 0, inputManager->deviceCount * sizeof(InputDevice));


}

void AllocateInputDevice(InputDevice *device, InputDeviceType type, int32 discreteCount, int32 analogueCount)
{
    device->type = type;
    device->discreteCount = discreteCount;

    device->framesHeld = (int32 *)malloc(sizeof(int32) * discreteCount);
    device->released = (bool *)malloc(sizeof(bool) * discreteCount);
    device->pressed = (bool *)malloc(sizeof(bool) * discreteCount);
    device->timePressed = (real32 *)malloc(sizeof(real32) * discreteCount);

    device->analogueCount = analogueCount;
    device->prevAnalogue = (real32 *)malloc(sizeof(real32) * analogueCount);
    device->analogue = (real32 *)malloc(sizeof(real32) * analogueCount);

    memset(device->framesHeld, -1, sizeof(int32) * discreteCount);
    memset(device->pressed, 0, sizeof(bool) * discreteCount);
    memset(device->released, 0, sizeof(bool) * discreteCount);
    memset(device->timePressed, 0, sizeof(real32) * discreteCount);

    memset(device->analogue, 0, sizeof(real32) * analogueCount);
    memset(device->prevAnalogue, 0, sizeof(real32) * analogueCount);
}



bool InputHeld(InputDevice* device, int32 inputID)
{
    return device->framesHeld[inputID] > 0;
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    InputKeyboardDiscrete inputKey = glfwKeyToInputKeyboardDiscrete[key];
    if (action == GLFW_PRESS)
    {
        InputEvent event = {};
        event.device = Zayn->keyboard;
        event.index = inputKey;
        if (key == GLFW_KEY_A)
        {
            // std::cout << "A Is for Asshole: " << key << std::endl;

        }
        event.discreteValue = true;

        PushBack(&Zayn->inputManager.events, event);

        // std::cout << "Key pressed: " << key << std::endl;
    }
    else if (action == GLFW_RELEASE)
    {
        InputEvent event = {};
        event.device = Zayn->keyboard;
        event.index = inputKey;
        event.discreteValue = false;
        std::cout << "Key released: " << key << std::endl;

        PushBack(&Zayn->inputManager.events, event);

    }

#if IMGUI
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
#endif
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
#if IMGUI
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
#endif
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    // std::cout << "Key pressed: " << xpos << std::endl;
#if IMGUI
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
#endif

}

void InputUpdate(ZaynMemory* zaynMem, InputManager* inputManager)
{
    glfwPollEvents();


    glfwSetKeyCallback(zaynMem->window, keyCallback);
    glfwSetMouseButtonCallback(zaynMem->window, mouseButtonCallback);
    glfwSetCursorPosCallback(zaynMem->window, cursorPosCallback);


    for (int d = 0; d < inputManager->deviceCount; d++)
    {
        InputDevice* device = &inputManager->devices[d];

        if (device->type == InputDeviceType_Invalid) { continue; }

        for (int i = 0; i < device->discreteCount; i++)
        {
            device->released[i] = false;
             //std::cout << "device->discretecount: " << i << std::endl;

            if (device->framesHeld[i] >= 0)
            {
                 //std::cout << "device->discretecount: " << i << std::endl;
                 //std::cout << "device->framesHeld[i]++: " << device->framesHeld[i] << std::endl;
                device->framesHeld[i]++;
                device->pressed[i] = false;
            }
        }
    }

    for (int i = 0; i < inputManager->events.count; i++)
    {
        // std::cout << "inputManager->events.count: " << i << std::endl;

        InputEvent event = inputManager->events[i];
        int32 index = event.index;

        InputDevice* device = event.device;

        if (!event.discreteValue)
        {
            if (device->framesHeld[index] > 0)
            {
                device->released[index] = true;
            }
            device->timePressed[index] = -1;
            device->framesHeld[index] = -1;
            device->pressed[index] = false;
        }
        else
        {
            if (device->framesHeld[index] < 0)
            {
                 printf("pressed\n");
                std::cout << "device->framesHeld" << std::endl;

                device->timePressed[index] = zaynMem->time.zaynTime;
                device->framesHeld[index] = 0;
                device->pressed[index] = true;
                device->released[index] = false;
            }
            else
            {
                // @NOTE: we shouldnt even get a pressed event when the key is being held
            }
        }
    }
}

// @NOTE: to be cleared at the end of the frame so we have access to inputChars thruout update. 
void ClearInputManager(InputManager* input) 
{
    DynamicArrayClear(&input->events);

    input->charCount = 0;
    //memset(input->inputChars, 0, input->charSize);
}


//void InputUpdate(ZaynMemory* zaynMem, InputManager* inputManager)
//{
//
//}