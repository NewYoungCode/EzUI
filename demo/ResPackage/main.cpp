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

		UIString log = UIString("package: %s -> %s \n").format(packageDir.c_str(), outFile.c_str()).ansi();
		printf(log.c_str());
		Resource::Package(packageDir, outFile);
		printf("package done!\n");
		return 0;
	}

	Application app;
	app.EnableHighDpi();
	MainFrm frm(lpCmdLine);
	frm.Show();

	return app.Exec();
};