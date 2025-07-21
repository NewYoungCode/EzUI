#pragma once
#include <iostream>

#include "FileSystem.h"
#include "Time.hpp"
namespace Log {
	//是否启用日志
	extern bool Enable;
	extern bool WriteFile;
	extern void WriteLog(const Text::String& log);

	template<typename ...T>
	/// <summary>
	/// 打印utf8的字符
	/// </summary>
	/// <typeparam name="...T"></typeparam>
	/// <param name="formatStr"></param>
	/// <param name="...args"></param>
	inline Text::String Info(const Text::String& formatStr, const T &...args) {
		if (!Enable)return "";
		// 计算格式化后的字符串所需的内存大小
		int bufSize = ::snprintf(nullptr, 0, formatStr.c_str(), args...) + 2;  // +1是为了换行符和结束符 \n '\0'
		char* buf = new char[bufSize];
		auto count = ::sprintf_s(buf, bufSize, formatStr.c_str(), std::forward<const T&>(args)...);
		buf[count] = '\n';
		buf[count + 1] = 0;
		Text::String info(buf);
		info = Time::Now().ToString("HH:mm:ss ") + info;
		delete[] buf;
		//转为本地可识别的编码
		auto ansi = info.ansi();
		std::cout << ansi;
		OutputDebugStringA(ansi.c_str());
		WriteLog(info);
		return info;
	}
};
