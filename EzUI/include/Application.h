#pragma once
#include "Window.h"

namespace EzUI {
	extern LRESULT CALLBACK EzUI_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	class UI_EXPORT Application
	{
		void Init();
	public:
		static void exit(int exitCode = 0) {
			PostQuitMessage(exitCode);
		}
		static std::wstring StartPath() {
			WCHAR exeFullPath[512]{ 0 };
			::GetModuleFileNameW(NULL, exeFullPath, 512);
			std::wstring exeFullPathw(exeFullPath);
			size_t pos = exeFullPathw.rfind(L"\\");
			return exeFullPathw.substr(0, pos);
		}
	public:
		Application();
		//使用vs中的资源ID加载资源包 resID资源ID custResType自定义资源类型名称 密码
		Application(int resID, const EString& custResType, const EString&password="");
		//使用本地文件名称加载资源包
		Application(const EString& fileName, const EString& password = "");
		virtual ~Application();
		int exec();
	};
};