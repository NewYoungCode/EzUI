#pragma once
#include "General.h"
#include <time.h>
namespace Time {
	/// <summary>
	/// 时间戳转字符串
	/// </summary>
	/// <param name="time_"></param>
	/// <returns></returns>
	inline std::string TimeStampToString(time_t time_, const std::string& format = "%Y-%m-%d %H:%M:%S") {
		char timeStr[64]{ 0 };
		struct tm* ttime;
		ttime = ::localtime(&time_);
		::strftime(timeStr, 64, format.c_str(), ttime);
		return std::string(timeStr);
	}
	/// <summary>
	/// 字符串转时间戳
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	inline time_t StringToTimeStamp(const std::string& str, const std::string& format = "%d-%d-%d %d:%d:%d") {
		struct tm tm_;
		int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
		::sscanf_s(str.c_str(), format.c_str(), &year, &month, &day, &hour, &minute, &second);
		tm_.tm_year = year - 1900;
		tm_.tm_mon = month - 1;
		tm_.tm_mday = day;
		tm_.tm_hour = hour;
		tm_.tm_min = minute;
		tm_.tm_sec = second;
		tm_.tm_isdst = 0;
		time_t timeStamp = ::mktime(&tm_);
		return timeStamp;
	}

	inline SYSTEMTIME StringToSYSTEMTIME(const std::string& str, const std::string& format = "%d-%d-%d %d:%d:%d") {
		SYSTEMTIME tm_;
		int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
		::sscanf_s(str.c_str(), format.c_str(), &year, &month, &day, &hour, &minute, &second);
		tm_.wYear = year;
		tm_.wMonth = month;
		tm_.wDay = day;
		tm_.wHour = hour;
		tm_.wMinute = minute;
		tm_.wSecond = second;
		return tm_;
	}

	inline time_t FileTimeToTime(const FILETIME& ft) {
		time_t t;
		LONGLONG  ll;
		ULARGE_INTEGER            ui;
		ui.LowPart = ft.dwLowDateTime;
		ui.HighPart = ft.dwHighDateTime;
		ll = (ft.dwHighDateTime << 32) + ft.dwLowDateTime;  //这一步是不是多余的
		t = ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
		//将ui.QuadPart的数据类型ULONGLONG强制转换为time_t的LONGLONG数据类型
		return t;
	}

	namespace Now {
		std::string ToString(const std::string& format = "yyyy-MM-dd hh:mm:ss");
		template<class T>
		//统计函数耗时时间
		inline time_t StopWatch(const T& _func) {
			auto beg_t = std::chrono::system_clock::now();    //起始时间
			_func();
			auto end_t = std::chrono::system_clock::now();    //结束时间
			std::chrono::duration<double> diff = end_t - beg_t;//时间间隔s
			time_t total = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();//时间间隔ms
			printf("Elapsed :%d ms\n", total);
			return total;
		}
	}
};
