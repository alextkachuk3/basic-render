#include <Windows.h>
#include "AssertUtils.h"
#include "GlobalState.h"

static GlobalState globalState;

static LRESULT CALLBACK Win32WindowCallBack(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		globalState.SetIsRunning(false);
		return 0;
	default:
		return DefWindowProcA(windowHandle, message, wParam, lParam);
	}
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{	
	globalState.Initialize(hInstance, "Render", 1280, 720, Win32WindowCallBack);
	globalState.SetIsRunning(true);

	LARGE_INTEGER timerFrequency;
	QueryPerformanceFrequency(&timerFrequency);

	LARGE_INTEGER beginTime;
	QueryPerformanceCounter(&beginTime);

	while (globalState.IsRunning())
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
				globalState.SetIsRunning(false);
			}
			else
			{
				TranslateMessage(&message);
				DispatchMessageA(&message);
			}
		}

		float offset = globalState.GetCurOffset() + 300.0f * frameTime;
		globalState.SetCurOffset(offset);

		u32* pixels = globalState.GetFrameBufferPixels();
		u32 width = globalState.GetFrameBufferWidth();
		u32 height = globalState.GetFrameBufferHeight();

		for (uint32_t y = 0; y < height; y++)
		{
			for (uint32_t x = 0; x < width; x++)
			{
				u8 red = static_cast<uint8_t>(x + offset);
				u8 green = static_cast<uint8_t>(y + offset);
				u8 blue = 100;
				u8 alpha = 255;
				u32 color = ((u32)alpha << 24) | ((u32)red << 16) | ((u32)green << 8) | (u32)blue;
				pixels[y * width + x] = color;
			}
		}

		RECT ClientRect = {};
		Assert(GetClientRect(globalState.GetWindowHandle(), &ClientRect));
		u32 ClientWidth = ClientRect.right - ClientRect.left;
		u32 ClientHeight = ClientRect.bottom - ClientRect.top;

		BITMAPINFO BitmapInfo = {};
		BitmapInfo.bmiHeader.biSize = sizeof(tagBITMAPINFOHEADER);
		BitmapInfo.bmiHeader.biWidth = globalState.GetFrameBufferWidth();
		BitmapInfo.bmiHeader.biHeight = globalState.GetFrameBufferHeight();
		BitmapInfo.bmiHeader.biPlanes = 1;
		BitmapInfo.bmiHeader.biBitCount = 32;
		BitmapInfo.bmiHeader.biCompression = BI_RGB;

		Assert(StretchDIBits(
			globalState.GetDeviceContext(),
			0,
			0,
			ClientWidth,
			ClientHeight,
			0,
			0,
			globalState.GetFrameBufferWidth(),
			globalState.GetFrameBufferHeight(),
			globalState.GetFrameBufferPixels(),
			&BitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY
		));
	}

	globalState.ReleaseResources();
	return 0;
}
