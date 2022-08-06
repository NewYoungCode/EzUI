#include "global.h"
#include "MainFrm.h"
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	Application app;
	StopWatch sw;
	MainFrm form(800, 600);
	form.Show();
	Debug::Log("´°¿ÚÆô¶¯ºÄÊ± %d ms", sw.ElapsedMilliseconds());
	//FrameWindow fw(800,600);
	//Layout l;
	//l.Style.BackgroundColor = Color::White;
	//fw.SetLayout(&l);
	//fw.Show();
	return	app.exec();
}
