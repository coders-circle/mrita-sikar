#pragma once
#include <Windows.h>

enum MouseButtons{MOUSE_LEFT, MOUSE_RIGHT};
class Window
{
private:
	HWND m_hwnd;
	void(*m_resize)(int width, int height);
	void(*m_update)(double totalTime, double deltaTime);
	void(*m_render)();
	bool m_opengl;

	static LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool InitializeWindow(const wchar_t* title, HINSTANCE hInstance, int showWnd, int width, int height);
	void SetupPixelFormat();

	HGLRC m_hRC;
	HDC m_hDC;

	bool m_mouseButtons[2];
public:
	Window();
	int InitWindow(const wchar_t* title, int &width, int &height, char** argv, int showCmd);
	int MainLoop();
	void SetupOpengl();
	void DestroyOpengl(); 
	void SwapBuffers() { ::SwapBuffers(m_hDC); }
	void Exit() { PostQuitMessage(0); }

	void ResizeFunc(void(*resize)(int, int))		{ if (resize) m_resize = resize; }
	void UpdateFunc(void(*update)(double, double))	{ if (update) m_update = update; }
	void RenderFunc(void(*render)())				{ if (render) m_render = render; }	

	bool CheckKey(int key) { return (GetAsyncKeyState(key) & 0x80000) != 0; }
	bool CheckKey(char key) { return (GetAsyncKeyState(VkKeyScan(key)) & 0x80000) != 0; }
	bool CheckMButton(int button) { return m_mouseButtons[button]; }

	void GetMousePos(int &x, int &y) { POINT pt; GetCursorPos(&pt); x = pt.x, y = pt.y; }
	void SetMousePos(int x, int y) { SetCursorPos(x, y); }
	void ShowMouseCursor(bool show) { ShowCursor(show); }
};

