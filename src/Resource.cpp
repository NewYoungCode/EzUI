#include "Resource.h"
namespace EzUI {
	Resource::ReadStream::ReadStream(HRSRC hRsrc) {
		auto hModule = ::GetModuleHandle(NULL);
		this->ptr = (char*)::LoadResource(hModule, hRsrc);
		this->count = ::SizeofResource(hModule, hRsrc);
	}
	Resource::ReadStream::ReadStream(const EString& fileName) {
		this->ifs = new std::ifstream(fileName.unicode(), std::ios::binary);
		this->ifs->seekg(0, std::ios::end);
		this->count = ifs->tellg();
		this->ifs->seekg(0);
	}
	void Resource::ReadStream::seekg(size_t pos) {
		if (ifs) {
			ifs->seekg(pos);
		}
		else {
			this->pos = pos;
		}
	}
	void Resource::ReadStream::read(char* buf, size_t count) {
		if (ifs) {
			ifs->read(buf, count);
		}
		else {
			::memcpy(buf, ptr + pos, count);
			this->pos += count;
		}
	}
	size_t Resource::ReadStream::tellg() {
		if (ifs) {
			return ifs->tellg();
		}
		else {
			return pos;
		}
	}
	const size_t& Resource::ReadStream::size() {
		return this->count;
	}
	Resource::ReadStream::~ReadStream() {
		if (ifs) {
			delete ifs;
		}
		if (ptr) {
			::FreeResource((HGLOBAL)ptr);
		}
	}

	//寻找指定目录以及目录下的所有文件
	void __Resource__FindFilesRecursively(const std::wstring& path, std::list<std::wstring>* result) {
		WIN32_FIND_DATAW findData;
		HANDLE findHandle = FindFirstFileW((path + L"/*").c_str(), &findData);
		if (findHandle == INVALID_HANDLE_VALUE) {
			return;
		}
		do
		{
			if (findData.cFileName[0] == L'.') {
				continue;
			}
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// 如果是目录，递归查找该目录  
				std::wstring newPath = path + L"/" + findData.cFileName;
				__Resource__FindFilesRecursively(newPath, result);
			}
			else {
				std::wstring newPath = path + L"/" + findData.cFileName;
				result->push_back(newPath);
			}
		} while (FindNextFileW(findHandle, &findData));
		FindClose(findHandle);
	}
	void  Resource::Package(const EString& dir, const EString& outFile) {
		std::list<Entry> items;
		std::list<std::wstring> files;
		__Resource__FindFilesRecursively(dir.unicode(), &files);
		DWORD headOffset = sizeof(DWORD);
		std::ofstream ofs(outFile.unicode(), std::ios::binary);
		ofs.seekp(headOffset);
		for (auto& file : files) {
			//读取文件大小
			std::ifstream ifs(file, std::ios::binary);
			ifs.seekg(0, std::ios::end);
			auto size = ifs.tellg();
			//读取文件
			EString data;
			data.resize(size);
			ifs.seekg(0);
			ifs.read((char*)data.c_str(), size);
			//记录文件名称,偏移,大小
			Entry item;
			item.name = file;
			item.size = size;
			item.offset = headOffset;
			ofs.write(data.c_str(), data.size());
			headOffset += data.size();
			items.push_back(item);
		}
		//将偏移文件头偏移信息写入到文件初始位置
		ofs.seekp(0);
		ofs.write((char*)(&headOffset), 4);
		//设置到文件条目位置
		ofs.seekp(headOffset);
		//处理路径
		EString root = dir + "/";
		root = root.Replace("\\", "/");
		root = root.Replace("//", "/");
		if (root[root.size() - 1] = '/') {
			root.erase(root.size() - 1, 1);
		}
		size_t pos = root.rfind("/");
		if (pos != size_t(-1)) {
			root = root.substr(0, pos + 1);
		}
		for (auto& item : items) {
			//写入文件偏移位置
			ofs.write((char*)(&item.offset), 4);
			//写入文件大小
			ofs.write((char*)(&item.size), 4);
			//文件路径名称
			EString name = item.name;
			name = name.Replace("\\", "/");
			name = name.Replace("//", "/");
			name = name.Replace(root, "");
			ofs.write(name.c_str(), name.size() + 1);
		}
		//首位呼应
		//ofs.write((char*)(&headOffset), 4);
		ofs.flush();
		ofs.close();
	}

	void Resource::UnPackage() {
		auto& ifs = *(this->rStream);
		if (ifs.size() < 8) {
			//不是标准的资源文件 不执行解析
			ASSERT(!"error resource");
			return;
		}
		//读取记录文件位置的偏移
		ifs.seekg(0);
		DWORD headOffset;
		ifs.read((char*)&headOffset, 4);
		////读取末尾
		//ifs.seekg(ifs.size() - 4);
		//DWORD endValue;
		//ifs.read((char*)&endValue, 4);
		//if (headOffset != endValue) {
		//	//不是标准的资源文件 不执行解析
		//	ASSERT(!"error resource");
		//	return;
		//}
		//开始读取文件剩余条目
		ifs.seekg(headOffset);
		while (ifs.tellg() < ifs.size())
		{
			//读取到文件偏移位置
			DWORD fileOffset;
			ifs.read((char*)&fileOffset, 4);
			//读取文件大小
			DWORD fileSize;
			ifs.read((char*)&fileSize, 4);
			//读取文件名称
			char buf[512];
			for (size_t i = 0; i < sizeof(buf); i++)
			{
				char ch;
				ifs.read((char*)&ch, 1);
				buf[i] = ch;
				if (ch == 0) {
					break;
				}
			}
			Entry item;
			item.offset = fileOffset;
			item.size = fileSize;
			item.name = buf;
			items.push_back(item);
		}
	}

	Resource::Resource(const EString& resFile) {
		this->rStream = new ReadStream(resFile);
		this->UnPackage();
	}
	Resource::Resource(HRSRC hRsrc) {
		this->rStream = new ReadStream(hRsrc);
		this->UnPackage();
	}
	Resource:: ~Resource() {
		if (rStream) {
			delete rStream;
		}
	}
	bool Resource::GetFile(const EString& fileName, std::string* out) {
		for (const auto& it : items) {
			if (it.name == fileName) {
				out->resize(it.size);
				rStream->seekg(it.offset);
				rStream->read((char*)out->c_str(), it.size);
				return true;
			}
		}
		return false;
	}
};

