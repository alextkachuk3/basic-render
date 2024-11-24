#include "GraphicsContext.h"

GraphicsContext::GraphicsContext()
	: windowHandle(nullptr),
	deviceContext(nullptr),
	frameBufferWidth(0),
	frameBufferHeight(0),
	frameBufferPixels(nullptr),
	curAngle(0.0f),
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
	windowClass.lpszClassName = windowTitle;

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

void GraphicsContext::ProcessSystemMessages()
{
	MSG message;
	while (PeekMessageA(&message, windowHandle, 0, 0, PM_REMOVE))
	{
		if (message.message == WM_QUIT)
			SetIsRunning(false);
		else
		{
			TranslateMessage(&message);
			DispatchMessageA(&message);
		}
	}
}

void GraphicsContext::RenderFrame() const
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

V2 GraphicsContext::ProjectPoint(V3 pos) const
{
	return 0.5f * (pos.xy / pos.z + V2(1)) * V2((f32)GetFrameBufferWidth(), (f32)GetFrameBufferHeight());
}

void GraphicsContext::DrawTriangle(const V3* points, u32 pixelColor)
{
	V2 pointA = ProjectPoint(points[0]);
	V2 pointB = ProjectPoint(points[1]);
	V2 pointC = ProjectPoint(points[2]);

	V2 edge0 = pointB - pointA;
	V2 edge1 = pointC - pointB;
	V2 edge2 = pointA - pointC;

	bool isTopLeft0 = (edge0.x >= 0.0f && edge0.y > 0.0f) || (edge0.x > 0.0f && edge0.y == 0.0f);
	bool isTopLeft1 = (edge1.x >= 0.0f && edge1.y > 0.0f) || (edge1.x > 0.0f && edge1.y == 0.0f);
	bool isTopLeft2 = (edge2.x >= 0.0f && edge2.y > 0.0f) || (edge2.x > 0.0f && edge2.y == 0.0f);

	for (u32 y = 0; y < GetFrameBufferHeight(); y++)
	{
		for (u32 x = 0; x < GetFrameBufferWidth(); x++)
		{
			V2 pixelPoint = V2(x, y) + V2(0.5f, 0.5f);

			V2 pixelEdge0 = pixelPoint - pointA;
			V2 pixelEdge1 = pixelPoint - pointB;
			V2 pixelEdge2 = pixelPoint - pointC;

			f32 crossLength0 = V2::CrossProduct2d(pixelEdge0, edge0);
			f32 crossLength1 = V2::CrossProduct2d(pixelEdge1, edge1);
			f32 crossLength2 = V2::CrossProduct2d(pixelEdge2, edge2);

			if ((crossLength0 > 0.0f || (isTopLeft0 && crossLength0 == 0.0f)) &&
				(crossLength1 > 0.0f || (isTopLeft1 && crossLength1 == 0.0f)) &&
				(crossLength2 > 0.0f || (isTopLeft2 && crossLength2 == 0.0f)))
			{
				u32 pixelId = y * GetFrameBufferWidth() + x;
				GetFrameBufferPixels()[pixelId] = pixelColor;
			}
		}
	}
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

bool GraphicsContext::IsRunning() const
{
	return isRunning;
}

void GraphicsContext::SetIsRunning(bool running)
{
	isRunning = running;
}
