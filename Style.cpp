#include "Style.h"

namespace Style {
	void Erase(EString& str, TCHAR _char) {
		TCHAR* bufStr = new TCHAR[str.size()]{ 0 };
		size_t pos = 0;
		for (auto& it : str) {
			if (_char == it)continue;
			bufStr[pos] = it;
			pos++;
		}
		str = bufStr;
		delete bufStr;
	}
	
	Rect StringToRect(const EString& str) {
		auto rectStr = EString::Split(str.c_str(), L",");
		if (str.empty()) return Rect();//如果没写矩形区域
		Rect rect;
		rect.X = std::stoi(rectStr.at(0));
		rect.Y = std::stoi(rectStr.at(1));
		rect.Width = std::stoi(rectStr.at(2));
		rect.Height = std::stoi(rectStr.at(3));
		return rect;
	}
};
