#include "UIManager.h"

#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"

namespace EzUI {
	//去除空格或者其他符号 双引号内的空格不会去除
	void TrimStyle(EString& str, CHAR _char = ' ') {
		CHAR* bufStr = new CHAR[str.size() + 1]{ 0 };
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
	EString Attribute(TiXmlElement* node, const char* szstr) {
		auto str = node->Attribute(szstr);
		if (str == NULL) return "";
		return str;
	}
};

namespace EzUI {

	Control* UIManager::BuildControl(void* _node) {
		TiXmlElement* node = (TiXmlElement*)_node;
		Control* ctl = NULL;
		std::string tagStr(node->ValueTStr().c_str());
		EString::Tolower(&tagStr);
		do
		{
			if (tagStr == "control") {
				ctl = new Control();
				break;
			}
			if (tagStr == "vlist") {
				ctl = new VList;
				break;
			}
			if (tagStr == "hlist") {
				ctl = new HList;
				break;
			}
			if (tagStr == "vlayout" || tagStr == "vbox") {
				ctl = new VLayout;
				break;
			}
			if (tagStr == "hlayout" || tagStr == "hbox") {
				ctl = new HLayout;
				break;
			}
			if (tagStr == "layout" || tagStr == "box") {
				ctl = new Control;
				break;
			}
			if (tagStr == "tilelist") {
				ctl = new TileList;
				break;
			}
			if (tagStr == "tablayout") {
				ctl = new TabLayout;
				break;
			}
			if (tagStr == "spacer") {
				ctl = new Spacer();
				break;
			}
			if (tagStr == "vspacer") {
				ctl = new VSpacer(0);
				break;
			}
			if (tagStr == "hspacer") {
				ctl = new HSpacer(0);
				break;
			}
			if (tagStr == "label") {
				ctl = new Label;
				break;
			}
			if (tagStr == "button") {
				ctl = new Button;
				break;
			}
			if (tagStr == "radiobutton") {
				ctl = new RadioButton;
				break;
			}
			if (tagStr == "checkbox") {
				ctl = new CheckBox;
				break;
			}
			if (tagStr == "edit" || tagStr == "textbox") {
				ctl = new TextBox;
				break;
			}
			if (tagStr == "img" || tagStr == "pictureBox") {
				ctl = new PictureBox;
				break;
			}
			if (ctl == NULL) {
				ctl = this->OnBuildControl(tagStr);
			}
			if (ctl == NULL) {
				::MessageBoxA(NULL, EString("UnKnow Element " + tagStr).c_str(), "UIManager Erro", NULL);
				ctl = new Control;
			}
		} while (false);

		freeControls.push_front(ctl);

		//内联样式
		EString inlineStyle;
		EString inlineStyle_active;
		EString inlineStyle_hover;

		TiXmlAttribute* attr = node->FirstAttribute();
		do
		{
			if (!attr)break;
			auto attrName = attr->Name();
			EString attrValue(attr->Value());

			if (std::strcmp(attrName, "style") == 0) {
				TrimStyle(attrValue);
				ctl->SetAttribute(attrName, attrValue);
				continue;
			}
			if (std::strcmp(attrName, "hover") == 0) {
				TrimStyle(attrValue);
				ctl->SetAttribute(attrName, attrValue);
				continue;
			}
			if (std::strcmp(attrName, "active") == 0) {
				TrimStyle(attrValue);
				ctl->SetAttribute(attrName, attrValue);
				continue;
			}
			if (std::strcmp(attrName, "checked") == 0) {
				TrimStyle(attrValue);
				ctl->SetAttribute(attrName, attrValue);
				continue;
			}
			ctl->SetAttribute(attr->Name(), attrValue);
			this->OnSetAttribute(ctl, attrName, attrValue);
		} while ((attr = attr->Next()));


		{//加载样式 使用标签选择器
			LoadStyle(ctl, tagStr);
			LoadStyle(ctl, tagStr + ":checked");
			LoadStyle(ctl, tagStr + ":active");
			LoadStyle(ctl, tagStr + ":hover");
		}
		{//加载样式 使用类选择器  
			EString _class = ctl->GetAttribute("class");
			EString::Replace(&_class, "  ", " ");
			auto classs = _class.Split(" ");
			for (auto& className : classs) {
				LoadStyle(ctl, "." + className);
				LoadStyle(ctl, "." + className + ":checked");
				LoadStyle(ctl, "." + className + ":active");
				LoadStyle(ctl, "." + className + ":hover");
			}
		}
		if (!(ctl->Name.empty())) {//加载样式 使用ID选择器 
			LoadStyle(ctl, "#" + ctl->Name);
			LoadStyle(ctl, "#" + ctl->Name + ":checked");
			LoadStyle(ctl, "#" + ctl->Name + ":active");
			LoadStyle(ctl, "#" + ctl->Name + ":hover");
		}

		{//内联样式
			EString sytle_static = ctl->GetAttribute("style");//内联样式语法
			EString style_hover = ctl->GetAttribute("style:hover");//内联样式语法
			EString style_active = ctl->GetAttribute("style:active");//内联样式语法
			EString style_checked = ctl->GetAttribute("style:checked");//内联样式语法

			if (!sytle_static.empty()) {
				ctl->Style.SetStyleSheet(sytle_static, BuildImageCallback);
			}
			if (!style_hover.empty()) {
				ctl->HoverStyle.SetStyleSheet(style_hover, BuildImageCallback);
			}
			if (!style_active.empty()) {
				ctl->ActiveStyle.SetStyleSheet(style_active, BuildImageCallback);
			}
			if (!style_checked.empty()) {
				ISelect* ckBox = dynamic_cast<ISelect*>(ctl);
				if (ckBox) {
					ckBox->CheckedStyle.SetStyleSheet(style_checked, BuildImageCallback);
				}
			}
		}
		return ctl;
	}
	void UIManager::LoadControl(void* _node, Control* control) {
		TiXmlElement* node = (TiXmlElement*)_node;

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

	void UIManager::LoadFromRaw(const char* xmlRaw)
	{
		TiXmlDocument doc;
		auto result = doc.Parse(xmlRaw, NULL, TiXmlEncoding::TIXML_ENCODING_UTF8);
		//doc.Parse
		TiXmlElement* element = doc.FirstChildElement();//read frist element

		std::list<TiXmlElement*> controlNodes;
		//先处理样式
		do
		{
			if (element == NULL) break;
			if (element->ValueTStr() == "style") {// if element is style
				AnalysisStyle(element->GetText());//
			}
			else { //if no style , must be Control
				controlNodes.push_back(element);
			}
		} while ((element = element->NextSiblingElement()));
		//然后处理控件
		for (auto& element : controlNodes) {
			Control* control = BuildControl(element);//先加载根节点
			LoadControl(element, control);//加载子节点
			if (control) controls.push_back(control);
		}
	}

	void UIManager::AnalysisStyle(const EString& styleStr) {
		EString style = styleStr;
		//处理空格 双引号内的空格不处理
		TrimStyle(style);
		while (true)
		{
			//处理css的注释
			auto pos1 = style.find("/*");
			auto pos2 = style.find("*/", pos1 + 2);
			if (pos1 != size_t(-1) && pos2 != size_t(-1)) {
				style.erase(pos1, pos2 - pos1 + 2);
			}
			else {
				break;
			}
		}
		//分离每个样式
		std::list<EString> strs;
		while (true)
		{
			auto pos1 = style.find("}");
			if (pos1 != size_t(-1)) {
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
			EString name = style.substr(0, pos3);
			size_t pos4 = name.find(":");
			EString style_type;
			EString str = style.substr(pos3 + 1, pos2 - pos3 - 1);
			if (pos4 != size_t(-1)) {
				style_type = name.substr(pos4 + 1);
			}
			//考虑到多个选择器
			auto names = name.Split(",");
			for (auto& name : names) {
				//添加至集合
				UIManager::Selector selector;
				selector.selectorName = name;
				selector.styleStr = str;
				if (style_type == "hover") {
					selector.styleType = UIManager::Style::Hover;
				}
				else if (style_type == "active") {
					selector.styleType = UIManager::Style::Active;
				}
				else if (style_type == "checked") {
					selector.styleType = UIManager::Style::Checked;
				}
				else {
					selector.styleType = UIManager::Style::Static;
				}
				Selectors.push_back(selector);
			}
		}
	}

	void UIManager::LoadStyle(Control* ctl, const EString& selectorName)
	{
		for (auto& it : Selectors) {
			if (it.selectorName == selectorName) {
				if (it.styleType == UIManager::Style::Static) {
					ctl->Style.SetStyleSheet(it.styleStr, BuildImageCallback);
				}
				if (it.styleType == UIManager::Style::Hover) {
					ctl->HoverStyle.SetStyleSheet(it.styleStr, BuildImageCallback);
				}
				if (it.styleType == UIManager::Style::Active) {
					ctl->ActiveStyle.SetStyleSheet(it.styleStr, BuildImageCallback);
				}
				if (it.styleType == UIManager::Style::Checked) {
					ISelect* ckBox = dynamic_cast<ISelect*>(ctl);
					if (ckBox) {
						ckBox->CheckedStyle.SetStyleSheet(it.styleStr, BuildImageCallback);
					}
				}
			}
		}
	}

	Control* UIManager::OnBuildControl(const EString& nodeName) {
		if (EventBuilControl) {
			return EventBuilControl(nodeName);
		}
		return NULL;
	}
	void UIManager::OnSetAttribute(Control* ctl, const EString& attrName, const EString& attrValue) {
		if (EventSetAttribute) {
			EventSetAttribute(ctl, attrName, attrValue);
		}
	}
	UIManager::UIManager()
	{
		BuildImageCallback = [this](Image* img)->void {
			this->freeImages.push_back((Image*)img);
		};
	}
	void UIManager::SetupUI(Window* window)
	{
		window->SetLayout(GetNode(0));
	}

	void UIManager::LoadFile(const EString& fileName) {
		std::string data;
		GetResource(fileName, &data);
		LoadFromRaw((const char*)data.c_str());
	}
	void UIManager::Free(Control** ctl)
	{
		auto itor = std::find(freeControls.begin(), freeControls.end(), *ctl);
		if (itor != freeControls.end()) {
			delete* itor;
			freeControls.erase(itor);
		}
		else
		{
			delete* ctl;
		}
		*ctl = NULL;
	}
	void UIManager::Free(Image** img)
	{
		auto itor = std::find(freeImages.begin(), freeImages.end(), *img);
		if (itor != freeImages.end()) {
			delete* itor;
			freeImages.erase(itor);
		}
		else
		{
			delete* img;
		}
		*img = NULL;
	}
	UIManager::~UIManager() {
		for (auto& it : freeControls) {
			delete it;
		}
		for (auto& it : freeImages) {
			delete it;
		}
	}
	Control* UIManager::GetNodeByName(const EString& nodeName) {
		for (auto& it : controls) {
			if (it->Name == nodeName) {
				return it;
			}
		}
		return NULL;
	}
	Control* UIManager::GetNode(size_t pos) {
		size_t _pos = 0;
		for (auto& it : controls) {
			if (pos == _pos) {
				return it;
			}
			_pos++;
		}
		return NULL;
	}

	_Selector::_Selector(const Controls& Controls)
	{
		this->ctls = Controls;
	}
	_Selector::_Selector(Control* ctl)
	{
		this->ctl = ctl;
	}
	_Selector& _Selector::Css(const EString& styleStr)
	{
		for (auto& it : this->ctls) {
			if (notCtl == it)continue;
			it->SetStyleSheet(styleStr);
		}
		if (ctl) {
			if (notCtl == ctl)return *this;
			ctl->SetStyleSheet(styleStr);
		}
		return *this;
	}
	_Selector& _Selector::CssHover(const EString& styleStr)
	{
		for (auto& it : this->ctls) {
			if (notCtl == it)continue;
			it->SetStyleSheet(styleStr, ControlState::Hover);
		}
		if (ctl) {
			if (notCtl == ctl)return *this;
			ctl->SetStyleSheet(styleStr, ControlState::Hover);
		}
		return *this;
	}
	_Selector& _Selector::CssActive(const EString& styleStr)
	{
		for (auto& it : this->ctls) {
			if (notCtl == it)continue;
			it->SetStyleSheet(styleStr, ControlState::Active);
		}
		if (ctl) {
			if (notCtl == ctl)return *this;
			ctl->SetStyleSheet(styleStr, ControlState::Active);
		}
		return *this;
	}
	_Selector& _Selector::Attr(const EString& key, const EString& value)
	{
		for (auto& it : this->ctls) {
			if (notCtl == it)continue;
			it->SetAttribute(key, value);
		}
		if (ctl) {
			if (notCtl == ctl)return *this;
			ctl->SetAttribute(key, value);
		}
		return *this;
	}
	_Selector& _Selector::Refresh()
	{
		for (auto& it : this->ctls) {
			it->Invalidate();
		}
		if (ctl) {
			ctl->Invalidate();
		}
		return *this;
	}
	_Selector& _Selector::Not(Control* fiterCtl) {

		this->notCtl = fiterCtl;
		return *this;
	}
};