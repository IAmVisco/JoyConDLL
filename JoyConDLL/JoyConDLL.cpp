#include "JoyConDLL.h"

// globals
HRESULT hr;
DIJOYSTATE2 js;
LPDIRECTINPUT8 di;
JOYCONSTATE jc;
JOYCONS joyconsBoth;
DIDEVCAPS capabilities;
std::vector<LPDIRECTINPUTDEVICE8> joycons;

HRESULT tryConnect()
{
    joycons.clear();
    // Create a DirectInput device
    if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
        IID_IDirectInput8, (VOID**)&di, NULL))) {
        return hr;
    }
    // Look for the first simple joystick we can find.
    if (FAILED(hr = di->EnumDevices(DI8DEVCLASS_GAMECTRL, enumCallback,
        NULL, DIEDFL_ATTACHEDONLY))) {
        return hr;
    }
    if (joycons.size() < 1)
        return E_FAIL;
    return S_OK;
}

JOYCONS pollJoycons()
{
    int num = 0;
    for (auto joycon : joycons)
    {
        jc.Status = joyStickPoll(&js, joycon);
        if (!FAILED(jc.Status))
        {
            if (num++ == 0)
                joyconsBoth.Left = mapData(js);
            else
                joyconsBoth.Right = mapData(js);
        }
    }
    if (num == 1)
        joyconsBoth.Right.Status = E_FAIL;
    return joyconsBoth;
}

size_t getJoyconsAmount()
{
    return joycons.size();
}

JOYCONSTATE mapData(DIJOYSTATE2 js)
{
    jc.POV = js.rgdwPOV[0];
    jc.A = js.rgbButtons[0];
    jc.X = js.rgbButtons[1];
    jc.B = js.rgbButtons[2];
    jc.Y = js.rgbButtons[3];
    jc.SL = js.rgbButtons[4];
    jc.SR = js.rgbButtons[5];
    jc.Minus = js.rgbButtons[8];
    jc.Plus = js.rgbButtons[9];
    return jc;
}

BOOL CALLBACK enumCallback(const DIDEVICEINSTANCE* instance, VOID* context)
{
    LPDIRECTINPUTDEVICE8 joycon;
    if (FAILED(di->CreateDevice(instance->guidInstance, &joycon, NULL)))
        return DIENUM_CONTINUE;
    else
    {
        if (FAILED(hr = joycon->SetDataFormat(&c_dfDIJoystick2))) {
            throw "Cant set data format";
        }
        capabilities.dwSize = sizeof(DIDEVCAPS);
        if (FAILED(hr = joycon->GetCapabilities(&capabilities))) {
            throw "Cant get capabilities";
        }
        joycons.push_back(joycon);
        return DIENUM_CONTINUE;
    }
}

HRESULT joyStickPoll(DIJOYSTATE2 *js, LPDIRECTINPUTDEVICE8 joystick)
{
    hr = joystick->Poll();
    if (FAILED(hr)) {
        hr = joystick->Acquire();
        while (hr == DIERR_INPUTLOST) {
            hr = joystick->Acquire();
        }

        if ((hr == DIERR_INVALIDPARAM) || (hr == DIERR_NOTINITIALIZED)) {
            return E_FAIL;
        }

        // If another application has control of this device, return successfully.
        // We'll just have to wait our turn to use the joystick.
        if (hr == DIERR_OTHERAPPHASPRIO) {
            return S_OK;
        }
    }

    if (FAILED(hr = joystick->GetDeviceState(sizeof(DIJOYSTATE2), js))) {
        return hr; // The device should have been acquired during the Poll()
    }

    return S_OK;
}