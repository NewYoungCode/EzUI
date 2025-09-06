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

	std::map<UIString, std::function<Control* ()>> g_createFunc;
	void RegisterControl(const UIString& ctrlName, const std::function<Control* ()>& create_cb) {
		if (!ctrlName.empty() && create_cb) {
			g_createFunc[ctrlName.toLower()] = create_cb;
		}
	}
	void InitControls()
	{
		RegisterControl<Control>("control");
		RegisterControl<Control>("layout");
		RegisterControl<Control>("box");

		RegisterControl<VListView>("vlist");
		RegisterControl<VListView>("vlistview");

		RegisterControl<HListView>("hlist");
		RegisterControl<HListView>("hlistview");

		RegisterControl<VLayout>("vlayout");
		RegisterControl<VLayout>("vbox");

		RegisterControl<HLayout>("hlayout");
		RegisterControl<HLayout>("hbox");

		RegisterControl<TileListView>("tilelist");
		RegisterControl<TileListView>("tilelistview");

		RegisterControl<TabLayout>("tablayout");

		RegisterControl<Spacer>("spacer");
		RegisterControl<VSpacer>("vspacer");
		RegisterControl<HSpacer>("hspacer");

		RegisterControl<Label>("label");
		RegisterControl<Button>("button");
		RegisterControl<RadioButton>("radiobutton");
		RegisterControl<CheckBox>("checkbox");

		RegisterControl<ComboBox>("combobox");
		RegisterControl<ComboBox>("select");

		RegisterControl<TextBox>("edit");
		RegisterControl<TextBox>("textbox");
		RegisterControl<TextBox>("input");

		RegisterControl<PictureBox>("img");
		RegisterControl<PictureBox>("pictureBox");

		RegisterControl<IFrame>("iframe");
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
		if (!ctl->IsSpacer()) {
			this->m_controls.push_back({ ctl, tagNamee });
		}
	}
	Control* UIManager::OnBuildControl(const UIString& tagName_) {
		UIString tagName = tagName_.toLower();
		//优先匹配全局自定义控件
		for (auto& it : g_createFunc) {
			if (it.first == tagName) {
				Control* ctrl = it.second();
				if (ctrl) {
					return ctrl;
				}
			}
		}
		return NULL;
	}
	UIManager::UIManager()
	{
	}
	void UIManager::SetupUI(Window* window)
	{
		if (!m_rootNode.empty()) {
			Control* root = m_rootNode[0];
			window->SetLayout(root);
		}
	}
	void UIManager::SetupUI(Control* parentCtl)
	{
		if (!m_rootNode.empty()) {
			Control* root = m_rootNode[0];
			parentCtl->Clear();
			parentCtl->Add(root);
		}
	}

	void UIManager::LoadXml(const UIString& fileName) {
		std::string data;
		if (GetResource(fileName, &data)) {
			LoadXml(data.c_str(), data.size());
		}
	}
	void UIManager::LoadXml(const char* data, size_t dataCount)
	{
		this->Clear();//清理旧的控件和样式
		TiXmlDocument doc;
		auto result = doc.Parse(data, NULL, TiXmlEncoding::TIXML_ENCODING_UTF8);
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
			m_rootNode.push_back(control);//存入根节点集合
			LoadControl(element, control);//加载子节点
		}
		//加载样式
		this->LoadStyle(styleStr.c_str(), styleStr.size());
	}
	void UIManager::LoadStyle(const UIString& fileName)
	{
		std::string data;
		if (!GetResource(fileName, &data)) {
			return;
		}
		this->LoadStyle(data.c_str(), data.size());
	}
	void UIManager::LoadStyle(const char* data, size_t dataCount)
	{
		UIString styleStr(data, dataCount);
		std::list<UIManager::Style> styles;
		//分析出样式
		this->AnalysisStyle(styleStr, &styles);
		//保存样式到全局
		this->m_styles.splice(this->m_styles.end(), styles);  // 把 styles 的所有元素移动到 this->_styles 末尾
		if (!this->m_styles.empty()) {
			for (auto& it : this->m_controls) {
				//对所有加载进来的控件应用样式
				this->ApplyStyle(it.m_ctl, this->m_styles, it.m_tagName);
			}
		}
	}

	void UIManager::Clear() {
		for (auto itor = m_controls.rbegin(); itor != m_controls.rend(); ++itor)
		{
			Control* ctrl = itor->m_ctl;
			delete ctrl;
		}
		m_rootNode.clear();
		m_controls.clear();
		m_styles.clear();
	}

	UIManager::~UIManager() {
		this->Clear();
	}

};