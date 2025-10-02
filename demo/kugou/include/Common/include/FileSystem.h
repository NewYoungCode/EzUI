#pragma once
#include <functional>
#include <fstream>
#include <Windows.h>
#include "Text.h"

namespace FileSystem {
	class FileInfo;
	enum FileType :int
	{
		File = 2,
		Directory = 4
	};
	//重载枚举的 | 运算符
	inline FileType operator|(FileType left, FileType right)
	{
		return static_cast<FileType>(static_cast<int>(left) | static_cast<int>(right));
	}
};

namespace File {
	typedef std::string FileStream;
	//创建文件
	extern bool Create(const Text::String& filename);
	//删除文件
	extern bool Delete(const Text::String& filename);
	//判断文件是否存在
	extern bool Exists(const Text::String& filename);
	//文件移动或者改名
	extern bool Move(const Text::String& oldname, const Text::String& newname);
	//读取文件并out返回
	extern bool ReadFile(const  Text::String& filename, FileStream* fileStream);
	//写入文件
	extern bool WriteFile(const FileStream* fileStream, const Text::String& filename);
	//写入文件
	extern bool WriteFile(const char* fileStream, size_t count, const Text::String& filename);
	//拷贝文件
	extern bool Copy(const  Text::String& filename, const  Text::String& des_filename, bool overwrite = true);
	//获取文件字节大小
	extern ULONGLONG GetFileSize(const Text::String& fileName);
};

namespace Directory {
	//创建目录
	extern bool Create(const Text::String& path);
	//拷贝目录所有文件到目标目录
	extern bool Copy(const Text::String& srcPath, const Text::String& desPath, bool overwrite = true);
	//移动目录到新位置
	extern bool Move(const Text::String& oldname, const Text::String& newname);
	//删除路径 如果存在子文件夹或者文件 将会递归删除
	extern bool Delete(const Text::String& directoryName);
	//通配符查找文件夹/文件
	extern size_t Find(const Text::String& path, std::vector<FileSystem::FileInfo>& result, const Text::String& pattern = "*.*", bool loopSubDir = false, FileSystem::FileType fileType = FileSystem::FileType::Directory | FileSystem::FileType::File);
	//检查路径是否存在
	extern bool Exists(const Text::String& path);
};

namespace Path {
	//格式化路径为统一反斜杠
	extern Text::String Format(const Text::String& path);
	//判断路径是不是相同
	extern bool Equal(const Text::String& path1, const Text::String& path2);
	//获取文件名称(文件名称)
	extern Text::String GetFileNameWithoutExtension(const Text::String& _filename);
	//获取文件目录名称(所在目录)
	extern Text::String GetDirectoryName(const Text::String& _filename);
	//获取文件名称+后缀
	extern Text::String GetFileName(const Text::String& _filename);
	//获取用户桌面路径
	extern Text::String UserDesktop(bool publicUser = true);
	//获取开始菜单路径
	extern Text::String StartPrograms(bool publicUser = true);
	//获取文件后缀名(后缀名)
	extern Text::String GetExtension(const Text::String& _filename);
	//获取进程所在绝对路径目录
	extern Text::String StartPath();
	//获取进程所在绝对路径包含文件名称
	extern const Text::String& StartFileName();
#undef GetTempPath
	/// <summary>
	/// 获取应当前windows用户的临时目录
	/// </summary>
	/// <returns></returns>
	extern Text::String GetTempPath();
	/// <summary>
	/// 获取应用程序的临时目录
	/// </summary>
	/// <returns></returns>
	extern Text::String GetAppTempPath(const Text::String& appName = "");
	/// <summary>
	/// 获取应用程序数据存储目录 C:/Users/%s/AppData/Local/%s
	/// </summary>
	/// <returns></returns>
	extern Text::String GetAppDataPath(const Text::String& appName = "");
};
namespace FileSystem {
	class FileInfo
	{
	private:
		std::ifstream* ifs = NULL;
		ULONGLONG StreamPos = 0;
	public:
		DWORD dwFileAttributes;
		const Text::String FileName;
		bool IsFile() const {
			return !(dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		}
		bool IsReadOnly() const {
			return dwFileAttributes & FILE_ATTRIBUTE_READONLY;
		}
		const ULONGLONG FileSize = 0;
		FileInfo() {}
		FileInfo(const Text::String& fileName) {
			if (File::Exists(fileName)) {
				(Text::String)FileName = fileName;
				//获取大小
				ifs = new std::ifstream(fileName.unicode(), std::ios::binary);
				ifs->seekg(0, std::ios::end);
				(ULONGLONG&)FileSize = ifs->tellg();
			}
		}
		size_t Read(char* _buf_, size_t _rdCount = 255) {
			size_t rdbufCount = _rdCount;
			if (StreamPos + _rdCount >= FileSize) {
				rdbufCount = FileSize - StreamPos;
			}
			if (rdbufCount == 0) {
				return 0;
			}
			if (ifs == NULL) {
				ifs = new std::ifstream(FileName.unicode(), std::ios::binary);
			}
			ifs->seekg(StreamPos);
			ifs->read(_buf_, rdbufCount);
			StreamPos += rdbufCount;
			return rdbufCount;
		}
		void Close() {
			ifs->close();
		}
		~FileInfo() {
			if (ifs) {
				delete ifs;
			}
		}
	};
}