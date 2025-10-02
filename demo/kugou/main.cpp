#include "global.h"
#include "mainFrm.h"

#include <DbgHelp.h>
#include <strsafe.h>
#include <tchar.h>

#pragma comment(lib, "dbghelp.lib")
void CreateMiniDump(EXCEPTION_POINTERS* pep)
{
	// 生成文件名，带时间戳
	SYSTEMTIME stLocalTime;
	GetLocalTime(&stLocalTime);

	TCHAR szFileName[MAX_PATH];
	StringCchPrintf(szFileName, MAX_PATH, _T("CrashDump_%04d-%02d-%02d_%02d_%02d_%02d.dmp"),
		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);

	HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;

		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = pep;
		mdei.ClientPointers = FALSE;

		// 生成完整的 dump，可以改为 MiniDumpNormal 看需要
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, &mdei, nullptr, nullptr);

		CloseHandle(hFile);
	}
}
// 全局异常处理函数
LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS* ExceptionInfo)
{
	CreateMiniDump(ExceptionInfo);
	return EXCEPTION_EXECUTE_HANDLER;  // 结束程序
}
void EnableCrashDumps() {
	// 注册异常处理回调
	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{


	Application app;//app类
	app.EnableHighDpi();//启用高dpi适配
	app.SetResource("my_res");//设定vs中的自定义资源名称

	MainFrm frm;//主窗口

	frm.CenterToScreen();//屏幕居中
	frm.Show();//显示窗口

	return app.Exec();//进行消息循环
}