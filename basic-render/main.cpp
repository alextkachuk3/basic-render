#include <Windows.h>
#include <cstdint>

#define Assert(Expression) if (!(Expression)) {__debugbreak();}
#define InvalidCodePath Assert(!"Invalid Code Path")

typedef uint32_t u32;
typedef uint8_t u8;

struct GlobalState
{
	HWND windowHandle;
	HDC deviceContext;
	u32 frameBufferWidth;
	u32 frameBufferHeight;
	u32* frameBufferPixels;
	float curOffset;
	bool isRunning;
};

static GlobalState globalState;

static LRESULT Win32WindowCallBack(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
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

void InitializeWindow(HINSTANCE hInstance)
{
	WNDCLASSA WindowClass = {};
	WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	WindowClass.lpfnWndProc = Win32WindowCallBack;
	WindowClass.hInstance = hInstance;
	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WindowClass.lpszClassName = "Render";

	Assert(RegisterClassA(&WindowClass));

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
		NULL
	);

	Assert(globalState.windowHandle);

	globalState.deviceContext = GetDC(globalState.windowHandle);
}

void InitializeFrameBuffer()
{
	RECT ClientRect = {};
	Assert(GetClientRect(globalState.windowHandle, &ClientRect));

	globalState.frameBufferWidth = ClientRect.right - ClientRect.left;
	globalState.frameBufferHeight = ClientRect.bottom - ClientRect.top;
	globalState.frameBufferPixels = (u32*)malloc(sizeof(u32) * globalState.frameBufferWidth * globalState.frameBufferHeight);

	Assert(globalState.frameBufferPixels);
}

void UpdateFrame(float frameTime)
{
	const float speed = 300.0f;
	globalState.curOffset += speed * frameTime;

	for (size_t Y = 0; Y < globalState.frameBufferHeight; Y++)
	{
		for (size_t X = 0; X < globalState.frameBufferWidth; X++)
		{
			u32 pixelId = Y * globalState.frameBufferWidth + X;

			u8 Red = static_cast<u8>((X + globalState.curOffset));
			u8 Green = static_cast<u8>((Y + globalState.curOffset));
			u8 Blue = 128;
			u8 Alpha = 255;

			u32 PixelColor = (Alpha << 24) | (Red << 16) | (Green << 8) | Blue;
			globalState.frameBufferPixels[pixelId] = PixelColor;
		}
	}
}

void RenderFrame()
{
	RECT ClientRect = {};
	Assert(GetClientRect(globalState.windowHandle, &ClientRect));
	u32 ClientWidth = ClientRect.right - ClientRect.left;
	u32 ClientHeight = ClientRect.bottom - ClientRect.top;

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

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR lpCmdLine, _In_ int nCmdShow)
{
	globalState.isRunning = true;
	LARGE_INTEGER TimerFrequency = {};
	Assert(QueryPerformanceFrequency(&TimerFrequency));

	InitializeWindow(hInstance);
	InitializeFrameBuffer();

	LARGE_INTEGER BeginTime = {};
	LARGE_INTEGER EndTime = {};
	Assert(QueryPerformanceCounter(&BeginTime));

	while (globalState.isRunning)
	{
		Assert(QueryPerformanceCounter(&EndTime));
		float frameTime = (float)(EndTime.QuadPart - BeginTime.QuadPart) / (float)(TimerFrequency.QuadPart);
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

		UpdateFrame(frameTime);
		RenderFrame();
	}

	ReleaseDC(globalState.windowHandle, globalState.deviceContext);
	free(globalState.frameBufferPixels);

	return 0;
}
