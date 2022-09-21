#pragma once
#include "UIDef.h"
#include "IType.h"
#include "EString.h"
namespace EzUI {
	namespace Debug {
		template<typename ...T>
		inline void Log(const EString& formatStr, T ...args) {
#ifdef DEBUGLOG
			WCHAR buf[1024]{ 0 };
			auto count = swprintf_s((buf), 1024, formatStr.utf16().c_str(), std::forward<T>(args)...);
			buf[count] = '\n';
			buf[count + 1] = NULL;
			OutputDebugStringW(buf);
#endif
		}
	};

	namespace Dialog {
		//inline EString OpenFileDialog(HWND oWner, const EString&title, const EString&filter, const EString&defaultPath) {
		//	OPENFILENAMEW ofn = { 0 };
		//	WCHAR strFilename[MAX_PATH] = { 0 };//
		//	ofn.lStructSize = sizeof(OPENFILENAME);//
		//	ofn.hwndOwner = oWner;//
		//	ofn.lpstrFilter = filter.utf16().c_str();//
		//	ofn.nFilterIndex = 1;//
		//	ofn.lpstrFile = strFilename;//
		//	ofn.nMaxFile = sizeof(strFilename);//
		//	ofn.lpstrInitialDir = defaultPath.utf16().c_str();//
		//	ofn.lpstrTitle =  title.utf16().c_str();//
		//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//
		//	GetOpenFileNameW(&ofn);
		//	return strFilename;
		//}
		//inline EString OpenFileDialog(HWND oWner, const EString& title, const EString& filter, const EString& defaultPath) {
		//	OPENFILENAME ofn = { 0 };
		//	CHAR strFilename[MAX_PATH] = { 0 };//
		//	ofn.lStructSize = sizeof(OPENFILENAME);//
		//	ofn.hwndOwner = oWner;//
		//	ofn.lpstrFilter = filter.c_str();//
		//	ofn.nFilterIndex = 1;//
		//	ofn.lpstrFile = strFilename;//
		//	ofn.nMaxFile = sizeof(strFilename);//
		//	ofn.lpstrInitialDir = defaultPath.c_str();//
		//	ofn.lpstrTitle = title.c_str();//
		//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//
		//	GetOpenFileName(&ofn);
		//	return strFilename;
		//}
	}
};