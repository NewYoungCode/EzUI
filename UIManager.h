#pragma once
#include "EzUI.h"
#include "Control.h"

namespace UIManager {
	extern std::map<EString, EString> styles;//默认样式集合
	extern std::map<EString, EString> styles_active;//按下样式集合
	extern std::map<EString, EString> styles_hover;//鼠标悬浮样式集合
	Control* LoadControl(const EString & filename);

	inline size_t  Replace(std::string  &str, const std::string & oldText, const std::string & newText)
	{
		std::string &newStr = str;
		size_t pos;
		pos = str.find(oldText);
		size_t count = 0;
		for (; pos != std::string::npos;) {
			newStr.replace(pos, oldText.size(), newText);
			count++;
			pos = newStr.find(oldText);
		}
		return count;
	}
	//去除空格或者其他符号 双引号内的空格不会去除
	inline void TrimStyle(EString&str, TCHAR _char = ' ') {
		TCHAR *bufStr = new TCHAR[str.size()]{ 0 };
		size_t pos = 0;
		char count = 0;
		for (auto &it : str) {
			if (it == '"') {
				count++;
			}
			if (it == _char && count != 1) {
				continue;
			}
			if (count == 2)count = 0;
			bufStr[pos] = it;
			pos++;
		}
		str = bufStr;
		delete bufStr;
	}
	inline void Erase(EString&str, TCHAR _char = ' ') {
		TCHAR *bufStr = new TCHAR[str.size()]{ 0 };
		size_t pos = 0;
		for (auto &it : str) {
			if (_char == it)continue;
			bufStr[pos] = it;
			pos++;
		}
		str = bufStr;
		delete bufStr;
	}
	inline Color StringToColor(const EString&str) {

#ifdef UNICODE
		auto color = String::Split(String::UnicodeToANSI(str), ",");

#else
		auto color = String::Split(str, ",");

#endif

		BYTE A = 255;
		BYTE R = 255;
		BYTE G = 255;
		BYTE B = 255;
		if (color.size() == 3) {
			R = std::stoi(color.at(0));
			G = std::stoi(color.at(1));
			B = std::stoi(color.at(2));
		}
		if (color.size() == 4) {
			A = std::stoi(color.at(0));
			R = std::stoi(color.at(1));
			G = std::stoi(color.at(2));
			B = std::stoi(color.at(3));
		}
		return Color(A, R, G, B);
	}
}
