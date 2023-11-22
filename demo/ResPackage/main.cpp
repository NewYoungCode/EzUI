#include "mainFrom.h"

void AddContextMenu() {
	// 打开注册表项
	HKEY hKey;
	RegCreateKeyExW(HKEY_CLASSES_ROOT, L"Directory\\shell\\EzUI_ResPackage", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	// 设置右键菜单显示的名称
	std::wstring menuName(L"打包成EzUI资源");
	RegSetValueExW(hKey, NULL, 0, REG_SZ, (BYTE*)menuName.c_str(), menuName.size() * 2);
	// 设置菜单点击后执行的命令
	RegCreateKeyExW(hKey, L"command", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	std::wstring cmdLine;
	{
		WCHAR buf[512]{ 0 };
		::GetModuleFileNameW(NULL, buf, 512);
		cmdLine = buf;
		cmdLine = L"\"" + cmdLine + L"\" \"%1\"";
	}
	RegSetValueExW(hKey, NULL, 0, REG_SZ, (BYTE*)cmdLine.c_str(), cmdLine.size() * 2); // %1 代表选中的文件夹路径
	// 关闭注册表项
	RegCloseKey(hKey);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	//需要管理员权限
	AddContextMenu();

	//如需取消右键菜单 到注册表此路径下删除即可
	//计算机\HKEY_CLASSES_ROOT\Directory\shell\EzUI_ResPackage

	Application app;
	app.EnableHighDpi();

	MainFrm frm(lpCmdLine);
	frm.Show();

	return app.Exec();
};