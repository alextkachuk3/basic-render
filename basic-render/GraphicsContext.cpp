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

void GraphicsContext::DrawTriangle(const V3* points, const V3* colors) const
{
	V2 pointA = ProjectPoint(points[0]);
	V2 pointB = ProjectPoint(points[1]);
	V2 pointC = ProjectPoint(points[2]);

	i32 minX = (i32)min(min(pointA.x, pointB.x), pointC.x);
	i32 maxX = (i32)max(max(round(pointA.x), round(pointB.x)), round(pointC.x));
	i32 minY = (i32)min(min(pointA.y, pointB.y), pointC.y);
	i32 maxY = (i32)max(max(round(pointA.y), round(pointB.y)), round(pointC.y));

	minX = max(0, minX);
	minX = min(frameBufferWidth - 1, minX);
	maxX = max(0, maxX);
	maxX = min(frameBufferWidth - 1, maxX);
	minY = max(0, minY);
	minY = min(frameBufferHeight - 1, minY);
	maxY = max(0, maxY);
	maxY = min(frameBufferHeight - 1, maxY);

	V2 edges[] =
	{
		pointB - pointA,
		pointC - pointB,
		pointA - pointC
	};

	bool isTopLeft[] =
	{
		(edges[0].x >= 0.0f && edges[0].y > 0.0f) || (edges[0].x > 0.0f && edges[0].y == 0.0f),
		(edges[1].x >= 0.0f && edges[1].y > 0.0f) || (edges[1].x > 0.0f && edges[1].y == 0.0f),
		(edges[2].x >= 0.0f && edges[2].y > 0.0f) || (edges[2].x > 0.0f && edges[2].y == 0.0f)
	};

	f32 barycentricDiv = V2::CrossProduct(pointB - pointA, pointC - pointA);

	for (i32 Y = minY; Y <= maxY; ++Y)
	{
		for (i32 X = minX; X <= maxX; ++X)
		{
			V2 pixelPoint = V2(X, Y) + V2(0.5f, 0.5f);

			V2 pixelEdges[] =
			{
				pixelPoint - pointA,
				pixelPoint - pointB,
				pixelPoint - pointC
			};

			f32 crossLengths[] =
			{
				V2::CrossProduct(pixelEdges[0], edges[0]),
				V2::CrossProduct(pixelEdges[1], edges[1]),
				V2::CrossProduct(pixelEdges[2], edges[2])
			};

			if ((crossLengths[0] > 0.0f || (isTopLeft[0] && crossLengths[0] == 0.0f)) &&
				(crossLengths[1] > 0.0f || (isTopLeft[1] && crossLengths[1] == 0.0f)) &&
				(crossLengths[2] > 0.0f || (isTopLeft[2] && crossLengths[2] == 0.0f)))
			{
				u32 pixelId = Y * frameBufferWidth + X;

				f32 t0 = -crossLengths[1] / barycentricDiv;
				f32 t1 = -crossLengths[2] / barycentricDiv;
				f32 t2 = -crossLengths[0] / barycentricDiv;
				V3 finalColor = (t0 * colors[0] + t1 * colors[1] + t2 * colors[2]) * 255.0f;

				frameBufferPixels[pixelId] = ((u32)0xFF << 24) | ((u32)finalColor.r << 16) | ((u32)finalColor.g << 8) | (u32)finalColor.b;
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
