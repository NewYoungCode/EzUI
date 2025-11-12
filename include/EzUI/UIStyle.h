#pragma once
#include "EzUI.h"
namespace ezui {
	namespace detail {
		
		extern UI_EXPORT void AnalysisStyle(const UIString& styleStr, std::list<Style>* out);//分析样式
		extern UI_EXPORT int MathStyle(Control* ctl, const UIString& selectorName, const std::list<Style>& selectors);
		extern UI_EXPORT void ApplyStyle(Control* ctl, const UIString& selectName, const std::list<Style>& selectors);
		extern UI_EXPORT void ApplyStyle(Control* ctl, const std::list<Style>& selectors);

		//数值是否有效(是否启用)
		template<typename T>
		inline bool __IsValid(const T& value) {
			return value.IsEnabled();
		}
		//是否是像素单位
		inline bool __IsPx(const UIString& num, float& outNum) {
			size_t pos = num.find("px");
			if (pos != 0 && pos == num.size() - 2) {
				outNum = std::stof(num.substr(0, pos).c_str());
				return true;
			}
			return false;
		}
		//字符串转数值
		inline float __ToFloat(const UIString& numStr) {
			float value;
			if (__IsPx(numStr, value)) {//以px为单位
				return value;
			}
			value = std::atof(numStr.c_str());
			if (numStr.rfind("%") != std::string::npos) {//判断是不是百分比
				value = value / 100.0f;//如果是百分比则返回小数点(0.0f~1.0f)
			}
			return value;//直接转数值返回
		}
		//处理url(d:/imgs/aa.png)路径
		inline Image* __MakeImage(UIString value) {
			value = value.replace("\"", "");//删除双引号;
			auto pos1 = value.find("(");
			auto pos2 = value.find(")");
			if (pos1 != std::string::npos && pos2 != std::string::npos) {
				//background-image:url(res/images/xxx.png)的方式
				value = value.substr(pos1 + 1, pos2 - pos1 - 1);
			}
			return Image::Make(value);
		}
		//构建边框信息并回调
		inline void __MakeBorder(const UIString& key, UIString str, Border& border, float scale = 1.0f) {
			if (!key.empty()) {
				if (key == "border-width") {
					border = __ToFloat(str) * scale + 0.5;
					return;
				}
				if (key == "border-color") {
					border.Color = Color::Make(str);
					return;
				}
				if (key == "border-style") {
					if (str == "solid") {
						border.Style = StrokeStyle::Solid;
					}
					else if (str == "dashed") {
						border.Style = StrokeStyle::Dash;
					}
					return;
				}
				if (key == "border-radius") {
					border.Radius = __ToFloat(str) * scale + 0.5;
					return;
				}
				if (key == "border-top-left-radius") {
					border.TopLeftRadius = __ToFloat(str) * scale + 0.5;
					return;
				}
				if (key == "border-top-right-radius") {
					border.TopRightRadius = __ToFloat(str) * scale + 0.5;
					return;
				}
				if (key == "border-bottom-right-radius") {
					border.BottomRightRadius = __ToFloat(str) * scale + 0.5;
					return;
				}
				if (key == "border-bottom-left-radius") {
					border.BottomLeftRadius = __ToFloat(str) * scale + 0.5;
					return;
				}
			}
			size_t pos1 = str.find('(');
			size_t pos2 = str.find(')');
			if (pos1 != std::string::npos && pos2 != std::string::npos && pos2 > pos1) {
				//删除括号内的空格
				std::string inside = str.substr(pos1 + 1, pos2 - pos1 - 1);
				inside.erase(std::remove(inside.begin(), inside.end(), ' '), inside.end());
				str = str.substr(0, pos1 + 1) + inside + str.substr(pos2);
			}
			auto values = str.split(" ");
			for (auto& v : values) {
				float num;
				if (v == "dashed") {
					border.Style = StrokeStyle::Dash;
					continue;
				}
				else if (v == "solid") {
					border.Style = StrokeStyle::Solid;
					continue;
				}
				else if (v == "none") {
					border.Style = StrokeStyle::None;
					continue;
				}
				else if (__IsPx(v, num)) {
					if (key == "border") {
						border.Left = num * scale + 0.5;
						border.Top = num * scale + 0.5;
						border.Right = num * scale + 0.5;
						border.Bottom = num * scale + 0.5;
					}
					else if (key == "border-left") {
						border.Left = num * scale + 0.5;
					}
					else if (key == "border-top") {
						border.Top = num * scale + 0.5;
					}
					else if (key == "border-right") {
						border.Right = num * scale + 0.5;
					}
					else if (key == "border-bottom") {
						border.Bottom = num * scale + 0.5;
					}
					continue;
				}
				else
				{
					bool isGood = false;
					Color color = Color::Make(v, &isGood);
					if (isGood) {
						border.Color = color;
					}
				}
			}
		}
	};
};