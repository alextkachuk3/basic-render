#include <Windows.h>

#define Assert(Expression) if (!(Expression)) {__debugbreak();}
#define InvalidCodePath Assert(!"Invalid Code Path")

struct GlobalState
{
	HWND windowHandle;
	HDC deviceContext;
	UINT frameBufferWidth;
	UINT frameBufferHeight;
	UINT* frameBufferPixels;
	FLOAT curOffset;
	bool isRunning;
};

static GlobalState globalState;

LRESULT Win32WindowCallBack(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
{
	LRESULT Result = {};

	switch (Message)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		globalState.isRunning = false;
		break;
	default:
		Result = DefWindowProcA(WindowHandle, Message, WParam, LParam);
	}

	return Result;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR lpCmdLine, _In_ int nCmdShow)
{
	globalState.isRunning = true;
	LARGE_INTEGER TimerFrequency = {};
	Assert(QueryPerformanceFrequency(&TimerFrequency));

	WNDCLASSA WindowClass = {};
	WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	WindowClass.lpfnWndProc = Win32WindowCallBack;
	WindowClass.hInstance = hInstance;
	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WindowClass.lpszClassName = "Render";

	if (!RegisterClassA(&WindowClass)) InvalidCodePath;

	globalState.windowHandle = CreateWindowExA(
		0,
		WindowClass.lpszClassName,
		"Render",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1280,
		720,
		NULL,
		NULL,
		hInstance,
		NULL);

	if(!globalState.windowHandle) InvalidCodePath;

	globalState.deviceContext = GetDC(globalState.windowHandle);

	RECT ClientRect = {};
	Assert(GetClientRect(globalState.windowHandle, &ClientRect));
	globalState.frameBufferWidth = ClientRect.right - ClientRect.left;
	globalState.frameBufferHeight = ClientRect.bottom - ClientRect.top;
	globalState.frameBufferPixels = (UINT*)malloc(sizeof(UINT) * globalState.frameBufferWidth * globalState.frameBufferHeight);

	LARGE_INTEGER BeginTime = {};
	LARGE_INTEGER EndTime = {};
	Assert(QueryPerformanceCounter(&BeginTime));

	while (globalState.isRunning)
	{
		Assert(QueryPerformanceCounter(&EndTime));
		FLOAT frameTime = (FLOAT)(EndTime.QuadPart - BeginTime.QuadPart) / (FLOAT)(TimerFrequency.QuadPart);
		BeginTime = EndTime;

		MSG Message = {};
		while (PeekMessageA(&Message, globalState.windowHandle, 0, 0, PM_REMOVE))
		{
			switch (Message.message)
			{
			case WM_QUIT:
				globalState.isRunning = false;
				break;
			default:
				TranslateMessage(&Message);
				DispatchMessageA(&Message);
			}
		}

		CONST FLOAT speed = 300.0f;
		globalState.curOffset += speed * frameTime;

		for (UINT Y = 0; Y < globalState.frameBufferHeight; Y++)
		{
			for (UINT X = 0; X < globalState.frameBufferWidth; X++)
			{
				UINT pixedId = Y * globalState.frameBufferWidth + X;

				UINT8 red = (UINT)(X) / 1.6;
				UINT8 Green = 0;
				UINT8 Blue = (UINT)(Y + globalState.curOffset);
				UINT8 Alpha = 255;
				UINT PixelColor = ((UINT)Alpha << 24) | ((UINT)red << 16) | ((UINT)Green << 8) | (UINT)Blue;

				globalState.frameBufferPixels[pixedId] = PixelColor;
			}
		}

		Assert(GetClientRect(globalState.windowHandle, &ClientRect));
		UINT ClientWidth = ClientRect.right - ClientRect.left;
		UINT ClientHeight = ClientRect.bottom - ClientRect.top;

		BITMAPINFO BitmapInfo = {};
		BitmapInfo.bmiHeader.biSize = sizeof(tagBITMAPINFOHEADER);
		BitmapInfo.bmiHeader.biWidth = globalState.frameBufferWidth;
		BitmapInfo.bmiHeader.biHeight = globalState.frameBufferHeight;
		BitmapInfo.bmiHeader.biPlanes = 1;
		BitmapInfo.bmiHeader.biBitCount = 32;
		BitmapInfo.bmiHeader.biCompression = BI_RGB;

		Assert(StretchDIBits(
			globalState.deviceContext,
			0,
			0,
			ClientWidth,
			ClientHeight,
			0,
			0,
			globalState.frameBufferWidth,
			globalState.frameBufferHeight,
			globalState.frameBufferPixels,
			&BitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY
		));
	}

	ReleaseDC(globalState.windowHandle, globalState.deviceContext);
	free(globalState.frameBufferPixels);

	return 0;
}
