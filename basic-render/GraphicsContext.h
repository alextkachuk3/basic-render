#pragma once

#include <Windows.h>
#include "Typedefs.h"
#include "AssertUtils.h"

class GraphicsContext
{
private:
	HWND windowHandle;
	HDC deviceContext;
	u32 frameBufferWidth;
	u32 frameBufferHeight;
	u32* frameBufferPixels;
	float curAngle;
	bool isRunning;

public:
	GraphicsContext();
	~GraphicsContext();

	void Initialize(HINSTANCE hInstance, const char* windowTitle, int width, int height, WNDPROC windowCallback);
	void AllocateFrameBuffer();
	void ReleaseResources();

	void ProcessSystemMessages();
	void RenderFrame();

	HWND GetWindowHandle() const;
	void SetWindowHandle(HWND handle);

	HDC GetDeviceContext() const;
	void SetDeviceContext(HDC context);

	u32 GetFrameBufferWidth() const;
	void SetFrameBufferWidth(u32 width);

	u32 GetFrameBufferHeight() const;
	void SetFrameBufferHeight(u32 height);

	u32* GetFrameBufferPixels() const;

	bool IsRunning() const;
	void SetIsRunning(bool running);
};
