#pragma once
#include "General.h"
#include <TlHelp32.h>
#include <ShlObj.h>
#include <ShObjIdl.h>
#include <ShlGuid.h>
#include <psapi.h>
#include <process.h>

namespace WinTool {
	typedef struct {
		unsigned long processId;
		HWND best_handle;
	}handle_data;
	//给进程提权
	BOOL EnablePrivilege(HANDLE process = NULL);
	//创建桌面快捷方式
	bool CreateDesktopLnk(const std::string &pragmaFilename, const std::string &LnkName = "", const std::string& cmdline = "", const std::string& iconFilename = "");
	//设置程序自启动
	bool SetAutoBoot(const std::string& filename = "", bool enable = true);
	//获取程序自启动状态
	bool GetAutoBootStatus(const std::string& filename);
	BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam);
	HWND FindMainWindow(DWORD processId);
	BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam);
	//获取进程信息
	std::vector<PROCESSENTRY32> FindProcessInfo(const std::string& _proccname);
	//根据进程名称打开进程
	HANDLE OpenProcess(const std::string& _proccname);
	//获取进程ID集合
	std::vector<DWORD> FindProcessId(const std::string& proccname);
	//获取进程文件路径
	std::string FindProcessFilename(DWORD processId);
	//关闭所有进程
	int CloseProcess(const std::vector<DWORD>& processIds);
	//使用进程ID关闭进程
	bool CloseProcess(DWORD processId);
	//获取进程是不是64位的
	bool Is64BitPorcess(DWORD processId);
	bool Is86BitPorcess(DWORD processId);
	//获取当前进程ID
	DWORD GetCurrentProcessId();
	//获取系统信息
	void SafeGetNativeSystemInfo(__out LPSYSTEM_INFO lpSystemInfo);
	//获取系统位数
	 int GetSystemBits();
	 //获取计算机唯一识别码
	 std::string GetComputerID();

};
