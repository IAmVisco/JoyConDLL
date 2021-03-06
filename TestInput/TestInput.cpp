#include <windows.h>
#include <dinput.h>
#include <iostream>

HRESULT TryConnect();
void PollInput();
BOOL CALLBACK enumCallback(const DIDEVICEINSTANCE* instance, VOID* context);
HRESULT JoyStickProp();
HRESULT JoyStickPoll(DIJOYSTATE2 *js);

typedef struct JOYCONSTATE {
    DWORD   POV;
    int     A;
    int     B;
    int     Y;
    int     X;
};
// globals
HRESULT hr;
DIJOYSTATE2 js;
LPDIRECTINPUT8 di;
JOYCONSTATE jc;
DIDEVCAPS capabilities;
LPDIRECTINPUTDEVICE8 joystick;
int connRes = -1;

int main()
{
    while (true)
    {
        if (connRes != 0)
        {
            std::cout << "Trying...\n";
            connRes = TryConnect();
        }
        else
        {
            PollInput();
            for (int i = 0; i < 128; i++)
            {
                if (js.rgbButtons[i] == 128)
                    std::cout << i << "\n";
            }
        }
    }
}

HRESULT TryConnect()
{
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
    // Make sure we got a joystick
    if (joystick == NULL) {
        return E_FAIL;
    }
    JoyStickProp();
    return S_OK;
}

void PollInput()
{
    if (!FAILED(JoyStickPoll(&js)))
    {
        
    }
}

BOOL CALLBACK enumCallback(const DIDEVICEINSTANCE* instance, VOID* context)
{
    HRESULT hr;
    hr = di->CreateDevice(instance->guidInstance, &joystick, NULL);
    if (FAILED(hr)) {
        return DIENUM_CONTINUE;
    }
    return DIENUM_STOP;
}

HRESULT JoyStickProp()
{
    if (FAILED(hr = joystick->SetDataFormat(&c_dfDIJoystick2))) {
        return hr;
    }
    if (FAILED(hr = joystick->SetCooperativeLevel(NULL, DISCL_EXCLUSIVE | DISCL_FOREGROUND))) {
        return hr;
    }
    capabilities.dwSize = sizeof(DIDEVCAPS);
    if (FAILED(hr = joystick->GetCapabilities(&capabilities))) {
        return hr;
    }
    return S_OK;
}

HRESULT JoyStickPoll(DIJOYSTATE2 *js)
{
    HRESULT hr;

    if (joystick == NULL) {
        return S_OK;
    }

    // Poll the device to read the current state
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

    // Get the input's device state
    if (FAILED(hr = joystick->GetDeviceState(sizeof(DIJOYSTATE2), js))) {
        return hr; // The device should have been acquired during the Poll()
    }

    return S_OK;
}
