#pragma once
#include "EString.h"
namespace EzUI {
	/// <summary>
	/// 框架中的资源类
	/// </summary>
	class UI_EXPORT Resource {
	public:
		struct Entry {
			size_t Offset = 0;//偏移
			size_t Size = 0;//大小
			EString Name;//名称
		};
		//资源文件读取流
		class UI_EXPORT ReadStream {
			size_t _pos = 0;
			size_t _count = 0;
			const char* _ptr = NULL;
			std::ifstream* _ifs = NULL;
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
		ReadStream* _rStream = NULL;
		void UnPackage();
	public:
		const std::list<Entry> Items;
		const bool IsGood = false;
		//对资源目录进行打包
		UI_EXPORT static void Package(const EString& dir, const EString& outFile, const std::function<void(const EString&, int, int)>& packCallback = NULL);
	public:
		virtual ~Resource();
		//从本地文件创建对象
		Resource(const EString& resFile);
		//使用vs内置资源文件创建对象
		Resource(HRSRC hRsrc);
		//寻找资源中的文件
		bool GetFile(const EString& fileName, std::string* out);
		//传入item直接返回数据
		void GetFile(const Entry& item, std::string* out);
	};
};