#pragma once

#include <Windows.h>
#include "Typedefs.h"
#include "AssertUtils.h"

class GlobalState
{
private:
	HWND windowHandle;
	HDC deviceContext;
	u32 frameBufferWidth;
	u32 frameBufferHeight;
	u32* frameBufferPixels;
	float curOffset;
	bool isRunning;

public:
	GlobalState();
	~GlobalState();

	void Initialize(HINSTANCE hInstance, const char* windowTitle, int width, int height, WNDPROC windowCallback);
	void AllocateFrameBuffer();
	void ReleaseResources();

	HWND GetWindowHandle() const;
	void SetWindowHandle(HWND handle);

	HDC GetDeviceContext() const;
	void SetDeviceContext(HDC context);

	u32 GetFrameBufferWidth() const;
	void SetFrameBufferWidth(u32 width);

	u32 GetFrameBufferHeight() const;
	void SetFrameBufferHeight(u32 height);

	u32* GetFrameBufferPixels() const;

	float GetCurOffset() const;
	void SetCurOffset(float offset);

	bool IsRunning() const;
	void SetIsRunning(bool running);
};
