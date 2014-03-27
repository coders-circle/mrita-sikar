#pragma comment (lib, "opengl32")
#ifdef _DEBUG
#pragma comment (lib, "libs\\SOILdebug")
#else
#pragma comment (lib, "libs\\SOIL")
#endif

#include "main.h"


#include <ctime>

int main(int argc, char* argv[])
{

	int ret;
	try
	{
		ret = g_window.InitWindow(L"मृत-शिकार", g_width, g_height, argv, SW_SHOWMAXIMIZED);	
		g_window.SetupOpengl();
	}
	catch (int x)
	{
		if (x == -1)
			std::cout << "Window Initialization - Failed" << std::endl;
		else
			std::cout << "DOESN'T SUPPORT OPENGL3.3" << std::endl;
		system("pause");
		return x;
	}

	g_window.RenderFunc(Render);
	g_window.UpdateFunc(Update);
	g_window.ResizeFunc(Resize);

	srand(static_cast<int>(time(NULL)));
	Initialize();
	Resize(g_width, g_height);

	ret = g_window.MainLoop();

	CleanUp();

	return ret;
}
