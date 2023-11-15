#include "mainFrom.h"

void AddContextMenu() {
	// ��ע�����
	HKEY hKey;
	RegCreateKeyExW(HKEY_CLASSES_ROOT, L"Directory\\shell\\EzUI_ResPackage", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	// �����Ҽ��˵���ʾ������
	std::wstring menuName(L"�����EzUI��Դ");
	RegSetValueExW(hKey, NULL, 0, REG_SZ, (BYTE*)menuName.c_str(), menuName.size() * 2);
	// ���ò˵������ִ�е�����
	RegCreateKeyExW(hKey, L"command", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	std::wstring cmdLine;
	{
		WCHAR buf[512]{ 0 };
		::GetModuleFileNameW(NULL, buf, 512);
		cmdLine = buf;
		cmdLine = L"\"" + cmdLine + L"\" \"%1\"";
	}
	RegSetValueExW(hKey, NULL, 0, REG_SZ, (BYTE*)cmdLine.c_str(), cmdLine.size() * 2); // %1 ����ѡ�е��ļ���·��
	// �ر�ע�����
	RegCloseKey(hKey);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	//��Ҫ����ԱȨ��
	AddContextMenu();

	//����ȡ���Ҽ��˵� ��ע����·����ɾ������
	//�����\HKEY_CLASSES_ROOT\Directory\shell\EzUI_ResPackage

	Application app;
	app.EnableHighDpi();

	MainFrm frm(lpCmdLine);
	frm.Show();

	return app.Exec();
};