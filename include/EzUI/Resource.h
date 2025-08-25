#pragma once
#include "UIString.h"

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
			std::streampos m_pos = 0;
			std::streamsize m_count = 0;
			const char* m_ptr = NULL;
			std::ifstream* m_ifs = NULL;
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
		ReadStream* m_rStream = NULL;
		void UnPackage();
		bool m_isGood = false;
	public:
		const std::list<Entry> Items;
		bool IsGood();
		//对资源目录进行打包
		static void Package(const UIString& dir, const UIString& outFile, const std::function<void(const UIString&, int, int)>& packCallback = NULL);
	public:
		virtual ~Resource();
		//从本地文件创建资源对象
		Resource(const UIString& resFile);
		//使用windows内置资源文件创建资源对象
		Resource(HRSRC hRsrc);
		//寻找资源中的文件
		bool GetFile(const UIString& fileName, std::string* out);
		//传入item直接返回数据
		void GetFile(const Entry& item, std::string* out);
	};
};