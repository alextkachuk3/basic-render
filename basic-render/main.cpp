#include <Windows.h>
#include <cmath>
#include "AssertUtils.h"
#include "GraphicsContext.h"
#include "V2.h"
#include "V3.h"

static float pi = 3.14159265359f;

static GraphicsContext graphicsContext;

static V2 ProjectPoint(V3 Pos)
{
	return 0.5f * (Pos.xy / Pos.z + V2(1)) * V2((f32)graphicsContext.GetFrameBufferWidth(), (f32)graphicsContext.GetFrameBufferHeight());
}

void DrawTriangle(V3* Points, u32 PixelColor)
{
	V2 PointA = ProjectPoint(Points[0]);
	V2 PointB = ProjectPoint(Points[1]);
	V2 PointC = ProjectPoint(Points[2]);

	V2 Edge0 = PointB - PointA;
	V2 Edge1 = PointC - PointB;
	V2 Edge2 = PointA - PointC;

	bool IsTopLeft0 = (Edge0.x >= 0.0f && Edge0.y > 0.0f) || (Edge0.x > 0.0f && Edge0.y == 0.0f);
	bool IsTopLeft1 = (Edge1.x >= 0.0f && Edge1.y > 0.0f) || (Edge1.x > 0.0f && Edge1.y == 0.0f);
	bool IsTopLeft2 = (Edge2.x >= 0.0f && Edge2.y > 0.0f) || (Edge2.x > 0.0f && Edge2.y == 0.0f);

	for (u32 y = 0; y < graphicsContext.GetFrameBufferHeight(); y++)
	{
		for (u32 x = 0; x < graphicsContext.GetFrameBufferWidth(); x++)
		{
			V2 PixelPoint = V2(x, y) + V2(0.5f, 0.5f);

			V2 PixelEdge0 = PixelPoint - PointA;
			V2 PixelEdge1 = PixelPoint - PointB;
			V2 PixelEdge2 = PixelPoint - PointC;

			f32 CrossLength0 = V2::CrossProduct2d(PixelEdge0, Edge0);
			f32 CrossLength1 = V2::CrossProduct2d(PixelEdge1, Edge1);
			f32 CrossLength2 = V2::CrossProduct2d(PixelEdge2, Edge2);

			if ((CrossLength0 > 0.0f || (IsTopLeft0 && CrossLength0 == 0.0f)) &&
				(CrossLength1 > 0.0f || (IsTopLeft1 && CrossLength1 == 0.0f)) &&
				(CrossLength2 > 0.0f || (IsTopLeft2 && CrossLength2 == 0.0f)))
			{
				u32 PixelId = y * graphicsContext.GetFrameBufferWidth() + x;
				graphicsContext.GetFrameBufferPixels()[PixelId] = PixelColor;
			}
		}
	}
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

		u32 Colors[] =
		{
			0xFF00FF00,
			0xFFFF00FF,
			0xFF0000FF,
		};

		for (i32 TriangleId = 9; TriangleId >= 0; --TriangleId)
		{
			f32 DistToCamera = powf(2.0f, TriangleId + 1);
			V3 Points[3] =
			{
				V3(-1.0f, -0.5f, DistToCamera),
				V3(0, 0.5f, DistToCamera),
				V3(1.0f, -0.5f, DistToCamera),
			};

			for (u32 PointId = 0; PointId < 3; ++PointId)
			{
				V3 ShiftedPoint = Points[PointId] + V3(cosf(currentTime), sinf(currentTime), 0);
				Points[PointId] = ShiftedPoint;
			}

			DrawTriangle(Points, Colors[TriangleId % 3]);
		}

		currentTime += frameTime * speed;

		if (currentTime >= 2.0f * pi)
			currentTime -= 4.0f * pi;

		graphicsContext.ProcessSystemMessages();
		graphicsContext.RenderFrame();
	}

	graphicsContext.ReleaseResources();
	return 0;
}
