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
		if (!m_rootNode.empty()) {
			m_rootNode[0]->SetStyleSheet(styleStr);
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
	}

	UIManager::~UIManager() {
		this->Clear();
	}

};