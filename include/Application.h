#pragma once
#include "EzUI.h"
namespace EzUI {


	extern LRESULT CALLBACK EzUI_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	class UI_EXPORT Application
	{
	private:
	public:
		Application();
		virtual ~Application();
		int exec();
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
	};

};