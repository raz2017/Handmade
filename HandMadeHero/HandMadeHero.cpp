// HandMadeHero.cpp : Defines the entry point for the application.
//

#include "HandMadeHero.h"
#include "windows.h"
#include "stdint.h"

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

static bool Running;
static BITMAPINFO BitmapInfo;
static void* BitmapMemory;
static int BitmapWidth;
static int BitmapHeight;

static int BytesPerPixel = 4;


void RenderWeirdGrandient(int XOffset, int YOffset)
{
	int Width = BitmapWidth;
	int Height = BitmapHeight;

	int Pitch = Width * BytesPerPixel;
	uint8* Row = (uint8*)BitmapMemory;
	for (int Y = 0; Y < BitmapHeight; ++Y)
	{
		uint32* Pixel = (uint32*)Row;
		for (int X = 0; X < BitmapWidth; ++X)
		{
			uint8 Blue = (X + XOffset);
			uint8 Green = (Y + YOffset);

			*Pixel++ = ((Green << 8) | Blue);
		}

		Row += Pitch;
	}
}


void WIN32ResizeDIBSection(int Width, int Height)
{
	if (BitmapMemory)
	{
		VirtualFree(BitmapMemory, 0, MEM_RELEASE);
	}

	BitmapWidth = Width;
	BitmapHeight = Height;

	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = BitmapWidth;
	BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

	int BitMapMemorySize = (BitmapWidth * BitmapHeight) * BytesPerPixel;
	BitmapMemory = VirtualAlloc(0, BitMapMemorySize, MEM_COMMIT, PAGE_READWRITE);

	RenderWeirdGrandient(128, 0);

}

void
WIN32UpdateWindow(HDC DeviceContext, RECT* WindowRect, int X, int Y, int Width, int Height)
{
	int WindowWidth = WindowRect->right - WindowRect->left;
	int WindowHeight = WindowRect->bottom - WindowRect->top;

	StretchDIBits(DeviceContext, 0, 0, BitmapWidth, BitmapHeight,
	              0, 0, WindowWidth, WindowHeight,
	              BitmapMemory, &BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK WIN32MainWindowCallBack(
	HWND Window,
	UINT Message,
	WPARAM WParam,
	LPARAM LParam
)
{
	LRESULT Result = 0;

	switch (Message)
	{
	case WM_SIZE:
		{
			RECT ClientRect;
			GetClientRect(Window, &ClientRect);
			int Width = ClientRect.right - ClientRect.left;
			int Height = ClientRect.bottom - ClientRect.top;

			WIN32ResizeDIBSection(Width, Height);
			OutputDebugStringA("WM_SIZE\n");
		}
		break;

	case WM_CLOSE:
		{
			Running = false;
		}
		break;

	case WM_DESTROY:
		{
			Running = false;
		}
		break;

	case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		}
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.top;
			int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;

			RECT ClientRect;
			GetClientRect(Window, &ClientRect);

			WIN32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
			EndPaint(Window, &Paint);
		}
		break;

	default:
		{
			Result = DefWindowProc(Window, Message, WParam, LParam);
		}
		break;
	}
	return (Result);
}

int CALLBACK WinMain(HINSTANCE Instance,
                     HINSTANCE PrevInstance,
                     LPSTR CommandLine,
                     int ShowCode)
{
	WNDCLASS WindowClass = {};

	WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WIN32MainWindowCallBack;
	WindowClass.hInstance = Instance;
	WindowClass.lpszClassName = L"HandmadeHeroWindowClass";

	if (RegisterClass(&WindowClass))
	{
		HWND WindowHandle = CreateWindowEx(
			0,
			WindowClass.lpszClassName,
			L"Handmade Hero",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			Instance,
			0);

		if (WindowHandle)
		{
			int XOffset = 0;
			int YOffset = 0;
			Running = true;

			while (Running)
			{
		
				MSG Message;
				while ( PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
				{
					if (Message.message == WM_QUIT)
					{
						Running = false;
					}

					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}

				RenderWeirdGrandient(XOffset, YOffset);
				HDC DeviceContext = GetDC(WindowHandle);
				RECT ClientRect;
				GetClientRect(WindowHandle, &ClientRect);
				int WindowWidth = ClientRect.right - ClientRect.left;
				int WindowHeight = ClientRect.bottom - ClientRect.top;
				WIN32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);

				++XOffset;
			}
		}
	}
	else
	{
	}
	return (0);
};
