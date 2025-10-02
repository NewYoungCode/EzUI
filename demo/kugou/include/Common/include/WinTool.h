#pragma once
#include <map>
#include <functional>
#include <Windows.h>
#include <TlHelp32.h>

#include "Text.h"

#ifdef GetUserName
#undef GetUserName
inline void GetUserName(LPSTR lpBuffer, LPDWORD pcbBuffer) {
	::GetUserNameA(lpBuffer, pcbBuffer);
}
inline void GetUserName(LPWSTR lpBuffer, LPDWORD pcbBuffer) {
	::GetUserNameW(lpBuffer, pcbBuffer);
}
#endif

#ifdef GetComputerName
#undef GetComputerName
inline void GetComputerName(LPSTR lpBuffer, LPDWORD pcbBuffer) {
	::GetComputerNameA(lpBuffer, pcbBuffer);
}
inline void GetComputerName(LPWSTR lpBuffer, LPDWORD pcbBuffer) {
	::GetComputerNameW(lpBuffer, pcbBuffer);
}
#endif

namespace WinTool {
	/// <summary>
	/// 路由信息
	/// </summary>
	struct RouterInfo {
		Text::String IP;
		Text::String MAC;
	};

	typedef struct {
		unsigned long processId;
		HWND best_handle;
	}handle_data;

	typedef struct _MyAdpterInfo
	{
		std::vector<std::string> Ip;
		std::string MacAddress;
		std::string Description;
		std::string Name;
		UINT Type;
	}MyAdpterInfo;

	struct AppInfo {
		/// app名称
		Text::String DisplayName;
		/// 版本号
		Text::String DisplayVersion;
		//显示图标
		Text::String DisplayIcon;
		// 发布者
		Text::String Publisher;
		//软件安装路径
		Text::String InstallLocation;
		// 主程序启动完整路径 C:\\Program Files\\xxx\xxx.exe
		Text::String PragmaFile;
		// 卸载执行的命令行
		Text::String UninstallString;
		// 产品帮助链接
		Text::String HelpLink;
		//帮助说明[这是一款桌面工具]
		Text::String Comments;
		//产品官网
		Text::String URLInfoAbout;
		//是否禁用控制面板修改按钮
		bool NoModify = false;
		//是否禁用控制面板修复按钮
		bool NoRepair = false;
		// 是否创建桌面快捷方式
		bool DesktopLink = true;
		// 是否开机启动
		bool AutoBoot = false;
		//为所有用户注册
		bool AllUsers = true;
	};
	//给进程提权
	extern BOOL EnablePrivilege(HANDLE process = NULL);
	//创捷快捷方式
	extern bool CreateLink(const Text::String& pragmaFilename, const Text::String& outDir, const Text::String& LnkName = L"", const Text::String& cmdline = L"", const Text::String& iconFilename = L"");
	//删除快捷方式
	extern void DeleteLink(const Text::String& linkDir, const Text::String& pragmaFilename, const Text::String& LnkName = "");
	/// 删除注册表中某个项及其子项和值
	extern void DeleteKeyRecursively(HKEY hKeyParent, const wchar_t* subKey);
	//获取软件相关信息
	extern Text::String GetAppValue(const Text::String& appName_en, const Text::String& key);
	//根据软件名称修改一些信息
	extern bool SetAppValue(const Text::String& appName_en, const Text::String& key, const Text::String& value);
	//注册软件到电脑
	extern bool RegisterApp(const AppInfo& appInfo);
	//从电脑上注销软件
	extern 	void UnRegisterApp(const Text::String& appName_en);
	//给软件注册许可
	extern bool RegisterLicenser(const Text::String& exeFilename, const Text::String& softwareData);
	//获取软件许可证书
	extern Text::String FindLicenser(const Text::String& exeFilename);
	//设置程序自启动 rootKey参数:HKEY_CURRENT_USER(当前用户), HKEY_LOCAL_MACHINE//所有用户(需要管理员)
	extern bool SetAutoBoot(const Text::String& filename = L"", bool enable = true, HKEY rootKey = HKEY_CURRENT_USER);
	//获取程序自启动状态 rootKey参数:HKEY_CURRENT_USER(当前用户), HKEY_LOCAL_MACHINE//所有用户(需要管理员)
	extern bool IsAutoBoot(const Text::String& _keyName, HKEY rootKey = HKEY_CURRENT_USER);
	//检查计划任务
	extern bool IsInTask(const Text::String& _taskName);
	//添加到启动计划任务
	extern bool AddBootTask(const Text::String& _taskName, const Text::String& _exeFile);
	//寻找进程中的窗口
	extern HWND FindMainWindow(DWORD processId);
	//获取进程信息
	extern std::vector<PROCESSENTRY32W> FindProcessInfo(const Text::String& _proccname);
	//根据进程名称打开进程
	extern HANDLE OpenProcess(const Text::String& _proccname);
	//获取进程ID集合
	extern std::vector<DWORD> FindProcessId(const Text::String& proccname);
	//获取进程文件路径
	extern Text::String FindProcessFilename(DWORD processId);
	//关闭所有进程
	extern int CloseProcess(const std::vector<DWORD>& processIds);
	//使用进程ID关闭进程
	extern bool CloseProcess(DWORD processId);
	//使用句柄关闭进程
	extern bool CloseProcess(HANDLE hProcess, UINT exitCode = 0);
	//获取进程是不是64位的
	extern bool Is64BitPorcess(DWORD processId);
	//获取进程是不是32位的
	extern bool Is86BitPorcess(DWORD processId);
	//获取当前进程ID
	extern DWORD GetCurrentProcessId();
	//获取系统位数
	extern int GetSystemBits();
	//获取计算机唯一识别码
	extern Text::String GetComputerID();
	//用户当前用户名称
	extern Text::String GetUserName();
	//获取计算机名称
	extern Text::String GetComputerName();
	//获取网卡相关
	extern int GetAdptersInfo(std::vector<MyAdpterInfo>& adpterInfo);
	/// 获取磁盘可以用空间单位:GB
	extern double GetDiskFreeSize(const Text::String& path);
	/// 直接执行可执行文件并获取返回内容
	extern Text::String ExecuteCMD(const Text::String& cmdStr, std::function<void(const Text::String&)> callback = NULL, HANDLE* outHandle = NULL);
	///获取首选网卡的mac地址
	extern Text::String GetMacAddress();
	/// 获取操作系统的版本号
	extern Text::String GetWinVersion();
	/// 弹出选择文件对话框(filter传入参数的方式存在问题 以后改)
	extern Text::String ShowFileDialog(HWND ownerWnd = NULL, const Text::String& defaultPath = "", const Text::String& title = "Select a File", const Text::String& filter = "All Files\0*.*\0");
	/// 弹出选择目录对话框
	extern Text::String ShowFolderDialog(HWND ownerWnd = NULL, const Text::String& defaultPath = "", const Text::String& title = "Select a directory");
	/// 获取路由信息
	extern RouterInfo GetRouterInfo();
	/// 获取电脑的com端口名称
	extern std::vector<Text::String> GetComPorts();
	/// 安装带有.inf文件的驱动
	extern bool InstallDriver(const Text::String& infPath, bool* needReboot = NULL);
	/// 获取已安装的应用
	extern std::map<Text::String, Text::String> GetApps();
	/// 检测程序是否被调试
	extern bool CheckDebug();
	/// 检查程序是否正在运行 使用文件独占方式
	extern bool IsRunning(const Text::String& productName = "", bool lock = true);
	/// 添加一个程序到防火墙规则 ps:需要管理员权限运行
	extern void AddFirewallRule(const Text::String& programFile);
	//是否接管异常(仅限正式环境调用 使用vs启动会被vs接管异常)
	extern bool IsExceptionHijacked();
};
