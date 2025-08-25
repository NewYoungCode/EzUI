#include "UIManager.h"
#include "IFrame.h"

#include "tinyxml.h"
#include "tinystr.h"

namespace ezui {
	inline UIString __Attribute(TiXmlElement* node, const char* szstr) {
		auto str = node->Attribute(szstr);
		if (str == NULL) return "";
		return str;
	}
};

namespace ezui {

	int MathStyle(Control* ctl, const UIString& selectorName, const std::list<UIManager::Style>& selectors)
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
	void ApplyStyle(Control* ctl, const  UIString& selectName, const std::list<UIManager::Style>& selectors)
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
	void UIManager::ApplyStyle(Control* ctl, const std::list<UIManager::Style>& selectors, const UIString& tagName) {
		{//加载样式 使用标签选择器
			if (!tagName.empty()) {
				UIString selectName = tagName;
				ezui::ApplyStyle(ctl, selectName, selectors);
			}
		}
		{//加载样式 使用属性选择器
			for (auto& attr : ctl->GetAttributes()) {
				UIString selectName = UIString("[%s=%s]").format(attr.first.c_str(), attr.second.c_str());
				ezui::ApplyStyle(ctl, selectName, selectors);
			}
		}
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
		{//加载内联样式 优先级最高
			UIString sytle_static = ctl->GetAttribute("style");//内联样式语法
			if (!sytle_static.empty()) { //内联样式只允许描述静态效果
				ctl->SetStyleSheet(ControlState::Static, sytle_static.trim());
			}
		}
	}
	void UIManager::AnalysisStyle(const UIString& styleStr, std::list<UIManager::Style>* out) {
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
				UIManager::Style selector;
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
	Control* UIManager::BuildControl(void* _node) {
		TiXmlElement* node = (TiXmlElement*)_node;
		Control* ctl = NULL;
		std::string tagName(node->ValueTStr().c_str());
		ctl = this->OnBuildControl(tagName);
		if (ctl == NULL) {
			UIString text = UIString("unknow element \"%s\"").format(tagName.c_str());
			::MessageBoxA(NULL, text.c_str(), "UIManager Erro", MB_OK);
			ctl = new Control;
		}
		//设置控件属性
		TiXmlAttribute* attr = node->FirstAttribute();
		do
		{
			if (!attr)break;
			UIString attrName = attr->Name();
			UIString attrValue(attr->Value());
			ctl->SetAttribute(attrName, attrValue);
		} while ((attr = attr->Next()));
		return ctl;
	}
	void UIManager::LoadControl(void* _node, Control* control) {
		TiXmlElement* node = (TiXmlElement*)_node;

		TiXmlElement* fristChild = NULL;
		if ((fristChild = node->FirstChildElement()))//先寻找子控件
		{
			UIString tagName = fristChild->Value();
			Control* ctl = BuildControl(fristChild);
			this->RegisterControl(ctl, tagName);
			LoadControl(fristChild, ctl);
			control->Add(ctl);
			TiXmlElement* nextChild = fristChild->NextSiblingElement();
			while (nextChild)//然后寻找兄弟
			{
				UIString tagName = nextChild->Value();
				Control* ctl2 = BuildControl(nextChild);
				this->RegisterControl(ctl2, tagName);
				LoadControl(nextChild, ctl2);
				control->Add(ctl2);
				nextChild = nextChild->NextSiblingElement();
			}
		}
	}
	void UIManager::RegisterControl(Control* ctl, const UIString& tagNamee)
	{
		//弹簧需要交给控件自行处理
		if (dynamic_cast<Spacer*>(ctl) == NULL) {
			this->m_controls.emplace_front(ctl, tagNamee);
		}
	}
	Control* UIManager::OnBuildControl(const UIString& tagName_) {
		UIString tagName = tagName_.toLower();
		Control* ctl = NULL;
		do
		{
			if (tagName == "control" || tagName == "layout" || tagName == "box") {
				ctl = new Control;
				break;
			}
			if (tagName == "vlist" || tagName == "vlistview") {
				ctl = new VListView;
				break;
			}
			if (tagName == "hlist" || tagName == "hlistview") {
				ctl = new HListView;
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
			if (tagName == "tilelist" || tagName == "tilelistview") {
				ctl = new TileListView;
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
			if (tagName == "combobox" || tagName == "select") {
				ctl = new ComboBox;
				break;
			}
			if (tagName == "edit" || tagName == "textbox" || tagName == "input") {
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

		} while (false);
		//自定义控件
		if (ctl == NULL && ControlBuilder) {
			ctl = ControlBuilder(tagName_);
		}
		return ctl;
	}
	UIManager::UIManager()
	{
	}
	void UIManager::SetupUI(Window* window)
	{
		Control* root = GetRoot();
		if (root && !root->GetSize().Empty()) {
			window->SetSize(root->GetSize());
			window->CenterToScreen();
		}
		window->SetLayout(root);
	}
	void UIManager::SetupUI(Control* parentCtl)
	{
		if (GetRoot()) {
			parentCtl->Add(GetRoot());
			GetRoot()->SetDockStyle(DockStyle::Fill);
		}
	}

	void UIManager::LoadXml(const UIString& fileName) {
		std::string data;
		if (GetResource(fileName, &data)) {
			LoadXml(data.c_str(), data.size());
		}
	}
	void UIManager::LoadXml(const char* fileData, size_t fileSize)
	{
		TiXmlDocument doc;
		auto result = doc.Parse(fileData, NULL, TiXmlEncoding::TIXML_ENCODING_UTF8);
		//doc.Parse
		TiXmlElement* element = doc.FirstChildElement();//read frist element
		std::list<TiXmlElement*> controlNodes;
		//存储样式
		UIString styleStr;
		//先处理样式
		do
		{
			if (element == NULL) break;
			if (element->ValueTStr() == "style") {// if element is style
				const char* szStr = element->GetText();
				if (szStr) {
					styleStr = szStr;
				}
			}
			else { //if no style , must be Control
				controlNodes.push_back(element);
			}
		} while ((element = element->NextSiblingElement()));
		//然后处理控件
		for (auto& element : controlNodes) {
			UIString tagName = element->Value();
			Control* control = BuildControl(element);//先加载根节点
			this->RegisterControl(control, tagName);
			LoadControl(element, control);//加载子节点
		}
		//加载样式
		this->SetStyleSheet(styleStr);
	}
	void UIManager::LoadStyle(const UIString& fileName)
	{
		std::string data;
		if (!GetResource(fileName, &data)) {
			return;
		}
		this->SetStyleSheet(data);
	}
	void UIManager::SetStyleSheet(const UIString& styleContent)
	{
		UIString data = styleContent;
		std::list<UIManager::Style> styles;
		//分析出样式
		this->AnalysisStyle(data, &styles);
		//保存样式到全局
		this->m_styles.splice(this->m_styles.end(), styles);  // 把 styles 的所有元素移动到 this->_styles 末尾
		if (!this->m_styles.empty()) {
			for (auto& it : this->m_controls) {
				//对所有加载进来的控件应用样式
				this->ApplyStyle(it.m_ctl, this->m_styles, it.m_tagName);
			}
		}
	}
	void UIManager::Free(Control** ctl)
	{
		for (auto itor = m_controls.begin(); itor != m_controls.end(); ++itor)
		{
			if (itor->m_ctl == *ctl) {
				delete* ctl;
				*ctl = NULL;
				m_controls.erase(itor);
				break;
			}
		}
	}
	UIManager::~UIManager() {
		for (auto& it : m_controls) {
			delete it.m_ctl;
		}
	}
	Control* UIManager::GetRoot() {
		if (!m_controls.empty()) {
			return m_controls.rbegin()->m_ctl;
		}
		return NULL;
	}

};