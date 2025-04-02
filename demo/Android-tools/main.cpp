#include "mainForm.h"
#include "Time.hpp"
#include "Log.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	//{
	//	auto licenser = WinTool::FindLicenser(Path::StartFileName());
	//	if (licenser.empty()) {
	//		::MessageBoxW(::GetConsoleWindow(), L"未找到软件许可!", L"", 0);
	//		return  false;
	//	}
	//	time_t timeout = std::atoll(licenser.c_str()) + 60 * 60 * 24 * 15;//七天
	//	Log::Info(L"软件到期时间%s", Time(timeout).ToString().c_str());
	//	if (::time(0) > timeout) {
	//		::MessageBoxW(::GetConsoleWindow(), L"软件许可已到期!", L"", 0);
	//		return false;
	//	}
	//}

	Application app;
	app.EnableHighDpi();

	MainFrm frm(lpCmdLine);
	frm.Show();

	return app.Exec();
};