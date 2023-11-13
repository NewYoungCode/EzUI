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

	//Ѱ��ָ��Ŀ¼�Լ�Ŀ¼�µ������ļ�
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
				// �����Ŀ¼���ݹ���Ҹ�Ŀ¼  
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
			//��ȡ�ļ���С
			std::ifstream ifs(file, std::ios::binary);
			ifs.seekg(0, std::ios::end);
			auto size = ifs.tellg();
			//��ȡ�ļ�
			EString data;
			data.resize(size);
			ifs.seekg(0);
			ifs.read((char*)data.c_str(), size);
			//��¼�ļ�����,ƫ��,��С
			Entry item;
			item.name = file;
			item.size = size;
			item.offset = headOffset;
			ofs.write(data.c_str(), data.size());
			headOffset += data.size();
			items.push_back(item);
		}
		//��ƫ���ļ�ͷƫ����Ϣд�뵽�ļ���ʼλ��
		ofs.seekp(0);
		ofs.write((char*)(&headOffset), 4);
		//���õ��ļ���Ŀλ��
		ofs.seekp(headOffset);
		//����·��
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
			//д���ļ�ƫ��λ��
			ofs.write((char*)(&item.offset), 4);
			//д���ļ���С
			ofs.write((char*)(&item.size), 4);
			//�ļ�·������
			EString name = item.name;
			name = name.Replace("\\", "/");
			name = name.Replace("//", "/");
			name = name.Replace(root, "");
			ofs.write(name.c_str(), name.size() + 1);
		}
		//��λ��Ӧ
		//ofs.write((char*)(&headOffset), 4);
		ofs.flush();
		ofs.close();
	}

	void Resource::UnPackage() {
		auto& ifs = *(this->rStream);
		if (ifs.size() < 8) {
			//���Ǳ�׼����Դ�ļ� ��ִ�н���
			ASSERT(!"error resource");
			return;
		}
		//��ȡ��¼�ļ�λ�õ�ƫ��
		ifs.seekg(0);
		DWORD headOffset;
		ifs.read((char*)&headOffset, 4);
		////��ȡĩβ
		//ifs.seekg(ifs.size() - 4);
		//DWORD endValue;
		//ifs.read((char*)&endValue, 4);
		//if (headOffset != endValue) {
		//	//���Ǳ�׼����Դ�ļ� ��ִ�н���
		//	ASSERT(!"error resource");
		//	return;
		//}
		//��ʼ��ȡ�ļ�ʣ����Ŀ
		ifs.seekg(headOffset);
		while (ifs.tellg() < ifs.size())
		{
			//��ȡ���ļ�ƫ��λ��
			DWORD fileOffset;
			ifs.read((char*)&fileOffset, 4);
			//��ȡ�ļ���С
			DWORD fileSize;
			ifs.read((char*)&fileSize, 4);
			//��ȡ�ļ�����
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

