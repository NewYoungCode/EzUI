#pragma once
#include "UIString.h"
#include <ios>

namespace ezui {
	/// <summary>
	/// 框架中的资源类
	/// </summary>
	class UI_EXPORT Resource {
	public:
		struct Entry {
			std::streampos Offset = 0;//偏移
			std::streamsize Size = 0;//大小
			UIString Name;//名称
		};
		//资源文件读取流
		class UI_EXPORT ReadStream {
			std::streampos _pos = 0;
			std::streamsize _count = 0;
			const char* _ptr = NULL;
			std::ifstream* _ifs = NULL;
		public:
			ReadStream(HRSRC hRsrc);
			ReadStream(const UIString& fileName);
			void seekg(std::streampos pos);
			void read(char* buf, std::streamsize count);
			std::streampos tellg();
			const std::streamsize size();
			virtual ~ReadStream();
		};
	private:
		ReadStream* _rStream = NULL;
		void UnPackage();
		bool _isGood = false;
	public:
		const std::list<Entry> Items;
		bool IsGood();
		//对资源目录进行打包
		static void Package(const UIString& dir, const UIString& outFile, const std::function<void(const UIString&, int, int)>& packCallback = NULL);
		static HRSRC FindRC(const UIString& rcIDName);
	public:
		virtual ~Resource();
		//从本地文件创建对象
		Resource(const UIString& resFile);
		//使用vs内置资源文件创建对象
		Resource(HRSRC hRsrc);
		//寻找资源中的文件
		bool GetFile(const UIString& fileName, std::string* out);
		//传入item直接返回数据
		void GetFile(const Entry& item, std::string* out);
	};
};