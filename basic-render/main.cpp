#include <Windows.h>
#include <cmath>
#include "AssertUtils.h"
#include "GraphicsContext.h"
#include "V2.h"
#include "V3.h"

static float pi = 3.14159265359f;

static GraphicsContext graphicsContext;

V2 ProjectPoint(V3 Pos)
{
	V2 Result = Pos.xy / Pos.z;
	Result = 0.5f * (Result + V2(1)) * V2((float)graphicsContext.GetFrameBufferWidth(), (float)graphicsContext.GetFrameBufferHeight());
	return Result;
}

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
	graphicsContext.Initialize(hInstance, "Render", 1280, 720, Win32WindowCallBack);
	graphicsContext.SetIsRunning(true);

	LARGE_INTEGER timerFrequency;
	QueryPerformanceFrequency(&timerFrequency);

	LARGE_INTEGER beginTime;
	QueryPerformanceCounter(&beginTime);

	while (graphicsContext.IsRunning())
	{
		LARGE_INTEGER endTime;
		QueryPerformanceCounter(&endTime);
		f32 frameTime = (f32)(endTime.QuadPart - beginTime.QuadPart) / timerFrequency.QuadPart;
		beginTime = endTime;

		u32* pixels = graphicsContext.GetFrameBufferPixels();
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
				pixels[y * width + x] = color;
			}
		}

		for (int TriangleId = 0; TriangleId < 6; TriangleId++)
		{
			float Depth = powf(2, TriangleId + 1);

			V3 Points[3] =
			{
				V3(-1.0f, -0.5f, Depth),
				V3(1.0f, -0.5f, Depth),
				V3(0.0f, 0.5f, Depth)
			};

			for (int PointId = 0; PointId < 3; PointId++)
			{
				V3 TransformedPos = Points[PointId];

				V2 PixelPos = ProjectPoint(TransformedPos);

				if (PixelPos.x >= 0.0f && PixelPos.x < width &&
					PixelPos.y >= 0.0f && PixelPos.y < height)
				{
					int PixelId = int(PixelPos.y) * width + int(PixelPos.x);
					pixels[PixelId] = 0xFF00FF00;
				}
			}
		}

		graphicsContext.ProcessSystemMessages();
		graphicsContext.RenderFrame();
	}

	graphicsContext.ReleaseResources();
	return 0;
}
