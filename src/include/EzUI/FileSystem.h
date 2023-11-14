#pragma once
#include "EString.h"
namespace EzUI {
	struct FileInfo final {
		EString Name;
		DWORD Attr;
		bool IsDirectory() {
			return (Attr & FILE_ATTRIBUTE_DIRECTORY);
		}
	};
	namespace File {
		//判断文件是否存在
		UI_EXPORT bool Exists(const EString& filenNme);
		//拷贝文件
		UI_EXPORT bool Copy(const EString& src, const EString& desc);
		//删除文件
		UI_EXPORT bool Delete(const EString& file);
		//文件移动或者改名
		UI_EXPORT bool Move(const EString& oldName, const EString& newName);
		//创建一个文件(如果文件已存在则清空其内容)
		UI_EXPORT bool Create(const EString& fileName);
		//将指定数据以二进制方式写入文件(如果文件存在内容则追加)
		UI_EXPORT bool Write(const char* fileData, size_t fileSize, const EString& outFileName);
		//读取文件到内存中
		UI_EXPORT size_t Read(const EString& fileName, std::string* data);
	}
	namespace Path {
		//格式化路径
		UI_EXPORT void Format(std::string* str);
		//获取文件名(不包括目录名 不包括扩展名) 
		UI_EXPORT EString GetFileNameWithoutExtension(const EString& _filename);
		//获取文件所在目录
		UI_EXPORT EString GetDirectoryName(const EString& _filename);
		//获取文件扩展名
		UI_EXPORT EString GetExtension(const EString& _filename);
		//获取文件名(文件名+扩展名)
		UI_EXPORT EString GetFileName(const EString& filename);
	}
	namespace Directory {
		//检测目录是否存在
		UI_EXPORT bool Exists(const EString& directoryNme);
		//创建目录 不存在的多级目录将会自动创建
		UI_EXPORT bool Create(const EString& path);
		//将目录和目录下的文件复制到指定的位置
		UI_EXPORT void Copy(const EString& srcPath, const EString& desPath);
		//使用通配符搜索文件和目录
		UI_EXPORT void Find(const EString& directory, std::vector<FileInfo>* result, const EString& pattern = "*.*", bool loopDir = true);
	}
};
