#include "FileSystem.h"
namespace ezui {
	namespace File {
		bool Exists(const UIString& filename) {
			DWORD dwAttr = ::GetFileAttributesW(filename.unicode().c_str());
			return (dwAttr != INVALID_FILE_ATTRIBUTES && !(dwAttr & FILE_ATTRIBUTE_DIRECTORY));
		}
		bool Copy(const UIString& src, const UIString& desc) {
			return ::CopyFileW(src.unicode().c_str(), desc.unicode().c_str(), FALSE);
		}
		bool Delete(const UIString& file) {
			return ::DeleteFileW(file.unicode().c_str());
		}
		bool Move(const UIString& oldName, const UIString& newName) {
			return ::MoveFileExW(oldName.unicode().c_str(), newName.unicode().c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED);
		}
		bool Create(const UIString& fileName) {
			std::ofstream ofs(fileName.unicode(), std::ios::out | std::ios::binary);
			return ofs.is_open();
		}
		bool Write(const char* fileData, size_t fileSize, const UIString& outFileName) {
			std::ofstream ofs(outFileName.unicode(), std::ios::binary | std::ios::app);
			if (ofs.is_open()) {
				ofs.write(fileData, fileSize);
				return ofs.good();
			}
			return false;
		}
		size_t Read(const UIString& fileName, std::string* data) {
			std::ifstream ifs(fileName.unicode(), std::ios::binary);
			ifs.seekg(0, std::ios::end);
			size_t size = ifs.tellg();
			data->resize(size);
			ifs.seekg(0);
			ifs.read((char*)data->data(), size);
			return size;
		}
	}
	namespace Path {
		void Format(std::string* _str) {
			auto& str = *_str;
			size_t size = str.size();
			if (size == 0) {
				return;
			}
			char* buf = new char[size];
			size_t j = 0;
			for (size_t i = 0; i < size; i++)
			{
				if (str[i] == '\\') {
					buf[j] = '/';
				}
				else {
					buf[j] = str[i];
				}
				j++;
			}
			for (size_t i = 0; i < size; i++)
			{
				// 将多个斜杠替换成单个斜杠
				if (buf[i] == '/') {
					size_t k = i + 1;
					while (buf[k] == '/' && k < size) {
						k++;
					}
					if (k > i + 1) {
						for (size_t m = i + 1; m < size - (k - i - 1); m++) {
							buf[m] = buf[m + (k - i - 1)];
						}
						size -= (k - i - 1);
					}
				}
			}
			str.clear();
			str.append(buf, size);
			delete[] buf; // 释放内存
		}
		UIString GetFileNameWithoutExtension(const UIString& _filename) {
			UIString newStr = _filename;
			Path::Format(&newStr);
			int bPos = newStr.rfind("/");
			int ePos = newStr.rfind(".");
			newStr = newStr.substr(bPos + 1, ePos - bPos - 1);
			return newStr;
		}
		UIString GetDirectoryName(const UIString& _filename) {
			UIString newStr = _filename;
			Path::Format(&newStr);
			int pos = newStr.rfind("/");
			return newStr.substr(0, pos);
		}
		UIString GetExtension(const UIString& _filename) {
			size_t pos = _filename.rfind(".");
			return pos == size_t(-1) ? "" : _filename.substr(pos);
		}
		UIString GetFileName(const UIString& filename) {
			return Path::GetFileNameWithoutExtension(filename) + Path::GetExtension(filename);
		}
	}
	namespace Directory {
		void __Find(const std::wstring& path, std::vector<FileInfo>* result, const std::wstring& pattern, bool looDir) {
			WIN32_FIND_DATAW findData;
			HANDLE findHandle = FindFirstFileW((path + L"/" + pattern).c_str(), &findData);
			if (findHandle == INVALID_HANDLE_VALUE) {
				return;
			}
			do
			{
				if (findData.cFileName[0] == L'.') {
					continue;
				}
				FileInfo file;
				std::wstring name = path + L"/" + findData.cFileName;
				file.Name = name;
				file.Attr = findData.dwFileAttributes;
				result->push_back(file);
				if (file.IsDirectory() && looDir) {
					__Find(name, result, pattern, looDir);
				}
			} while (FindNextFileW(findHandle, &findData));
			FindClose(findHandle);
		}
		bool Exists(const UIString& directoryNme) {
			DWORD dwAttr = GetFileAttributesW(directoryNme.unicode().c_str());
			if (dwAttr == DWORD(-1)) {
				return false;
			}
			if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
			{
				return true;
			}
			return false;
		}
		bool Create(const UIString& path) {
			::CreateDirectoryW(path.unicode().c_str(), NULL);
			if (Exists(path)) {
				return true;
			}
			//创建多级目录
			if (path.find(":") != size_t(-1)) {
				UIString dir = path + "/";
				Path::Format(&dir);
				auto  arr = dir.split("/");
				UIString root;
				if (!arr.empty()) {
					root += arr[0] + "/";
					for (size_t i = 1; i < arr.size(); i++)
					{
						if (arr[i].empty()) {
							continue;
						}
						root += arr[i] + "/";
						if (!Exists(root)) {
							::CreateDirectoryW(root.unicode().c_str(), NULL);
						}
					}
				}
			}
			return Exists(path);
		}
		void Copy(const UIString& srcPath, const UIString& desPath)
		{
			UIString basePath = srcPath;
			Path::Format(&basePath);
			std::vector<FileInfo> result;
			Directory::Find(srcPath, &result);
			for (auto& it : result) {
				auto fileName = it.Name;
				fileName = fileName.replace(basePath, "");
				if (it.IsDirectory()) {
					Directory::Create(desPath + "/" + fileName);
				}
				else {
					File::Copy(it.Name, desPath + "/" + fileName);
				}
			}
		}
		void Find(const UIString& directory, std::vector<FileInfo>* result, const UIString& pattern, bool loopDir)
		{
			__Find(directory.unicode(), result, pattern.unicode(), loopDir);
		}
	}
}