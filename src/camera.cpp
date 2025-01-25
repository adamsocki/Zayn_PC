
#include "camera.h"
#include "zayn.h"



void InputCameraMovement(ZaynMemory* zaynMem)
{
    Camera* cam = &zaynMem->camera;

    if (InputHeld(zaynMem->keyboard, Input_W))
    {
        cam->pos = cam->pos + cam->front * 15.0f * zaynMem->time.deltaTime;
    }
    if (InputHeld(zaynMem->keyboard, Input_S))
    {
        cam->pos = cam->pos - cam->front * 15.0f * zaynMem->time.deltaTime;
    }

    if (InputHeld(zaynMem->keyboard, Input_D))
    {
        cam->pos = cam->pos + cam->right * 15.0f * zaynMem->time.deltaTime;
    }
    if (InputHeld(zaynMem->keyboard, Input_A))
    {
        cam->pos = cam->pos - cam->right * 15.0f * zaynMem->time.deltaTime;
    }

    if (InputHeld(zaynMem->keyboard, Input_R))
    {
        cam->pos = cam->pos - cam->up * 15.0f * zaynMem->time.deltaTime;
    }
    if (InputHeld(zaynMem->keyboard, Input_F))
    {
        cam->pos = cam->pos - cam->up * 15.0f * zaynMem->time.deltaTime;
    }

    // rotate camera aroudn the x y and z axis using inputheld and other keys
    if (InputHeld(zaynMem->keyboard, Input_Q))
    {
        cam->yaw += cam->rotationSpeed * zaynMem->time.deltaTime;
    }
    if (InputHeld(zaynMem->keyboard, Input_E))
    {
        cam->yaw -= cam->rotationSpeed * zaynMem->time.deltaTime;
    }

    if (InputHeld(zaynMem->keyboard, Input_T))
    {
        cam->pitch += cam->rotationSpeed * zaynMem->time.deltaTime;
    }
    if (InputHeld(zaynMem->keyboard, Input_G))
    {
        cam->pitch -= cam->rotationSpeed * zaynMem->time.deltaTime;
    }

    if (InputHeld(zaynMem->keyboard, Input_Y))
    {
        cam->roll += cam->rotationSpeed * zaynMem->time.deltaTime;
    }
    if (InputHeld(zaynMem->keyboard, Input_H))
    {
        cam->roll -= cam->rotationSpeed * zaynMem->time.deltaTime;
    }
}