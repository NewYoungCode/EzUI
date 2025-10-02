#pragma once
#include "Text.h"
#include "FileSystem.h"

DECLARE_HANDLE(HZIP_U);

struct ZipItem
{
	int index;                 // index of this file within the zip
	TCHAR name[MAX_PATH];      // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime, ctime, mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
	bool isDir()const {
		if (attr & FILE_ATTRIBUTE_DIRECTORY) {
			return true;
		}
		return false;
	}
};

/// <summary>
/// 专门负责解压缩的类
/// </summary>
class UnZiper {
	HZIP_U ptr = NULL;
	int itemCount = 0;
public:
	UnZiper(const std::wstring& fileName, const std::string& password = "");
	UnZiper(const char* fileData, unsigned int size, const std::string& password = "");
	bool Find(const std::string& itemName, ZipItem* item);
	bool Find(int index, ZipItem* item);
	bool UnZipItem(const ZipItem& item, byte** data);
	int GetCount();
	virtual ~UnZiper();
public:
	static bool UnZip(UnZiper* zip, const Text::String& outDir, const std::string& password = "", std::function<void(const Text::String&, int total, int index, bool& stop)> callback = NULL) {
		bool ok = true;
		for (int i = 0; i < zip->GetCount(); i++)
		{
			ZipItem ze;
			zip->Find(i, &ze);
			Text::String itemName = outDir + "/" + Text::String(ze.name);
			if (ze.isDir()) {
				if (!Directory::Create(itemName)) {
					ok = false;
				}
			}
			else {
				File::Delete(itemName);
				byte* data = NULL;
				zip->UnZipItem(ze, &data);
				std::ofstream ofs(itemName.unicode(), std::ios::binary);
				ofs.write((char*)data, ze.unc_size);
				ofs.flush();
				ofs.close();
				if (!ofs.good()) {
					ok = false;
				}
				if (data) {
					delete[] data;
				}
			}
			if (callback) {
				bool stop = false;
				callback(itemName, zip->GetCount(), i, stop);
				if (stop) {
					break;
				}
			}
		}
		return ok;
	}

	static bool UnZip(const Text::String& zipFileName, const Text::String& outDir, const std::string& password = "", std::function<void(const Text::String&, int total, int index, bool& stop)> callback = NULL) {
		Directory::Create(outDir);
		UnZiper zip(zipFileName.unicode(), password);
		return UnZip(&zip, outDir, password, callback);
	}
};
