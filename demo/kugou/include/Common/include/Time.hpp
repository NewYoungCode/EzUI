#pragma once
#include <Windows.h>
#include <time.h>
#include <string>
#include "Text.h"
#include <sstream>
#include <iomanip>

class Time {
	time_t _time;
public:
	static Time Now() {
		return Time(::time(NULL));
	}
public:
	Time(time_t second) {
		_time = second;
	}
	Time(const std::string& timeStr, const std::string& format = "yyyy-MM-dd HH:mm:ss") {
		std::string formatStr = format;
		Text::Replace(&formatStr, "yyyy", "%Y");
		Text::Replace(&formatStr, "MM", "%m");
		Text::Replace(&formatStr, "dd", "%d");
		Text::Replace(&formatStr, "HH", "%H");
		Text::Replace(&formatStr, "mm", "%M");
		Text::Replace(&formatStr, "ss", "%S");

		std::tm tm{};
		std::istringstream ss(timeStr);
		ss >> std::get_time(&tm, formatStr.c_str());
		tm.tm_isdst = -1; // Enable daylight saving time

		std::time_t timeStamp = std::mktime(&tm);
		if (timeStamp == -1) {
			throw std::runtime_error("Error converting time string to timestamp.");
		}
		this->_time = timeStamp;
	}
	std::string ToString(const std::string& format = "yyyy-MM-dd HH:mm:ss") const{
		std::string formatStr = format;
		Text::Replace(&formatStr, "yyyy", "%Y");
		Text::Replace(&formatStr, "MM", "%m");
		Text::Replace(&formatStr, "dd", "%d");
		Text::Replace(&formatStr, "HH", "%H");
		Text::Replace(&formatStr, "mm", "%M");
		Text::Replace(&formatStr, "ss", "%S");

		char timeStr[128]{ 0 };
		time_t t = _time;
		struct tm* ttime = std::localtime(&t);
		std::strftime(timeStr, sizeof(timeStr), formatStr.c_str(), ttime);
		return timeStr;
	}
	time_t operator - (const Time& right) {
		return this->_time - right._time;
	}
	bool operator == (const Time& right) {
		return  this->_time == right._time;
	}
	bool operator != (const Time& right) {
		return  this->_time != right._time;
	}
	bool operator > (const Time& right) {
		return  this->_time > right._time;
	}
	bool operator < (const Time& right) {
		return  this->_time < right._time;
	}
	Time& operator += (time_t right) {
		this->_time += right;
		return  *this;
	}
	Time& operator -= (time_t right) {
		this->_time -= right;
		return  *this;
	}
	Time operator + (time_t right) {
		return  Time(this->_time + right);
	}
	Time operator - (time_t right) {
		return  Time(this->_time - right);
	}
};