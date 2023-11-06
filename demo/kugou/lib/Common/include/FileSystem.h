#pragma once
#include <sys/stat.h>
#include <direct.h>
#include "General.h"
#include "Text.h"
namespace File {
	//�����ļ�
	bool Create(const std::string &filename);
	//ɾ���ļ�
	bool Delete(const std::string &filename);
	//�ж��ļ��Ƿ����
	bool Exists(const std::string&filename);
	//�ļ��ƶ����߸���
	bool Move(const std::string &oldname, const std::string &newname);
	//��ȡ�ļ���out����
	void ReadFile(const  std::string &filename, std::string&outData);
	//д���ļ�
	void WriteFile(const std::stringstream & data, const std::string & filename);
	void Copy(const  std::string &filename, const  std::string &des_filename,bool cover=true);
	std::string CreateTempFile(const std::string&filename);
}
namespace Path {
	//�Լ�д���ļ������
	class FileWatcher {
	private:
		std::string math = "*.*";
		std::string path;
		std::function<void(const std::string& filename)> callback = NULL;
		size_t sleep;
		bool ctn = true;
		void TaskFunc();
	public:
		FileWatcher(const std::string& path, const std::string& math, const std::function<void(const std::string& filename)>& callback, size_t sleep = 500);
		~FileWatcher();
	};
	//����·��  MultiDir:�Ƿ񴴽��༶Ŀ¼
	bool Create(const std::string &path);
	//ɾ��·�� ����������ļ��л����ļ� ����ݹ�ɾ��
	bool Delete(const std::string& directoryName);
	//ͨ��������ļ�
	std::vector<std::string> SearchFiles(const std::string &path, const std::string &pattern);
	//���·���Ƿ����
	bool Exists(const std::string &path);
	//��ȡ�ļ�����(�ļ�����)
	std::string GetFileNameWithoutExtension(const std::string &_filename);
	//��ȡ�ļ�Ŀ¼����(����Ŀ¼)
	std::string GetDirectoryName(const std::string &_filename);
	//��ȡ�ļ�����+��׺
	std::string GetFileName(const std::string &_filename);
	//��ȡ�ļ���׺��(��׺��)
	std::string GetExtension(const std::string &_filename);
	//��ȡ�������ھ���·��Ŀ¼
	std::string	StartPath();
	//��ȡ�������ھ���·�������ļ�����
	std::string	StartFileName();
};
namespace FileSystem {
	typedef enum : char {
		None,
		File,
		Directory
	}FileType;
	struct FileInfo
	{
	private:
		std::ifstream *fs = NULL;
	public:
		unsigned long long StreamPos = 0;
		struct _stat64 __stat;
		FileType FileType = FileSystem::FileType::None;
		std::string Extension;
		std::string FullName;
		std::string FileName;
		bool ReadOnly = false;
		size_t Read(char*_buf_, size_t _rdCount = 512) {
			size_t rdbufCount = _rdCount;
			if (StreamPos + _rdCount >= __stat.st_size) {
				rdbufCount = __stat.st_size - StreamPos;
			}
			if (rdbufCount == 0) {
				return 0;
			}
			if (fs == NULL) {
				fs = new std::ifstream(FullName, std::ios::binary);
			}
			fs->seekg(StreamPos);
			fs->read(_buf_, rdbufCount);
			StreamPos += rdbufCount;
			return rdbufCount;
		}
		FileInfo() {}
		FileInfo(const std::string&filename) {
			int status = _stat64(filename.c_str(), &__stat);
			if (status == 0 && (__stat.st_mode & S_IFREG) == S_IFREG) {
				Extension = Path::GetExtension(filename);
				FileName = Path::GetFileName(filename);
				FullName = filename;
				FileType = FileType::File;
			}
		}
		void Close() {
			if (fs) {
				fs->close();
				delete fs;
				fs = NULL;
			}
		}
		~FileInfo() {
			if (fs) {
				fs->close();
				delete fs;
			}
		}
	};
	void ReadFileInfoWin32(const std::string& directory, WIN32_FIND_DATAA&pNextInfo, std::vector<FileSystem::FileInfo>&result);
	size_t  Find(const std::string& directory, std::vector<FileSystem::FileInfo>&result, const std::string& pattern = "*.*");
}