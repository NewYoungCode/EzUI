#pragma once
#include <Windows.h>
#include <time.h>
#include <string>
#include "Text.h"
#include <sstream>
#include <iomanip>

//日期时间类 秒级别
class DateTime {
	time_t m_time;
	Text::String m_data_str;
public:
	static DateTime Now();
public:
	DateTime(time_t second);
	DateTime(const std::string& timeStr, const std::string& format = "yyyy-MM-dd HH:mm:ss");
	std::string ToString(const std::string& format = "yyyy-MM-dd HH:mm:ss") const;
	time_t operator - (const DateTime& right);
	bool operator == (const DateTime& right);
	bool operator != (const DateTime& right);
	bool operator > (const DateTime& right);
	bool operator < (const DateTime& right);
	DateTime& operator += (time_t right);
	DateTime& operator -= (time_t right);
	DateTime operator + (time_t right);
	DateTime operator - (time_t right);
};