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
	//��������Ȩ
	BOOL EnablePrivilege(HANDLE process = NULL);
	//���������ݷ�ʽ
	bool CreateDesktopLnk(const std::string &pragmaFilename, const std::string &LnkName = "", const std::string& cmdline = "", const std::string& iconFilename = "");
	//���ó���������
	bool SetAutoBoot(const std::string& filename = "", bool enable = true);
	//��ȡ����������״̬
	bool GetAutoBootStatus(const std::string& filename);
	BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam);
	HWND FindMainWindow(DWORD processId);
	BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam);
	//��ȡ������Ϣ
	std::vector<PROCESSENTRY32> FindProcessInfo(const std::string& _proccname);
	//���ݽ������ƴ򿪽���
	HANDLE OpenProcess(const std::string& _proccname);
	//��ȡ����ID����
	std::vector<DWORD> FindProcessId(const std::string& proccname);
	//��ȡ�����ļ�·��
	std::string FindProcessFilename(DWORD processId);
	//�ر����н���
	int CloseProcess(const std::vector<DWORD>& processIds);
	//ʹ�ý���ID�رս���
	bool CloseProcess(DWORD processId);
	//��ȡ�����ǲ���64λ��
	bool Is64BitPorcess(DWORD processId);
	bool Is86BitPorcess(DWORD processId);
	//��ȡ��ǰ����ID
	DWORD GetCurrentProcessId();
	//��ȡϵͳ��Ϣ
	void SafeGetNativeSystemInfo(__out LPSYSTEM_INFO lpSystemInfo);
	//��ȡϵͳλ��
	 int GetSystemBits();
	 //��ȡ�����Ψһʶ����
	 std::string GetComputerID();

};
