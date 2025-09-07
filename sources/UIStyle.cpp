#include "UIStyle.h"
#include "ScrollBar.h"
#include "Control.h"

namespace ezui {
	void AnalysisStyle(const UIString& styleStr, std::list<Style>* out) {
		UIString style = styleStr.trim();
		while (true)
		{
			//处理css的注释
			auto pos1 = style.find("/*");
			auto pos2 = style.find("*/", pos1 + 2);
			if (pos1 != std::string::npos && pos2 != std::string::npos) {
				style.erase(pos1, pos2 - pos1 + 2);
			}
			else {
				break;
			}
		}
		//分离每个样式
		std::list<UIString> strs;
		while (true)
		{
			auto pos1 = style.find("}");
			if (pos1 != std::string::npos) {
				strs.push_back(style.substr(0, pos1 + 1));
				style.erase(0, pos1 + 1);
			}
			else {
				break;
			}
		}
		//解析样式类型
		for (auto& style : strs) {
			size_t pos2 = style.find("}");
			if (pos2 == -1)break;
			size_t pos3 = style.find("{");
			UIString name = style.substr(0, pos3);
			size_t pos4 = name.find(":");
			UIString style_type;
			UIString str = style.substr(pos3 + 1, pos2 - pos3 - 1);
			if (pos4 != std::string::npos) {
				style_type = name.substr(pos4 + 1);
				style_type = style_type.trim();
			}
			//考虑到多个选择器
			auto names = name.split(",");
			for (auto& name : names) {
				//添加至集合
				Style selector;
				selector.m_selectorName = name.trim();
				selector.m_styleStr = str.trim();

				if (style_type == "hover") {
					selector.m_styleType = ControlState::Hover;
				}
				else if (style_type == "active") {
					selector.m_styleType = ControlState::Active;
				}
				else if (style_type == "checked") {
					selector.m_styleType = ControlState::Checked;
				}
				else {
					selector.m_styleType = ControlState::Static;
				}
				out->push_back(selector);
			}
		}
	}

	int MathStyle(Control* ctl, const UIString& selectorName, const std::list<Style>& selectors)
	{
		int mathCount = 0;
		for (auto& it : selectors) {
			if (it.m_selectorName == selectorName) {
				++mathCount;
				auto& styleType = it.m_styleType;
				auto& styleStr = it.m_styleStr;
				ctl->SetStyleSheet(styleType, styleStr);
			}
		}
		//返回匹配成功的个数
		return mathCount;
	}

	void ApplyStyle(Control* ctl, const  UIString& selectName, const std::list<Style>& selectors)
	{
		MathStyle(ctl, selectName, selectors);
		MathStyle(ctl, selectName + ":checked", selectors);
		MathStyle(ctl, selectName + ":active", selectors);
		MathStyle(ctl, selectName + ":hover", selectors);
		MathStyle(ctl, selectName + ":disabled", selectors);

		//是否有滚动条 有滚动条则应用滚动条样式
		ScrollBar* scrollBar = ctl->GetScrollBar();
		if (scrollBar) {
			UIString scrollBarSelectName = UIString("%s::-webkit-scrollbar").format(selectName.c_str());
			MathStyle(scrollBar, scrollBarSelectName, selectors);
			MathStyle(scrollBar, scrollBarSelectName + ":active", selectors);
			MathStyle(scrollBar, scrollBarSelectName + ":hover", selectors);
			MathStyle(scrollBar, scrollBarSelectName + ":disabled", selectors);
		}
	}

	void ApplyStyle(Control* ctl, const std::list<Style>& selectors) {
		{//加载样式 使用类选择器  
			UIString classStr = ctl->GetAttribute("class");
			ui_text::Replace(&classStr, " ", ",");//去除所有空格转换为逗号
			auto classes = classStr.split(",");//分割类选择器
			for (const auto& className : classes) { //一个控件可能有多个类名
				UIString selectName = UIString(".%s").format(className.c_str());
				ezui::ApplyStyle(ctl, selectName, selectors);
			}
		}
		//加载样式 使用ID/name选择器 
		if (!(ctl->Name.empty())) {
			UIString selectName = UIString("#%s").format(ctl->Name.c_str());
			ezui::ApplyStyle(ctl, selectName, selectors);
		}

		//由于框架大改 暂时弃用
		//{//加载内联样式 优先级最高
		//	UIString sytle_static = ctl->GetAttribute("style");//内联样式语法
		//	if (!sytle_static.empty()) { //内联样式只允许描述静态效果
		//		ctl->SetStyleSheet(ControlState::Static, sytle_static);
		//	}
		//}
	}
};