#pragma once
#include "EzUI.h"
#include "Control.h"

namespace UIManager {
	extern std::map<EString, EString> styles;//默认样式集合
	extern std::map<EString, EString> styles_active;//按下样式集合
	extern  std::map<EString, EString> styles_hover;//鼠标悬浮样式集合
	std::vector<Control*> UI_EXPORT LoadControl(const EString& filename);

	inline size_t  Replace(std::string& str, const std::string& oldText, const std::string& newText)
	{
		std::string& newStr = str;
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
	inline Color StringToColor(const EString& str) {

		auto color = EString::Split(str.c_str(), TEXT(","));

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
