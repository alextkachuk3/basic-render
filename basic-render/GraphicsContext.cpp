#include "GraphicsContext.h"

GraphicsContext::GraphicsContext()
	: windowHandle(nullptr),
	deviceContext(nullptr),
	frameBufferWidth(0),
	frameBufferHeight(0),
	frameBufferPixels(nullptr),
	curOffset(0.0f),
	isRunning(false)
{
}

GraphicsContext::~GraphicsContext()
{
	ReleaseResources();
}

void GraphicsContext::Initialize(HINSTANCE hInstance, const char* windowTitle, int width, int height, WNDPROC windowCallback = DefWindowProcA)
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

void GraphicsContext::AllocateFrameBuffer()
{
	frameBufferPixels = static_cast<u32*>(malloc(sizeof(u32) * frameBufferWidth * frameBufferHeight));
	Assert(frameBufferPixels);
}

void GraphicsContext::ReleaseResources()
{
	if (deviceContext && windowHandle)
		ReleaseDC(windowHandle, deviceContext);
	if (frameBufferPixels)
	{
		free(frameBufferPixels);
		frameBufferPixels = nullptr;
	}
}

void GraphicsContext::RenderFrame()
{
	RECT clientRect = {};
	Assert(GetClientRect(windowHandle, &clientRect));
	uint32_t clientWidth = clientRect.right - clientRect.left;
	uint32_t clientHeight = clientRect.bottom - clientRect.top;

	BITMAPINFO bitmapInfo = {};
	bitmapInfo.bmiHeader.biSize = sizeof(tagBITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth = frameBufferWidth;
	bitmapInfo.bmiHeader.biHeight = frameBufferHeight;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	Assert(StretchDIBits(
		deviceContext,
		0, 0,
		clientWidth, clientHeight,
		0, 0,
		frameBufferWidth, frameBufferHeight,
		frameBufferPixels,
		&bitmapInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	));
}


HWND GraphicsContext::GetWindowHandle() const
{
	return windowHandle;
}

void GraphicsContext::SetWindowHandle(HWND handle)
{
	windowHandle = handle;
}

HDC GraphicsContext::GetDeviceContext() const
{
	return deviceContext;
}

void GraphicsContext::SetDeviceContext(HDC context)
{
	deviceContext = context;
}

u32 GraphicsContext::GetFrameBufferWidth() const
{
	return frameBufferWidth;
}

void GraphicsContext::SetFrameBufferWidth(u32 width)
{
	frameBufferWidth = width;
}

u32 GraphicsContext::GetFrameBufferHeight() const
{
	return frameBufferHeight;
}

void GraphicsContext::SetFrameBufferHeight(u32 height)
{
	frameBufferHeight = height;
}

u32* GraphicsContext::GetFrameBufferPixels() const
{
	return frameBufferPixels;
}

float GraphicsContext::GetCurOffset() const
{
	return curOffset;
}

void GraphicsContext::SetCurOffset(float offset)
{
	curOffset = offset;
}

bool GraphicsContext::IsRunning() const
{
	return isRunning;
}

void GraphicsContext::SetIsRunning(bool running)
{
	isRunning = running;
}
