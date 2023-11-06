//#pragma once
//#include <windows.h>
//#include <stdio.h>
//#include <strsafe.h>
//
//namespace Regedit {
//	//删除指定key
//	BOOL DelKey(HKEY keyRoot, LPTSTR keyPath);
//	//判断是否存在key
//	BOOL Exist(HKEY keyRoot, LPTSTR keyPath);
//
//	bool SetValue(HKEY hKey, const TString&key, const TString&value = TEXT(""));
//	void SetValueEx(HKEY hKey, const TString&key, const TString&value);
//	HKEY CreateKey(HKEY rootKey, const TString&keyPath, const TString&defaultValue = TEXT(""));
//};
//
//namespace Regedit {
//	inline bool SetValue(HKEY hKey, const TString&key, const TString&value) {
//		return !::RegSetValue(hKey, key.c_str(), REG_SZ, value.c_str(), value.size());
//	}
//	inline void SetValueEx(HKEY hKey, const TString&key, const TString&value) {
//		RegSetValueEx(hKey, key.c_str(), NULL, REG_SZ, (BYTE*)value.c_str(), value.size());
//	}
//	inline 	HKEY CreateKey(HKEY rootKey, const TString&keyPath, const TString&defaultValue) {
//		HKEY hKey;
//		RegCreateKey(rootKey, keyPath.c_str(), &hKey);
//		SetValue(hKey, TEXT(""), defaultValue);
//		return hKey;
//	}
//
//	inline BOOL DelKey(HKEY hKeyRoot, LPTSTR lpSubKey)
//	{
//		LPTSTR lpEnd;
//		LONG lResult;
//		DWORD dwSize;
//		TCHAR szName[MAX_PATH];
//		HKEY hKey;
//		FILETIME ftWrite;
//		// First, see if we can delete the key without having
//		// to recurse.
//		lResult = RegDeleteKey(hKeyRoot, lpSubKey);
//		if (lResult == ERROR_SUCCESS) {
//			return TRUE;
//		}
//		lResult = RegOpenKeyEx(hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);
//
//		if (lResult != ERROR_SUCCESS)
//		{
//			if (lResult == ERROR_FILE_NOT_FOUND) {
//				printf("Key not found.\n");
//				return TRUE;
//			}
//			else {
//				printf("Error opening key.\n");
//				return FALSE;
//			}
//		}
//
//		// Check for an ending slash and add one if it is missing.
//
//		lpEnd = lpSubKey + lstrlen(lpSubKey);
//
//		if (*(lpEnd - 1) != TEXT('\\'))
//		{
//			*lpEnd = TEXT('\\');
//			lpEnd++;
//			*lpEnd = TEXT('\0');
//		}
//
//		// Enumerate the keys
//
//		dwSize = MAX_PATH;
//		lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
//			NULL, NULL, &ftWrite);
//
//		if (lResult == ERROR_SUCCESS)
//		{
//			do {
//
//				StringCchCopy(lpEnd, MAX_PATH * 2, szName);
//
//				if (!DelKey(hKeyRoot, lpSubKey)) {
//					break;
//				}
//
//				dwSize = MAX_PATH;
//
//				lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
//					NULL, NULL, &ftWrite);
//
//			} while (lResult == ERROR_SUCCESS);
//		}
//
//		lpEnd--;
//		*lpEnd = TEXT('\0');
//
//		RegCloseKey(hKey);
//		// Try again to delete the key.
//		lResult = RegDeleteKey(hKeyRoot, lpSubKey);
//
//		if (lResult == ERROR_SUCCESS)
//			return TRUE;
//
//		return FALSE;
//	}
//	inline BOOL Exist(HKEY keyRoot, LPTSTR keyPath)
//	{
//		HKEY hExtKey;
//		if (RegOpenKey(keyRoot, keyPath, &hExtKey) == ERROR_SUCCESS)
//		{
//			RegCloseKey(hExtKey);
//			return TRUE;
//		}
//		return FALSE;
//	}
//};
//
//
//struct Version {
//	WORD v1, v2, v3, v4;
//};
//struct Product {
//private:
//	HKEY hExtKey = NULL;
//	TString  ProductName;
//	DWORD type = REG_SZ;
//	DWORD dwLen = MAX_PATH;
//public:
//	Product(const TString&ProductName) {
//		this->ProductName = ProductName;
//		if (RegOpenKey(HKEY_CLASSES_ROOT, ProductName.c_str(), &hExtKey) != ERROR_SUCCESS)//如果打不开
//		{
//			hExtKey = ::Regedit::CreateKey(HKEY_CLASSES_ROOT, ProductName);//就创建
//		}
//	}
//	void SetValue(const TString&key, const TString&value) {
//		Regedit::SetValueEx(hExtKey, key, value);
//	}
//	TString GetValue(const TString&key, const TString&defaulValue = "") {
//		TCHAR bufStr[MAX_PATH]{ 0 };
//		if (::RegQueryValueEx(hExtKey, key.c_str(), 0, &type, (LPBYTE)bufStr, &dwLen) != ERROR_SUCCESS) {
//			auto code = ::GetLastError();
//			SetValue(key, defaulValue);
//			return defaulValue;
//		}
//		return bufStr;
//	}
//	Version GetVersion() {//从注册表中获取程序版本
//		TString localVer = GetValue("ver", "0.0.0.0");
//		Version ver{ 0,0,0,0 };//当前程序版本
//		auto vers = Text::Split(localVer, ".");
//		ver.v1 = (WORD)std::stoi(vers.at(0));
//		ver.v2 = (WORD)std::stoi(vers.at(1));
//		ver.v3 = (WORD)std::stoi(vers.at(2));
//		ver.v4 = (WORD)std::stoi(vers.at(3));
//		return ver;
//	}
//	void SetVersion(const Version&ver) {//设置新版本
//		TCHAR newVer[MAX_PATH] = { 0 };
//		Text::Format(newVer, MAX_PATH, "%d.%d.%d.%d", ver.v1, ver.v2, ver.v3, ver.v4);
//		SetValue("ver", newVer);
//	}
//	~Product() {
//		if (hExtKey) {
//			::RegCloseKey(hExtKey);
//		}
//	}
//};