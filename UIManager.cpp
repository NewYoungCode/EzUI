#include "UIManager.h"
#include "HLayout.h"
#include "Label.h"
#include "Form.h"
#include "VLayout.h"
#include "TileLayout.h"
#include "Button.h"
#include "VList.h"
#include "HList.h"
#include "RadioButton.h"
#include "CheckBox.h"
#include "Edit.h"
#include "TabLayout.h"

#include"tinyxml/tinyxml.h"
#include"tinyxml/tinystr.h"

namespace UIManager {
	const EString Attribute(TiXmlElement* node, const char* szstr);
	void LoadControl(TiXmlElement* node, Control* control);
	Control* BuildControl(TiXmlElement* node);
	void LoadStyle(Control* ctl, ControlState styleType);

	std::map<EString, EString> styles;//默认样式集合
	std::map<EString, EString> styles_active;//按下样式集合
	std::map<EString, EString> styles_hover;//鼠标悬浮样式集合
}

namespace UIManager {
	const EString Attribute(TiXmlElement* node, const char* szstr) {
		auto str = node->Attribute(szstr);
		if (str == NULL) return L"";
		return str;
	}
	Control* BuildControl(TiXmlElement* node) {
		Control* ctl = NULL;
		std::string valueStr(node->ValueTStr().c_str());
		for (size_t i = 0; i < valueStr.size(); i++)
		{
			char& ch = (char&)valueStr.c_str()[i];
			if (ch >= 65 && ch <= 90) {
				ch += 32;
			}
		}
		do
		{
			if (valueStr == "control") {
				ctl = new Control();
				break;
			}
			if (valueStr == "vlist") {
				ctl = new VList;
				break;
			}
			if (valueStr == "hlist") {
				ctl = new HList;
				break;
			}
			if (valueStr == "vlayout") {
				ctl = new VLayout;
				break;
			}
			if (valueStr == "hlayout") {
				ctl = new HLayout;
				break;
			}
			if (valueStr == "layout") {
				ctl = new Layout;
				break;
			}
			if (valueStr == "tilelayout") {
				ctl = new TileLayout;
				break;
			}
			if (valueStr == "tablayout") {
				ctl = new TabLayout;
				break;
			}
			if (valueStr == "container") {
				ctl = new Container;
				break;
			}
			if (valueStr == "spacer") {
				ctl = new Spacer();
				break;
			}
			if (valueStr == "vspacer") {
				ctl = new VSpacer(0);
				break;
			}
			if (valueStr == "hspacer") {
				ctl = new HSpacer(0);
				break;
			}
			if (valueStr == "label") {
				ctl = new Label;
				((Label*)ctl)->SetText(Attribute(node, "text"));

				EString valign = Attribute(node, "valign");//垂直
				EString halign = Attribute(node, "halign");//水平

				if (!valign.empty() || !halign.empty()) {
					int v = 0;
					int h = 0;
					if (valign == "top") {
						v = (int)Align::Top;
					}else if (valign == "bottom") {
						v = (int)Align::Bottom;
					}
					else {
						v = (int)Align::Mid;
					}

					if (halign == "left") {
						h = (int)Align::Left;
					}
					else if (halign == "right") {
						h = (int)Align::Right;
					}
					else {
						h = (int)Align::Center;
					}


					if (v && h) {
						((Label*)ctl)->SetTextAlign((TextAlign)(v | h));
					}
					else if (v && !h) {
						((Label*)ctl)->SetTextAlign((TextAlign)(v));
					}
					else  if (!v && h) {
						((Label*)ctl)->SetTextAlign((TextAlign)(h));
					}
				}
				break;
			}
			if (valueStr == "button") {
				ctl = new Button;
				((Button*)ctl)->SetText(Attribute(node, "text"));
				break;
			}
			if (valueStr == "radiobutton") {
				ctl = new RadioButton;
				((RadioButton*)ctl)->SetText(Attribute(node, "text"));
				break;
			}
			if (valueStr == "checkbox") {
				ctl = new CheckBox;
				((CheckBox*)ctl)->SetText(Attribute(node, "text"));
				break;
			}
			if (valueStr == "edit") {
				ctl = new EditWnd;
				((EditWnd*)ctl)->SetText(Attribute(node, "text"));
				break;
			}
		} while (false);

		TiXmlAttribute* attr = node->FirstAttribute();
		do
		{
			if (!attr)break;
			ctl->SetAttribute(attr->Name(), attr->Value());
		} while ((attr = attr->Next()));

		if (ctl) {
			ctl->IsXmlControl = true;//标记为xml加载进来的控件
		}
		LoadStyle(ctl, ControlState::None);
		LoadStyle(ctl, ControlState::Active);
		LoadStyle(ctl, ControlState::Hover);
		return ctl;
	}
	void LoadControl(TiXmlElement* node, Control* control) {
		TiXmlElement* fristChild = NULL;
		if ((fristChild = node->FirstChildElement()))//先寻找子控件
		{
			//Debug::Log("node %s", fristChild->Value());
			Control* ctl = BuildControl(fristChild);
			LoadControl(fristChild, ctl);
			control->AddControl(ctl);
			TiXmlElement* nextChild = fristChild->NextSiblingElement();
			while (nextChild)//然后寻找兄弟
			{
				//Debug::Log("node %s", nextChild->Value());
				Control* ctl2 = BuildControl(nextChild);
				LoadControl(nextChild, ctl2);
				control->AddControl(ctl2);
				nextChild = nextChild->NextSiblingElement();
			}
		}
	}
	//去除空格或者其他符号 双引号内的空格不会去除
	void TrimStyle(EString& str, TCHAR _char = ' ') {
		TCHAR* bufStr = new TCHAR[str.size() + 1]{ 0 };
		size_t pos = 0;
		char count = 0;
		for (auto& it : str) {
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
	void AnalysisStyle(const EString& styleStr) {
		styles.clear();
		styles_active.clear();
		styles_hover.clear();

		EString style = styleStr;
		TrimStyle(style);
		while (style.size() > 0) {
			size_t pos = style.find("#");
			if (pos == -1)break;
			size_t pos2 = style.find("}");
			if (pos2 == -1)break;
			size_t pos3 = style.find("{");
			EString name = style.substr(pos + 1, pos3 - pos - 1);
			size_t pos4 = name.find(":");
			EString style_type;
			EString str = style.substr(pos3 + 1, pos2 - pos3 - 1);
			if (pos4 != size_t(-1)) {
				style_type = name.substr(pos4 + 1);
				name = name.substr(0, pos4);
			}
			if (style_type == "hover") {
				styles_hover.insert(std::pair<EString, EString>(name, str));
			}
			else if (style_type == "active") {
				styles_active.insert(std::pair<EString, EString>(name, str));
			}
			else {
				styles.insert(std::pair<EString, EString>(name, str));
			}
			style = style.substr(pos2 + 1);
		}
	}
	std::vector<Control*> LoadControl(const EString& filename)
	{
		TiXmlDocument doc;
		FILE* file(0);
		_wfopen_s(&file, filename.utf16().c_str(), L"rb+");
		if (!doc.LoadFile(file, TiXmlEncoding::TIXML_ENCODING_UTF8)) {//the file code page must utf8
			ASSERT(0);
		}
		std::vector<Control*> controls;
		TiXmlElement* element = doc.FirstChildElement();//read frist element
		do
		{
			if (element == NULL) break;
			if (element->ValueTStr() == "style") {// if element is style
				AnalysisStyle(element->GetText());//
			}
			else { //if no style , must be Control
				Control* control = BuildControl(element);//先加载根节点
				LoadControl(element, control);//加载子节点
				if (control) controls.push_back(control);
			}
		} while ((element = element->NextSiblingElement()));

		::fclose(file);
		return controls;
	}
	void LoadStyle(Control* ctl, ControlState styleState) {
		ControlStyle* style = NULL;
		std::map<EString, EString>::iterator styleStr;
		std::map<EString, EString>* _styles = NULL;
		if (styleState == ControlState::Active) {//按下样式
			style = &ctl->ActiveStyle;
			_styles = &styles_active;
			styleStr = styles_active.find(ctl->Name);
		}
		else if (styleState == ControlState::Hover) {//悬浮样式
			style = &ctl->HoverStyle;
			_styles = &styles_hover;
			styleStr = styles_hover.find(ctl->Name);
		}
		else {//默认样式
			style = &ctl->Style;
			_styles = &styles;
			styleStr = styles.find(ctl->Name);
		}
		if (styleStr != _styles->end() && styleStr->second.size() > 0) {
			style->SetStyleSheet(styleStr->second);
		}
	}
}