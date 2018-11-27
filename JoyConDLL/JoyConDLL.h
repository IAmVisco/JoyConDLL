#pragma once
#include <windows.h>
#include <dinput.h>
#include <iostream>
#include <vector>

typedef struct JOYCONSTATE {
    HRESULT Status;
    DWORD   POV;
    int     A;
    int     B;
    int     Y;
    int     X;
    int     Plus;
    int     SL;
    int     SR;
    int     JoystickClick;
};

typedef struct JOYCONS {
    JOYCONSTATE Left;
    JOYCONSTATE Right;
};

BOOL CALLBACK enumCallback(const DIDEVICEINSTANCE* instance, VOID* context);
HRESULT joyStickPoll(DIJOYSTATE2 *js, LPDIRECTINPUTDEVICE8 joystick);
JOYCONSTATE mapData(DIJOYSTATE2 js);

extern "C" {
    __declspec(dllexport)  HRESULT tryConnect();
    __declspec(dllexport)  JOYCONS pollJoycons();
    __declspec(dllexport)  size_t getJoyconsAmount();
}
