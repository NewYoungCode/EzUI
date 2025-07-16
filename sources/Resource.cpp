#include "Resource.h"
#include "ezui.h"
namespace ezui {
	Resource::ReadStream::ReadStream(HRSRC hRsrc) {
		this->m_ptr = (char*)::LoadResource(ezui::__EzUI__HINSTANCE, hRsrc);
		this->m_count = ::SizeofResource(ezui::__EzUI__HINSTANCE, hRsrc);
	}
	Resource::ReadStream::ReadStream(const UIString& fileName) {
		this->m_ifs = new std::ifstream(fileName.unicode(), std::ios::binary);
		this->m_ifs->seekg(0, std::ios::end);
		this->m_count = m_ifs->tellg();
		this->m_ifs->seekg(0);
	}
	void Resource::ReadStream::seekg(std::streampos pos) {
		if (m_ifs) {
			m_ifs->seekg(pos);
		}
		else {
			this->m_pos = pos;
		}
	}
	void Resource::ReadStream::read(char* buf, std::streamsize count) {
		if (m_ifs) {
			m_ifs->read(buf, count);
		}
		else {
			::memcpy(buf, m_ptr + m_pos, count);
			this->m_pos += count;
		}
	}
	std::streampos Resource::ReadStream::tellg() {
		if (m_ifs) {
			return m_ifs->tellg();
		}
		else {
			return m_pos;
		}
	}
	const  std::streamsize Resource::ReadStream::size() {
		return this->m_count;
	}
	Resource::ReadStream::~ReadStream() {
		if (m_ifs) {
			delete m_ifs;
		}
		if (m_ptr) {
			::FreeResource((HGLOBAL)m_ptr);
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
	void  Resource::Package(const UIString& dir, const UIString& outFile, const std::function<void(const UIString&, int, int)>& packCallback) {

		if (dir.empty()) {
			ASSERT(!"DirName is Empty !");
		}

		std::list<Entry> items;
		std::list<std::wstring> files;
		__Resource__FindFilesRecursively(dir.unicode(), &files);
		std::streamsize headOffset = sizeof(std::streamsize);
		std::ofstream ofs(outFile.unicode(), std::ios::binary);
		ofs.seekp(headOffset);
		for (auto& file : files) {
			//读取文件大小
			std::ifstream ifs(file, std::ios::binary);
			ifs.seekg(0, std::ios::end);
			auto size = ifs.tellg();
			//读取文件
			UIString data;
			data.resize(size);
			ifs.seekg(0);
			ifs.read((char*)data.c_str(), size);
			//记录文件名称,偏移,大小
			Entry item;
			item.Name = file;
			item.Size = size;
			item.Offset = headOffset;
			ofs.write(data.c_str(), data.size());
			headOffset += data.size();
			items.push_back(item);
		}
		//将偏移文件头偏移信息写入到文件初始位置
		ofs.seekp(0);
		ofs.write((char*)(&headOffset), sizeof(headOffset));
		//设置到文件条目位置
		ofs.seekp(headOffset);
		//处理路径
		UIString root = dir + "/";
		root = root.replace("\\", "/");
		root = root.replace("//", "/");
		if (root[root.size() - 1] == '/') {
			root.erase(root.size() - 1, 1);
		}
		size_t pos = root.rfind("/");
		if (pos != size_t(-1)) {
			root = root.substr(0, pos + 1);
		}
		int index = 0;
		for (auto& item : items) {
			//写入文件偏移位置
			ofs.write((char*)(&item.Offset), sizeof(headOffset));
			//写入文件大小
			ofs.write((char*)(&item.Size), sizeof(headOffset));
			//文件路径名称
			UIString name = item.Name;
			name = name.replace("\\", "/");
			name = name.replace("//", "/");
			name = name.replace(root, "", false);
			ofs.write(name.c_str(), name.size() + 1);
			if (packCallback) {
				packCallback(name, index, items.size());
			}
			index++;
		}
		//首位呼应
		ofs.write((char*)(&headOffset), sizeof(headOffset));
		ofs.flush();
		ofs.close();
	}

	void Resource::UnPackage() {
		m_isGood = false;
		std::list<Entry>& items = (std::list<Entry>&)this->Items;
		auto& ifs = *(this->m_rStream);
		if (ifs.size() < sizeof(std::streamsize) * 2) {
			//不是标准的资源文件 不执行解析
			ASSERT(!"error resource");
			return;
		}
		//读取记录文件位置的偏移
		ifs.seekg(0);
		std::streamsize headOffset;
		ifs.read((char*)&headOffset, sizeof(headOffset));
		//读取末尾
		ifs.seekg(ifs.size() - sizeof(headOffset));
		std::streamsize endValue;
		ifs.read((char*)&endValue, sizeof(headOffset));
		if (headOffset != endValue) {
			//不是标准的资源文件 不执行解析
			ASSERT(!"error resource");
			return;
		}
		m_isGood = true;
		//开始读取文件剩余条目
		ifs.seekg(headOffset);
		std::streampos endPos = ifs.size() - sizeof(headOffset);
		while (ifs.tellg() < endPos)
		{
			//读取到文件偏移位置
			std::streamsize fileOffset;
			ifs.read((char*)&fileOffset, sizeof(headOffset));
			//读取文件大小
			std::streamsize fileSize;
			ifs.read((char*)&fileSize, sizeof(headOffset));
			//读取文件名称
			std::vector<char> buf(32768);
			for (int i = 0; i < buf.size(); i++)
			{
				char ch;
				ifs.read((char*)&ch, 1);
				buf[i] = ch;
				if (ch == 0) {
					break;
				}
			}
			Entry item;
			item.Offset = fileOffset;
			item.Size = fileSize;
			item.Name = buf.data();
			items.push_back(item);
		}
	}

	Resource::Resource(const UIString& resFile) {
		this->m_rStream = new ReadStream(resFile);
		this->UnPackage();
	}
	Resource::Resource(HRSRC hRsrc) {
		this->m_rStream = new ReadStream(hRsrc);
		this->UnPackage();
	}
	Resource:: ~Resource() {
		if (m_rStream) {
			delete m_rStream;
		}
	}
	bool Resource::GetFile(const UIString& fileName, std::string* out) {
		for (const auto& it : this->Items) {
			if (it.Name == fileName) {
				out->resize(it.Size);
				m_rStream->seekg(it.Offset);
				m_rStream->read((char*)out->c_str(), it.Size);
				return true;
			}
		}
		return false;
	}
	bool Resource::IsGood() {
		return m_isGood;
	}
	void Resource::GetFile(const Entry& item, std::string* out) {
		out->resize(item.Size);
		m_rStream->seekg(item.Offset);
		m_rStream->read((char*)out->c_str(), item.Size);
	}
};

