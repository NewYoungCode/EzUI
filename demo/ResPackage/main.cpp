#include "mainFrom.h"
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	//格式化命令行参数
	int argc = 0;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	std::vector<UIString> args;
	for (int i = 0; i < argc; i++) {
		args.emplace_back(argv[i]);
	}
	LocalFree(argv);

	// 查找 "-package" 参数
	auto it = std::find(args.begin(), args.end(), L"-package");
	if (it != args.end()) {
		size_t index = std::distance(args.begin(), it);
		UIString packageDir = args[index + 1];
		UIString outFile = args[index + 2];

		UIString log = "packaging..." + packageDir + " -> " + outFile + "\n";
		printf(log.c_str());
		bool bRet = Resource::Package(packageDir, outFile);
		if (bRet) {
			printf("package succeeded !\n");
		}
		else {
			printf("package failed !\n");
		}
		return 0;
	}

	Application app;
	app.EnableHighDpi();
	app.SetResource("my_res");
	MainFrm frm(lpCmdLine);
	frm.Show();
	frm.CenterToScreen();
	::SetForegroundWindow(frm.GetWindowHandle());

	return app.Exec();
};
