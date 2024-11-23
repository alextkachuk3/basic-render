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

void GlobalState::Initialize(HINSTANCE hInstance, const char* windowTitle, int width, int height, WNDPROC windowCallback = DefWindowProcA)
{
	WNDCLASSA windowClass = {};
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = windowCallback;
	windowClass.hInstance = hInstance;
	windowClass.lpszClassName = "Render";

	if (!RegisterClassA(&windowClass)) InvalidCodePath;

	windowHandle = CreateWindowExA(
		0,
		windowClass.lpszClassName,
		windowTitle,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!windowHandle) InvalidCodePath;

	deviceContext = GetDC(windowHandle);

	RECT clientRect;
	GetClientRect(windowHandle, &clientRect);
	frameBufferWidth = clientRect.right - clientRect.left;
	frameBufferHeight = clientRect.bottom - clientRect.top;

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
