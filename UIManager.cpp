#include "UIManager.h"
#include "HLayout.h"
#include "Lable.h"
#include "Form.h"
#include "VLayout.h"
#include "TileLayout.h"
#include "Button.h"
#include "VList.h"
#include "HList.h"
#include "RadioButton.h"
#include "CheckBox.h"

#include"tinyxml/tinyxml.h"
#include"tinyxml/tinystr.h"

namespace UIManager {
	const EString Attribute(TiXmlElement* node, const char* szstr);
	void LoadControl(TiXmlElement* node, Control* control);
	Rect StringToRect(const EString& str);
	Control* BuildControl(TiXmlElement* node);
	void LoadStyle(Control* ctl, ControlState styleType);

	std::map<EString, EString> styles;//默认样式集合
	std::map<EString, EString> styles_active;//按下样式集合
	std::map<EString, EString> styles_hover;//鼠标悬浮样式集合
}

namespace UIManager {

	Rect StringToRect(const EString& str) {
		auto rectStr = String::Split(str.c_str(), L",");
		if (str.empty()) return Rect();//如果没写矩形区域
		Rect rect;
		rect.X = std::stoi(rectStr.at(0));
		rect.Y = std::stoi(rectStr.at(1));
		rect.Width = std::stoi(rectStr.at(2));
		rect.Height = std::stoi(rectStr.at(3));
		return rect;
	}
	const EString Attribute(TiXmlElement* node, const char* szstr) {
		auto str = node->Attribute(szstr);
		if (str == NULL) return L"";
		return str;
	}
	Control* BuildControl(TiXmlElement* node) {
		Control* ctl = NULL;
		do
		{
			if (node->ValueTStr() == "Control") {
				ctl = new Control();
				break;
			}
			if (node->ValueTStr() == "VList") {
				ctl = new VList;
				break;
			}
			if (node->ValueTStr() == "HList") {
				ctl = new HList;
				break;
			}
			if (node->ValueTStr() == "VLayout") {
				ctl = new VLayout;
				break;
			}
			if (node->ValueTStr() == "HLayout") {
				ctl = new HLayout;
				break;
			}
			if (node->ValueTStr() == "Layout") {
				ctl = new Layout;
				break;
			}
			if (node->ValueTStr() == "TileLayout") {
				ctl = new TileLayout;
				break;
			}
			if (node->ValueTStr() == "Container") {
				ctl = new Container;
				break;
			}
			if (node->ValueTStr() == "Spacer") {
				ctl = new Spacer();
				break;
			}
			if (node->ValueTStr() == "VSpacer") {
				ctl = new VSpacer(0);
				break;
			}
			if (node->ValueTStr() == "HSpacer") {
				ctl = new HSpacer(0);
				break;
			}
			if (node->ValueTStr() == "Lable") {
				ctl = new Lable;
				((Lable*)ctl)->SetText(Attribute(node, "text"));
				break;
			}
			if (node->ValueTStr() == "Button") {
				ctl = new Button;
				((Button*)ctl)->SetText(Attribute(node, "text"));
				break;
			}
			if (node->ValueTStr() == "RadioButton") {
				ctl = new RadioButton;
				((RadioButton*)ctl)->SetText(Attribute(node, "text"));
				break;
			}
			if (node->ValueTStr() == "CheckBox") {
				ctl = new CheckBox;
				((CheckBox*)ctl)->SetText(Attribute(node, "text"));
				break;
			}
		} while (false);
		//通用属性
		ctl->Name = Attribute(node, "name");
		ctl->SetRect(StringToRect(Attribute(node, "rect")));

		if (node->Attribute("width")) {//如果单独设置了宽高那就是绝对宽高了
			ctl->SetFixedWidth(std::stoi(node->Attribute("width")));
		}
		if (node->Attribute("height")) {//如果单独设置了宽高那就是绝对宽高了
			ctl->SetFixedHeight(std::stoi(node->Attribute("height")));
		}
		if (node->Attribute("visible")) {//控件是否可见
			ctl->Visible = (node->Attribute("visible") == "true" ? true : false);
		}
		if (node->Attribute("dock")) {//控件对其方式
			EString valueStr = node->Attribute("dock");
			do
			{
				if (valueStr == "fill") {
					ctl->Dock = DockStyle::Fill; break;
				}
				if (valueStr == "horizontal") {
					ctl->Dock = DockStyle::Horizontal; break;
				}
				if (valueStr == "vertical") {
					ctl->Dock = DockStyle::Vertical; break;
				}
			} while (0);
		}
		if (node->Attribute("action")) {//控件行为
			EString valueStr = node->Attribute("action");
			do
			{
				if (valueStr == "close") {
					ctl->Action = ControlAction::Close; break;
				}
				if (valueStr == "mini") {
					ctl->Action = ControlAction::Mini; break;
				}
				if (valueStr == "max") {
					ctl->Action = ControlAction::Max; break;
				}
				if (valueStr == "move" || valueStr == "movewindow") {
					ctl->Action = ControlAction::MoveWindow; break;
				}
			} while (0);
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
	void Erase(EString& str, TCHAR _char = ' ') {
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
	void AnalysisStyle(const EString& styleStr) {
		EString style;
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
			auto attrs = String::Split(styleStr->second.c_str(), L";");
			for (auto& it : attrs) {
				size_t pos = it.find(":");
				if (pos == -1)continue;
				EString key = it.substr(0, pos);
				EString value = it.substr(pos + 1);
				do
				{
					if (key == "background-color") {
						style->BackgroundColor = StringToColor(value);
						break;
					}
					if (key == "background-image") {
						style->BackgroundImage = new Image(value);
						break;
					}
					if (key == "fore-image") {
						style->BackgroundImage = new Image(value);
						break;
					}
					if (key == "border-color") {
						style->BorderColor = StringToColor(value);
						break;
					}
					if (key == "color" || key == "fore-color") {
						style->ForeColor = StringToColor(value);
						break;
					}
					if (key == "radius") {
						style->Radius = std::stoi(value);
						break;
					}
					if (key == "font-size") {
						style->FontSize = std::stoi(value);
						break;
					}
					if (key == "font-family") {
						Erase(value, '"');//删除双引号;
						style->FontFamily = value;
						break;
					}
					if (key == "border") {
						auto width = std::stoi(value);
						style->BorderLeft = width;
						style->BorderTop = width;
						style->BorderRight = width;
						style->BorderBottom = width;
						break;
					}
					if (key == "border-left") {
						style->BorderLeft = std::stoi(value);
						break;
					}
					if (key == "border-top") {
						style->BorderTop = std::stoi(value);
						break;
					}
					if (key == "border-right") {
						style->BorderRight = std::stoi(value);
						break;
					}
					if (key == "border-bottom") {
						style->BorderBottom = std::stoi(value);
						break;
					}
				} while (false);
			}
		}
	}
}