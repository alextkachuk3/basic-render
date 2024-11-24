#include <Windows.h>
#include <cmath>
#include <vector>
#include "AssertUtils.h"
#include "GraphicsContext.h"
#include "V2.h"
#include "V3.h"

static const f32 pi = 3.14159265359f;

static GraphicsContext graphicsContext;

static LRESULT CALLBACK Win32WindowCallBack(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		graphicsContext.SetIsRunning(false);
		return 0;
	default:
		return DefWindowProcA(windowHandle, message, wParam, lParam);
	}
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	graphicsContext.Initialize(hInstance, "Render", 1920, 1080, Win32WindowCallBack);
	graphicsContext.SetIsRunning(true);

	LARGE_INTEGER timerFrequency;
	QueryPerformanceFrequency(&timerFrequency);

	LARGE_INTEGER beginTime;
	QueryPerformanceCounter(&beginTime);

	const u32 blockSize = 3;
	const f32 speed = 0.75f;
	f32 currentTime = -2.0f * pi;

	while (graphicsContext.IsRunning())
	{
		LARGE_INTEGER endTime;
		QueryPerformanceCounter(&endTime);
		f32 frameTime = (f32)(endTime.QuadPart - beginTime.QuadPart) / timerFrequency.QuadPart;
		beginTime = endTime;

		u32* pixels = graphicsContext.GetFrameBufferPixels();
		f32* zBuffer = graphicsContext.GetZBuffer();
		u32 width = graphicsContext.GetFrameBufferWidth();
		u32 height = graphicsContext.GetFrameBufferHeight();

		for (u32 y = 0; y < height; y++)
		{
			for (u32 x = 0; x < width; x++)
			{
				u8 red = 0;
				u8 green = 0;
				u8 blue = 0;
				u8 alpha = 255;
				u32 color = ((u32)alpha << 24) | ((u32)red << 16) | ((u32)green << 8) | (u32)blue;

				u32 pixelIndex = y * width + x;
				pixels[pixelIndex] = color;
				zBuffer[pixelIndex] = FLT_MAX;
			}
		}

		V3 Positions1[] =
		{
			V3(-0.2f, 0.6f, 1.1f),
			V3(0.7f, -0.4f, 0.9f),
			V3(-0.7f, -0.4f, 0.9f),
		};

		V3 Colors1[] =
		{
			V3(0.9f, 0.1f, 0.1f),
			V3(0.1f, 0.9f, 0.1f),
			V3(0.1f, 0.1f, 0.9f),
		};

		V3 Positions2[] =
		{
			V3(-0.2f, 0.6f, 1.1f),
			V3(0.7f, -0.4f, 1.3f),
			V3(-0.7f, -0.4f, 0.7f),
		};

		V3 Colors2[] =
		{
			V3(1.0f, 1.0f, 0.0f),
			V3(0.0f, 1.0f, 1.0f),
			V3(1.0f, 0.0f, 1.0f),
		};

		graphicsContext.DrawTriangle(Positions2, Colors2);
		graphicsContext.DrawTriangle(Positions1, Colors1);

		currentTime += frameTime * speed;

		if (currentTime >= 2.0f * pi)
			currentTime = -2.0f * pi;

		graphicsContext.ProcessSystemMessages();
		graphicsContext.RenderFrame();
	}

	graphicsContext.ReleaseResources();
	return 0;
}
