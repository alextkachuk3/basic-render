#include "GlobalState.h"

GlobalState::GlobalState()
    : windowHandle(nullptr),
    deviceContext(nullptr),
    frameBufferWidth(0),
    frameBufferHeight(0),
    frameBufferPixels(nullptr),
    curOffset(0.0f),
    isRunning(false)
{
}

GlobalState::~GlobalState()
{
    ReleaseResources();
}

void GlobalState::Initialize(HWND window, HDC context, u32 width, u32 height)
{
    windowHandle = window;
    deviceContext = context;
    frameBufferWidth = width;
    frameBufferHeight = height;
    AllocateFrameBuffer();
}

void GlobalState::AllocateFrameBuffer()
{
    frameBufferPixels = static_cast<u32*>(malloc(sizeof(u32) * frameBufferWidth * frameBufferHeight));
    Assert(frameBufferPixels);
}

void GlobalState::ReleaseResources()
{
    if (deviceContext && windowHandle)
        ReleaseDC(windowHandle, deviceContext);
    if (frameBufferPixels)
    {
        free(frameBufferPixels);
        frameBufferPixels = nullptr;
    }
}

HWND GlobalState::GetWindowHandle() const
{
    return windowHandle;
}

void GlobalState::SetWindowHandle(HWND handle)
{
    windowHandle = handle;
}

HDC GlobalState::GetDeviceContext() const
{
    return deviceContext;
}

void GlobalState::SetDeviceContext(HDC context)
{
    deviceContext = context;
}

u32 GlobalState::GetFrameBufferWidth() const
{
    return frameBufferWidth;
}

void GlobalState::SetFrameBufferWidth(u32 width)
{
    frameBufferWidth = width;
}

u32 GlobalState::GetFrameBufferHeight() const
{
    return frameBufferHeight;
}

void GlobalState::SetFrameBufferHeight(u32 height)
{
    frameBufferHeight = height;
}

u32* GlobalState::GetFrameBufferPixels() const
{
    return frameBufferPixels;
}

float GlobalState::GetCurOffset() const
{
    return curOffset;
}

void GlobalState::SetCurOffset(float offset)
{
    curOffset = offset;
}

bool GlobalState::IsRunning() const
{
    return isRunning;
}

void GlobalState::SetIsRunning(bool running)
{
    isRunning = running;
}
