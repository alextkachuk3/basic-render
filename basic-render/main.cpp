#include <Windows.h>
#include "AssertUtils.h"
#include "GraphicsContext.h"

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
		float frameTime = static_cast<float>(endTime.QuadPart - beginTime.QuadPart) / timerFrequency.QuadPart;
		beginTime = endTime;

		MSG message;
		while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				graphicsContext.SetIsRunning(false);
			}
			else
			{
				TranslateMessage(&message);
				DispatchMessageA(&message);
			}
		}

		float offset = graphicsContext.GetCurOffset() + 300.0f * frameTime;
		graphicsContext.SetCurOffset(offset);

		u32* pixels = graphicsContext.GetFrameBufferPixels();
		u32 width = graphicsContext.GetFrameBufferWidth();
		u32 height = graphicsContext.GetFrameBufferHeight();

		for (size_t y = 0; y < height; y++)
		{
			for (size_t x = 0; x < width; x++)
			{
				u8 red = static_cast<uint8_t>(x + offset);
				u8 green = static_cast<uint8_t>(y + offset);
				u8 blue = 100;
				u8 alpha = 255;
				u32 color = ((u32)alpha << 24) | ((u32)red << 16) | ((u32)green << 8) | (u32)blue;
				pixels[y * width + x] = color;
			}
		}

		graphicsContext.RenderFrame();
	}

	graphicsContext.ReleaseResources();
	return 0;
}
