#include "EzUI/XMLPreview.hpp"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	ezui::Application app;//app类
	app.EnableHighDpi();//启用高dpi适配

	ezui::XMLPreview form;
	form.Show();
	form.SetTopMost(true);

	return app.Exec();
}