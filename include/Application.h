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
		//ʹ��vs�е���ԴID������Դ�� resID��ԴID custResType�Զ�����Դ�������� ����
		Application(int resID, const EString& custResType, const EString&password="");
		//ʹ�ñ����ļ����Ƽ�����Դ��
		Application(const EString& fileName, const EString& password = "");
		virtual ~Application();
		int exec();
	};
};