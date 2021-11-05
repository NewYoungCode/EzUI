#pragma once
#include <list>
#include <vector>
#include <functional>
#include <chrono>
#include <exception>
#include <iostream>
#include <string>
#include <map>
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ�ļ����ų�����ʹ�õ�����
#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include <vcruntime.h>
#include <commdlg.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <Ime.h>
#include <shellapi.h>
#include <time.h>
#include <sys/timeb.h>
#include <WinUser.h>
#include <objbase.h>
#define UI namespace EzUI

#ifdef _WINDLL
#define  UI_EXPORT  __declspec(dllexport) 
#else
#define UI_EXPORT
#endif // _WINDLL

template<typename T>
struct Tuple {
public:
	bool valid = false;//�ֶ������Ƿ���Ч
	T value = NULL;
	Tuple() :valid(false) {
	}
	Tuple(const T&value) {
		this->value = value;
		valid = true;
	}
	operator T&() {
		return value;
	}
	T& operator->() {
		return value;
	}
};

template<typename T>
struct SafeObject {
private:
	SafeObject(const SafeObject&right) {}
	SafeObject& operator=(const SafeObject&right) {}
public:
	T* ptr = NULL;
	SafeObject(T* _ptr) {
		ptr = _ptr;
	}

	~SafeObject() {
		if (ptr) delete ptr;
	}
};


#define UI_Float Tuple<float>
#define UI_Int Tuple<int>
#define UI_UInt Tuple<size_t>

#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif

#ifdef _DEBUG
#define DEBUGPAINT //����ģʽ���Ʊ߿�,������Ϣ ��
#define DEBUGLOG //���Դ�ӡ���
#endif

#ifndef GCL_HCURSOR
#define GCL_HCURSOR -12
#endif

#ifdef _WIN64
#define CPU64
#define UI_SetUserData(hWnd,data)  SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)data);
#define UI_GetUserData(hwnd) GetWindowLongPtr(hwnd, GWLP_USERDATA);
#else  
#define CPU86
#define UI_SetUserData(hWnd,data)  SetWindowLong(hWnd, GWLP_USERDATA, (LONG)data);
#define UI_GetUserData(hwnd) GetWindowLong(hwnd, GWLP_USERDATA);
#endif


#pragma warning(disable:4244)
#pragma warning(disable:4267)

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define UI_CLASSNAME TEXT("EzUI_Win32")
#define UI_NotifyIcon_CLASSNAME TEXT("EzUI_NotifyIcon_Class")

#define WM_NOTIFYICON WM_USER+0x01 //���½�������Ϣ
#define WM_CONTROL_DELETE WM_USER+0x02 //�ؼ�ɾ����ʱ��������Ϣ
#define WM_CONTROL_REFRESH WM_USER+0x03 //����ˢ�µ���Ϣ
//WM_USER ~ 0x7FFF
//����ڶ������������Ϣ
//0x04~0x0b�����Ϣ
//0x0c~0x0e������Ϣ(����,����,WM_CHAR)

#define WM_UIMESSAGE  WM_USER+20   //�û��������Ϣ UI����20����Ϣ

#ifdef UNICODE
//std::wstring
using EString = std::wstring;
#else
//std::string
using EString = std::string;
#endif

namespace String {
	inline std::wstring ANSIToUniCode(const std::string &str)
	{
		int bytes = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
		std::wstring wstrCmd;
		wstrCmd.resize(bytes);
		bytes = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), const_cast<wchar_t*>(wstrCmd.c_str()), wstrCmd.size());
		return wstrCmd;
	}

	inline std::vector<std::string> Split(const std::string& str, const std::string& ch_) {
		std::vector<std::string> arr;
		if (str.empty()) return arr;
		std::string buf = str;
		size_t pos = buf.find(ch_);
		if (pos == std::string::npos) {
			arr.push_back(str);
			return arr;
		}
		for (; pos != std::string::npos;) {
			arr.push_back(buf.substr(0, pos));
			buf = buf.erase(0, pos + ch_.size());
			pos = buf.find(ch_);
			if (pos == std::string::npos) {
				arr.push_back(buf);
			}
		}
		return arr;
	}
}
namespace MsgBox {
	inline int Show(const EString&text, const EString&title = TEXT(""), int mButton = NULL) {
		return ::MessageBox(::GetActiveWindow(), text.c_str(), title.c_str(), mButton);
	}
}

namespace Debug {
	template<typename ...T>
	inline void Log(const EString&formatStr, T ...args) {
#ifdef DEBUGLOG
		TCHAR buf[256]{ 0 };
#ifdef UNICODE
		auto count = swprintf_s((buf), 255, formatStr.c_str(), std::forward<T>(args)...);
#else
		auto count = sprintf_s((buf), 255, formatStr.c_str(), std::forward<T>(args)...);
#endif
		buf[count] = '\n';
		buf[count + 1] = NULL;
		OutputDebugString(buf);
#endif
	}
};

struct StopWatch {
private:
	std::chrono::system_clock::time_point beg_t;
public:
	StopWatch() {
		beg_t = std::chrono::system_clock::now();    //��ʼʱ��
	}
	time_t ElapsedMilliseconds() {
		auto end_t = std::chrono::system_clock::now();    //����ʱ��
		std::chrono::duration<double> diff = end_t - beg_t;//ʱ����s
		return std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();//ʱ����ms
	}
};



namespace Dialog {
	inline EString OpenFileDialog(const EString&title, const TCHAR*filter, const EString&defaultPath) {
		HWND hwnd = ::GetForegroundWindow();//��ȡϵͳǰ������
		OPENFILENAME ofn = { 0 };
		TCHAR strFilename[MAX_PATH] = { 0 };//���ڽ����ļ���
		ofn.lStructSize = sizeof(OPENFILENAME);//�ṹ���С
		ofn.hwndOwner = hwnd;//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������
		ofn.lpstrFilter = filter;//TEXT("�����ļ�\0*.*\0C/C++ Flie\0*.cpp;*.c;*.h\0\0");//���ù���
		ofn.nFilterIndex = 1;//����������
		ofn.lpstrFile = strFilename;//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL
		ofn.nMaxFile = sizeof(strFilename);//����������
		ofn.lpstrInitialDir = defaultPath.c_str();//��ʼĿ¼ΪĬ��
		ofn.lpstrTitle = title.c_str();//ʹ��ϵͳĬ�ϱ������ռ���
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//�ļ���Ŀ¼������ڣ�����ֻ��ѡ��
		GetOpenFileName(&ofn);
		return strFilename;
	}

	//
	//#include<windows.h>
	////�򿪱����ļ��Ի���
	//#include<Commdlg.h>
	//
	////ѡ���ļ��жԻ���
	//#include<Shlobj.h>
	//#pragma comment(lib,"Shell32.lib")
	//
	//int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	//	_In_opt_ HINSTANCE hPrevInstance,
	//	_In_ LPTSTR    lpCmdLine,
	//	_In_ int       nCmdShow)
	//{
	//loop:
	//	OPENFILENAME ofn = { 0 };
	//	TCHAR strFilename[MAX_PATH] = { 0 };//���ڽ����ļ���
	//	ofn.lStructSize = sizeof(OPENFILENAME);//�ṹ���С
	//	ofn.hwndOwner = NULL;//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������
	//	ofn.lpstrFilter = TEXT("�����ļ�\0*.*\0C/C++ Flie\0*.cpp;*.c;*.h\0\0");//���ù���
	//	ofn.nFilterIndex = 1;//����������
	//	ofn.lpstrFile = strFilename;//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL
	//	ofn.nMaxFile = sizeof(strFilename);//����������
	//	ofn.lpstrInitialDir = NULL;//��ʼĿ¼ΪĬ��
	//	ofn.lpstrTitle = TEXT("��ѡ��һ���ļ�");//ʹ��ϵͳĬ�ϱ������ռ���
	//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//�ļ���Ŀ¼������ڣ�����ֻ��ѡ��
	//	if (GetOpenFileName(&ofn))
	//	{
	//		MessageBox(NULL, strFilename, TEXT("ѡ����ļ�"), 0);
	//	}
	//	else {
	//		MessageBox(NULL, TEXT("��ѡ��һ���ļ�"), NULL, MB_ICONERROR);
	//		goto loop;
	//	}
	//
	//	ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;//Ŀ¼������ڣ������ļ�ǰ��������
	//	ofn.lpstrTitle = TEXT("���浽");//ʹ��ϵͳĬ�ϱ������ռ���
	//	ofn.lpstrDefExt = TEXT("cpp");//Ĭ��׷�ӵ���չ��
	//	if (GetSaveFileName(&ofn))
	//	{
	//		MessageBox(NULL, strFilename, TEXT("���浽"), 0);
	//	}
	//	else {
	//		MessageBox(NULL, TEXT("������һ���ļ���"), NULL, MB_ICONERROR);
	//	}
	//
	//	TCHAR szBuffer[MAX_PATH] = { 0 };
	//	BROWSEINFO bi = { 0 };
	//	bi.hwndOwner = NULL;//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������
	//	bi.pszDisplayName = szBuffer;//�����ļ��еĻ�����
	//	bi.lpszTitle = TEXT("ѡ��һ���ļ���");//����
	//	bi.ulFlags = BIF_NEWDIALOGSTYLE;
	//	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	//	if (SHGetPathFromIDList(idl, szBuffer)) {
	//		MessageBox(NULL, szBuffer, TEXT("��ѡ����ļ���"), 0);
	//	}
	//	else {
	//		MessageBox(NULL, TEXT("��ѡ��һ���ļ���"), NULL, MB_ICONERROR);
	//	}
	//
	//	return 0;
	//}
	//

}
