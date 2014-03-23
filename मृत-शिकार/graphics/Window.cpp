/*
WINDOW.CPP
-----------------------------
*/

#include "stdinc.h"
#include "timer.h"
#include "Window.h"

static Timer s_timer;
static void Update(double t1, double t2){}
static void Render(){}

LRESULT CALLBACK Window::wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* winptr = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (winptr == NULL)
		return DefWindowProc(hwnd, msg, wParam, lParam);
	else
		return winptr->WndProc(hwnd, msg, wParam, lParam);
}

Window::Window() : m_resize(NULL), m_update(Update), m_render(Render), m_opengl(false)
{
	m_mouseButtons[0] = m_mouseButtons[1] = false;
}

int Window::InitWindow(const wchar_t* title, int &width, int &height, char** argv, int showCmd)
{
	if (!InitializeWindow(title, GetModuleHandle(NULL), showCmd, width, height))
		return -1;
	
	RECT rc;
	GetWindowRect(m_hwnd, &rc);
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;

	return (int)m_hwnd;
}


bool Window::InitializeWindow(const wchar_t* title, HINSTANCE hInstance, int showWnd, int width, int height)
{
	wchar_t clsname[100] = L"WINDOWCLASS";
	std::wstring newname(clsname);
	newname = newname + title;
	std::copy(newname.begin(), newname.end(), clsname);
	clsname[newname.size()] = L'\0';

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = Window::wndproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = clsname;
	wc.hIconSm = LoadIcon(0, IDI_WINLOGO);

	if (!RegisterClassEx(&wc))
		return false;

	m_hwnd = CreateWindow(
		clsname, title, WS_OVERLAPPEDWINDOW,
		(GetSystemMetrics(SM_CXSCREEN) - width) / 2, (GetSystemMetrics(SM_CYSCREEN) - height) / 2,
		width, height, nullptr, nullptr, hInstance, nullptr
		);

	SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
	SetWindowLong(m_hwnd, GWL_STYLE, 0);

	if (!m_hwnd)
		return false;

	ShowWindow(m_hwnd, showWnd);
	UpdateWindow(m_hwnd);

	return true;
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
	{
		int width = LOWORD(lParam), height = HIWORD(lParam);
		if (m_resize) m_resize(width, height);
		break;
	}
		/*case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;
		case WM_KEYUP:
		switch (wParam)
		{
		}
		break;*/
	case WM_LBUTTONDOWN:
		m_mouseButtons[MOUSE_LEFT] = true;
		break;
	case WM_RBUTTONDOWN:
		m_mouseButtons[MOUSE_RIGHT] = true;
		break;
	case WM_LBUTTONUP:
		m_mouseButtons[MOUSE_LEFT] = false;
		break;
	case WM_RBUTTONUP:
		m_mouseButtons[MOUSE_RIGHT] = false;
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}


int Window::MainLoop()
{
	s_timer.Reset();
	s_timer.SetTargetDeltaSeconds(1.0 / 60);

	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			s_timer.Tick(m_update);
			if (s_timer.GetFrameCount() != 0)
			{
				m_render();
				//*
				char str[20] = "FPS: ";
				_itoa_s(s_timer.GetFramesPerSecond(), &str[5], 20 - 5, 10);
				SetWindowTextA(m_hwnd, str);
				//*/
			}
		}
	}

	if (m_opengl) DestroyOpengl();

	return (int)msg.wParam;
}

void Window::SetupPixelFormat()
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.nVersion = 1;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int pixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	SetPixelFormat(m_hDC, pixelFormat, &pfd);
}

void Window::SetupOpengl()
{
	if (m_opengl) return;

	m_hDC = GetDC(m_hwnd);
	SetupPixelFormat();
	m_hRC = wglCreateContext(m_hDC);
	wglMakeCurrent(m_hDC, m_hRC);
	glewInit();

	m_opengl = true;
}

void Window::DestroyOpengl()
{
	wglMakeCurrent(m_hDC, NULL);
	wglDeleteContext(m_hRC);
	m_opengl = false;
}

void Window::ResetTimer()
{
	s_timer.Reset();
}