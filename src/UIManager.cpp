#include "UIManager.h"

#include "tinyxml.h"
#include "tinystr.h"

namespace EzUI {
	//去除空格或者其他符号 双引号内的空格不会去除
	void TrimStyle(EString& str, CHAR _char = ' ') {
		CHAR* bufStr = new CHAR[str.size() + 1]{ 0 };
		size_t pos = 0;
		char count = 0;
		for (auto& it : str) {
			if (it == '"') {
				++count;
			}
			if (it == _char && count != 1) {
				continue;
			}
			if (count == 2)count = 0;
			bufStr[pos] = it;
			++pos;
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
		std::string tagName(node->ValueTStr().c_str());
		EString::Tolower(&tagName);
		do
		{
			if (tagName == "control") {
				ctl = new Control();
				break;
			}
			if (tagName == "vlist") {
				ctl = new VList;
				break;
			}
			if (tagName == "hlist") {
				ctl = new HList;
				break;
			}
			if (tagName == "vlayout" || tagName == "vbox") {
				ctl = new VLayout;
				break;
			}
			if (tagName == "hlayout" || tagName == "hbox") {
				ctl = new HLayout;
				break;
			}
			if (tagName == "layout" || tagName == "box") {
				ctl = new Control;
				break;
			}
			if (tagName == "tilelist") {
				ctl = new TileList;
				break;
			}
			if (tagName == "tablayout") {
				ctl = new TabLayout;
				break;
			}
			if (tagName == "spacer") {
				ctl = new Spacer();
				break;
			}
			if (tagName == "vspacer") {
				ctl = new VSpacer(0);
				break;
			}
			if (tagName == "hspacer") {
				ctl = new HSpacer(0);
				break;
			}
			if (tagName == "label") {
				ctl = new Label;
				break;
			}
			if (tagName == "button") {
				ctl = new Button;
				break;
			}
			if (tagName == "radiobutton") {
				ctl = new RadioButton;
				break;
			}
			if (tagName == "checkbox") {
				ctl = new CheckBox;
				break;
			}
			if (tagName == "edit" || tagName == "textbox") {
				ctl = new TextBox;
				break;
			}
			if (tagName == "img" || tagName == "pictureBox") {
				ctl = new PictureBox;
				break;
			}
			if (tagName == "iframe") {
				ctl = new IFrame;
				break;
			}
			if (ctl == NULL) {
				ctl = this->OnBuildControl(tagName);
			}
			if (ctl == NULL) {
				::MessageBoxA(NULL, EString("UnKnow Element \"" + tagName + "\"").c_str(), "UIManager Erro", NULL);
				ctl = new Control;
			}
		} while (false);

		//设置控件属性
		TiXmlAttribute* attr = node->FirstAttribute();
		do
		{
			if (!attr)break;
			EString attrName = attr->Name();
			EString attrValue(attr->Value());
			ctl->SetAttribute(attrName, attrValue);
			this->OnSetAttribute(ctl, attrName, attrValue);
		} while ((attr = attr->Next()));

		{//内联样式 等级最低
			EString sytle_static = ctl->GetAttribute("style");//内联样式语法
			EString style_hover = ctl->GetAttribute("style:hover");//内联样式语法
			EString style_active = ctl->GetAttribute("style:active");//内联样式语法
			EString style_checked = ctl->GetAttribute("style:checked");//内联样式语法

			if (!sytle_static.empty()) {
				TrimStyle(sytle_static);
				ctl->Style.SetStyleSheet(sytle_static, BuildImageCallback);
			}
			if (!style_hover.empty()) {
				TrimStyle(style_hover);
				ctl->HoverStyle.SetStyleSheet(style_hover, BuildImageCallback);
			}
			if (!style_active.empty()) {
				TrimStyle(style_active);
				ctl->ActiveStyle.SetStyleSheet(style_active, BuildImageCallback);
			}
			if (!style_checked.empty()) {
				auto* ckBox = dynamic_cast<CheckBox*>(ctl);
				if (ckBox) {
					TrimStyle(style_checked);
					ckBox->CheckedStyle.SetStyleSheet(style_checked, BuildImageCallback);
				}
			}
		}
		return ctl;
	}
	void __ApplayStyle(Control* ctl, const EString& selectorName, const std::list<UIManager::Selector>& selectors, const std::function<void(Image*)>& BuildImageCallback)
	{
		for (auto& it : selectors) {
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
					auto* ckBox = dynamic_cast<CheckBox*>(ctl);
					if (ckBox) {
						ckBox->CheckedStyle.SetStyleSheet(it.styleStr, BuildImageCallback);
					}
				}
			}
		}
	}
	void UIManager::ApplayStyle(Control* ctl, const std::list<UIManager::Selector>& selectors, const EString& tagName) {
		//加载样式 使用标签选择器
		if (!tagName.empty()) {
			__ApplayStyle(ctl, tagName, selectors, BuildImageCallback);
			__ApplayStyle(ctl, tagName + ":checked", selectors, BuildImageCallback);
			__ApplayStyle(ctl, tagName + ":active", selectors, BuildImageCallback);
			__ApplayStyle(ctl, tagName + ":hover", selectors, BuildImageCallback);
		}
		{//加载样式 使用类选择器  
			EString _class = ctl->GetAttribute("class");
			EString::Replace(&_class, "  ", " ");
			auto classs = _class.Split(" ");
			for (auto& className : classs) {
				__ApplayStyle(ctl, "." + className, selectors, BuildImageCallback);
				__ApplayStyle(ctl, "." + className + ":checked", selectors, BuildImageCallback);
				__ApplayStyle(ctl, "." + className + ":active", selectors, BuildImageCallback);
				__ApplayStyle(ctl, "." + className + ":hover", selectors, BuildImageCallback);
			}
		}
		//加载样式 使用ID选择器 
		if (!(ctl->Name.empty())) {
			__ApplayStyle(ctl, "#" + ctl->Name, selectors, BuildImageCallback);
			__ApplayStyle(ctl, "#" + ctl->Name + ":checked", selectors, BuildImageCallback);
			__ApplayStyle(ctl, "#" + ctl->Name + ":active", selectors, BuildImageCallback);
			__ApplayStyle(ctl, "#" + ctl->Name + ":hover", selectors, BuildImageCallback);
		}
		//为滚动条设置样式 xml控件中设置 scrollbar属性即可单独为滚动条设置样式
		ScrollBar* sb = ctl->GetScrollBar();
		if (sb && !sb->Name.empty()) {
			__ApplayStyle(ctl, "#" + sb->Name, selectors, BuildImageCallback);
			__ApplayStyle(ctl, "#" + sb->Name + ":active", selectors, BuildImageCallback);
			__ApplayStyle(ctl, "#" + sb->Name + ":hover", selectors, BuildImageCallback);
		}
	}

	void UIManager::LoadControl(void* _node, Control* control) {
		TiXmlElement* node = (TiXmlElement*)_node;

		TiXmlElement* fristChild = NULL;
		if ((fristChild = node->FirstChildElement()))//先寻找子控件
		{
			EString tagName = fristChild->Value();
			Control* ctl = BuildControl(fristChild);
			this->RecordControl(ctl, tagName);
			LoadControl(fristChild, ctl);
			control->Add(ctl);
			TiXmlElement* nextChild = fristChild->NextSiblingElement();
			while (nextChild)//然后寻找兄弟
			{
				EString tagName = fristChild->Value();
				Control* ctl2 = BuildControl(nextChild);
				this->RecordControl(ctl2, tagName);
				LoadControl(nextChild, ctl2);
				control->Add(ctl2);
				nextChild = nextChild->NextSiblingElement();
			}
		}
	}

	void UIManager::RecordControl(Control* ctl, const EString& tagNamee)
	{
		//弹簧需要交给控件自行处理
		if (dynamic_cast<Spacer*>(ctl) == NULL) {
			this->controls.emplace_front(ctl, tagNamee);
		}
	}


	void UIManager::AnalysisStyle(const EString& styleStr, std::list<UIManager::Selector>* out) {
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
				out->push_back(selector);
			}
		}
	}


	Control* UIManager::OnBuildControl(const EString& nodeName) {

		return NULL;
	}
	void UIManager::OnSetAttribute(Control* ctl, const EString& attrName, const EString& attrValue) {

	}
	UIManager::UIManager()
	{
		BuildImageCallback = [this](Image* img)->void {
			this->freeImages.push_back((Image*)img);
			};
	}
	void UIManager::SetupUI(Window* window)
	{
		window->SetLayout(GetRoot());
	}
	void UIManager::SetupUI(Control* parentCtl)
	{
		parentCtl->Add(GetRoot());
	}

	void UIManager::LoadXmlFile(const EString& fileName) {
		std::string data;
		if (GetResource(fileName, &data)) {
			LoadXml(data);
		}
	}
	void UIManager::LoadXml(const EString& xmlContent)
	{
		TiXmlDocument doc;
		auto result = doc.Parse(xmlContent.c_str(), NULL, TiXmlEncoding::TIXML_ENCODING_UTF8);
		//doc.Parse
		TiXmlElement* element = doc.FirstChildElement();//read frist element
		std::list<TiXmlElement*> controlNodes;
		//存储样式
		EString styleStr;
		//先处理样式
		do
		{
			if (element == NULL) break;
			if (element->ValueTStr() == "style") {// if element is style
				styleStr = element->GetText();
			}
			else { //if no style , must be Control
				controlNodes.push_back(element);
			}
		} while ((element = element->NextSiblingElement()));
		//然后处理控件
		for (auto& element : controlNodes) {
			EString tagName = element->Value();
			Control* control = BuildControl(element);//先加载根节点
			this->RecordControl(control, tagName);
			LoadControl(element, control);//加载子节点
		}
		//加载样式
		this->LoadStyle(styleStr);
	}
	void UIManager::LoadStyleFile(const EString& fileName)
	{
		std::string data;
		if (!GetResource(fileName, &data)) {
			return;
		}
		this->LoadStyle(data);
	}
	void UIManager::LoadStyle(const EString& styleContent)
	{
		EString data = styleContent;
		std::list<UIManager::Selector> styles;
		this->AnalysisStyle(data, &styles);
		if (!styles.empty()) {
			for (auto& it : controls) {
				//应用样式
				this->ApplayStyle(it.ctl, styles, it.tagName);
			}
		}
	}
	void UIManager::Free(Control** ctl)
	{
		for (auto itor = controls.begin(); itor != controls.end(); ++itor)
		{
			if (itor->ctl == *ctl) {
				delete* ctl;
				*ctl = NULL;
				controls.erase(itor);
				break;
			}
		}
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
		for (auto& it : controls) {
			delete it.ctl;
		}
		for (auto& it : freeImages) {
			delete it;
		}
	}

	Control* UIManager::GetRoot() {
		if (!controls.empty()) {
			return controls.rbegin()->ctl;
		}
		return NULL;
	}

	_Selector::_Selector(const std::vector<Control*>& controls)
	{
		this->ctls = controls;
	}
	_Selector::_Selector(const std::list<Control*>& controls)
	{
		this->ctls.resize(controls.size());
		for (auto& it : controls) {
			this->ctls.push_back(it);
		}
	}
	_Selector::_Selector(Control* ctl)
	{
		this->ctl = ctl;
	}
	_Selector::_Selector(Control* ct, const EString& _mathStr)
	{
		EString mathStr = _mathStr;
		mathStr.Replace("  ", " ");
		//模仿jquery进行元素匹配
	}
	_Selector::~_Selector()
	{
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