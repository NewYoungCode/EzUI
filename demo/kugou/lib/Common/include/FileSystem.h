#pragma once
#include <sys/stat.h>
#include <direct.h>
#include "General.h"
#include "Text.h"
namespace File {
	//创建文件
	bool Create(const std::string &filename);
	//删除文件
	bool Delete(const std::string &filename);
	//判断文件是否存在
	bool Exists(const std::string&filename);
	//文件移动或者改名
	bool Move(const std::string &oldname, const std::string &newname);
	//读取文件并out返回
	void ReadFile(const  std::string &filename, std::string&outData);
	//写入文件
	void WriteFile(const std::stringstream & data, const std::string & filename);
	void Copy(const  std::string &filename, const  std::string &des_filename,bool cover=true);
	std::string CreateTempFile(const std::string&filename);
}
namespace Path {
	//自己写的文件监控类
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
	//创建路径  MultiDir:是否创建多级目录
	bool Create(const std::string &path);
	//删除路径 如果存在子文件夹或者文件 将会递归删除
	bool Delete(const std::string& directoryName);
	//通配符搜索文件
	std::vector<std::string> SearchFiles(const std::string &path, const std::string &pattern);
	//检查路径是否存在
	bool Exists(const std::string &path);
	//获取文件名称(文件名称)
	std::string GetFileNameWithoutExtension(const std::string &_filename);
	//获取文件目录名称(所在目录)
	std::string GetDirectoryName(const std::string &_filename);
	//获取文件名称+后缀
	std::string GetFileName(const std::string &_filename);
	//获取文件后缀名(后缀名)
	std::string GetExtension(const std::string &_filename);
	//获取进程所在绝对路径目录
	std::string	StartPath();
	//获取进程所在绝对路径包含文件名称
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