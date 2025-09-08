#include "UILoader.h"
#include "IFrame.h"

#include "tinyxml.h"
#include "tinystr.h"

namespace ezui {
	struct XmlNode {
		Control* m_ctl;
		UIString m_tagName;
	public:
		XmlNode(Control* ctl, const UIString& tagName) :m_ctl(ctl), m_tagName(tagName) {}
	};

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

	Control* UILoader::BuildControl(void* _node) {
		TiXmlElement* node = (TiXmlElement*)_node;
		Control* ctl = NULL;
		std::string tagName(node->ValueTStr().c_str());
		ctl = this->OnBuildControl(tagName);
		if (ctl == NULL) {
			UIString text = UIString("unknow element \"%s\"").format(tagName.c_str());
			::MessageBoxA(NULL, text.c_str(), "UILoader Erro", MB_OK);
			ctl = new Control;
		}
		//设置控件属性
		TiXmlAttribute* attr = node->FirstAttribute();
		//内联样式
		UIString inlineStyle;
		do
		{
			if (!attr)break;
			UIString attrName = attr->Name();
			UIString attrValue = attr->Value();
			if (attrName == "style") {//暂时跳过内联样式
				inlineStyle = attrValue;//存储内联样式
			}
			else {
				ctl->SetAttribute(attrName, attrValue);
			}
		} while ((attr = attr->Next()));
		//为根控件挂载样式
		if (m_first) {
			ctl->SetStyleSheet(m_styleStr);
			m_first = false;
		}
		//最后应用内联样式
		if (!inlineStyle.empty()) {
			ctl->SetAttribute("style", inlineStyle);
		}
		return ctl;
	}
	void UILoader::LoadControl(void* _node, Control* control) {
		TiXmlElement* node = (TiXmlElement*)_node;

		TiXmlElement* fristChild = NULL;
		if ((fristChild = node->FirstChildElement()))//先寻找子控件
		{
			UIString tagName = fristChild->Value();
			Control* ctl = BuildControl(fristChild);
			control->Add(ctl);
			this->AttachControl(ctl, tagName);
			LoadControl(fristChild, ctl);
			TiXmlElement* nextChild = fristChild->NextSiblingElement();
			while (nextChild)//然后寻找兄弟
			{
				UIString tagName = nextChild->Value();
				Control* ctl2 = BuildControl(nextChild);
				control->Add(ctl2);
				this->AttachControl(ctl2, tagName);
				LoadControl(nextChild, ctl2);
				nextChild = nextChild->NextSiblingElement();
			}
		}
	}
	void UILoader::AttachControl(Control* ctl, const UIString& tagNamee)
	{
		//弹簧需要交给控件自行处理
		if (!ctl->IsSpacer()) {
			this->m_controls.push_back({ ctl, tagNamee });
		}
	}
	Control* UILoader::OnBuildControl(const UIString& tagName_) {
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
	void UILoader::SetupUI(Window* window)
	{
		if (!m_rootNode.empty()) {
			Control* root = m_rootNode[0];
			window->SetLayout(root);
		}
	}
	void UILoader::SetupUI(Control* parentCtl)
	{
		if (!m_rootNode.empty()) {
			Control* root = m_rootNode[0];
			parentCtl->m_controls.clear();//清除子控件
			parentCtl->Add(root);
		}
	}

	void UILoader::LoadXml(const UIString& fileName) {
		std::string data;
		if (GetResource(fileName, &data)) {
			LoadXml(data.c_str(), data.size());
		}
	}
	void UILoader::LoadXml(const char* data, size_t dataCount)
	{
		this->Clear();//清理旧的控件和样式
		TiXmlDocument doc;
		auto result = doc.Parse(data, NULL, TiXmlEncoding::TIXML_ENCODING_UTF8);
		//doc.Parse
		TiXmlElement* element = doc.FirstChildElement();//read frist element
		std::list<TiXmlElement*> controlNodes;
		//先处理样式
		do
		{
			if (element == NULL) break;
			if (element->ValueTStr() == "style") {// if element is style
				const char* szStr = element->GetText();
				if (szStr) {
					this->m_styleStr = szStr;//存储样式
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
			this->AttachControl(control, tagName);
			m_rootNode.push_back(control);//存入根节点集合
			LoadControl(element, control);//加载子节点
		}
		////为控件加载样式(根控件向下应用样式) //弃用
		//if (!m_rootNode.empty()) {
		//	m_rootNode[0]->SetStyleSheet(m_styleStr);
		//	//从新读取style属性加载内联样式(不应该这么做)
		//	UIString str = m_rootNode[0]->GetAttribute("style");
		//	m_rootNode[0]->SetStyleSheet(ControlState::Static, str);
		//}
	}

	Control* UILoader::GetRoot()
	{
		if (!m_rootNode.empty()) {
			return m_rootNode[0];
		}
		return NULL;
	}

	void UILoader::Clear() {
		for (auto itor = m_controls.rbegin(); itor != m_controls.rend(); ++itor)
		{
			Control* ctrl = itor->m_ctl;
			if (!ctrl->IsDestroying()) {
				delete ctrl;
			}
		}
		m_rootNode.clear();
		m_controls.clear();
		m_styleStr.clear();
		m_first = true;
	}

	UILoader::UILoader(Object* ownerObject) :Object(ownerObject)
	{
	}

	UILoader::~UILoader() {
		this->Clear();
	}

};