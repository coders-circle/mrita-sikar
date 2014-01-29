#pragma comment (lib, "opengl32")
#ifdef _DEBUG
#pragma comment (lib, "libs\\SOILdebug")
#else
#pragma comment (lib, "libs\\SOIL")
#endif

#include "game.h"

int main(int argc, char* argv[])
{
	int ret;
	ret = g_window.InitWindow(L"मृत-शिकार", g_width, g_height, argv, SW_SHOWMAXIMIZED);
	if (ret == -1)
	{
		MessageBox(0, L"Window Initialization - Failed", L"Error", MB_OK);
		return 0;
	}
	
	g_window.SetupOpengl();	

	g_window.RenderFunc(Render);
	g_window.UpdateFunc(Update);
	g_window.ResizeFunc(Resize);

	Initialize();
	Resize(g_width, g_height);

	ret = g_window.MainLoop();

	CleanUp();

	return ret;
}
