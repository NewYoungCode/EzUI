#pragma once
#include "EString.h"
namespace EzUI {
	/// <summary>
	/// 资源类
	/// </summary>
	class UI_EXPORT Resource {
	public:
		struct Entry {
			size_t offset = 0;
			size_t size = 0;
			EString name;
		};
		//资源文件读取流
		class UI_EXPORT ReadStream {
			size_t pos = 0;
			size_t count = 0;
			const char* ptr = NULL;
			std::ifstream* ifs = NULL;
		public:
			ReadStream(HRSRC hRsrc);
			ReadStream(const EString& fileName);
			void seekg(size_t pos);
			void read(char* buf, size_t count);
			size_t tellg();
			const size_t& size();
			virtual ~ReadStream();
		};
	private:
		std::list<Entry> items;
		ReadStream* rStream = NULL;
		void UnPackage();
	public:
		//对资源目录进行打包
		UI_EXPORT static void Package(const EString& dir, const EString& outFile);
	public:
		virtual ~Resource();
		//从本地文件创建对象
		Resource(const EString& resFile);
		//使用vs内置资源文件创建对象
		Resource(HRSRC hRsrc);
		//寻找资源中的文件
		bool GetFile(const EString& fileName, std::string* out);
	};
};