#include "UIManager.h"

#include "tinyxml.h"
#include "tinystr.h"

namespace EzUI {
	//ȥ���ո������������ ˫�����ڵĿո񲻻�ȥ��
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

		//���ÿؼ�����
		TiXmlAttribute* attr = node->FirstAttribute();
		do
		{
			if (!attr)break;
			EString attrName = attr->Name();
			EString attrValue(attr->Value());
			ctl->SetAttribute(attrName, attrValue);
			this->OnSetAttribute(ctl, attrName, attrValue);
		} while ((attr = attr->Next()));

		{//������ʽ �ȼ����
			EString sytle_static = ctl->GetAttribute("style");//������ʽ�﷨
			EString style_hover = ctl->GetAttribute("style:hover");//������ʽ�﷨
			EString style_active = ctl->GetAttribute("style:active");//������ʽ�﷨
			EString style_checked = ctl->GetAttribute("style:checked");//������ʽ�﷨

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
		//������ʽ ʹ�ñ�ǩѡ����
		if (!tagName.empty()) {
			__ApplayStyle(ctl, tagName, selectors, BuildImageCallback);
			__ApplayStyle(ctl, tagName + ":checked", selectors, BuildImageCallback);
			__ApplayStyle(ctl, tagName + ":active", selectors, BuildImageCallback);
			__ApplayStyle(ctl, tagName + ":hover", selectors, BuildImageCallback);
		}
		{//������ʽ ʹ����ѡ����  
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
		//������ʽ ʹ��IDѡ���� 
		if (!(ctl->Name.empty())) {
			__ApplayStyle(ctl, "#" + ctl->Name, selectors, BuildImageCallback);
			__ApplayStyle(ctl, "#" + ctl->Name + ":checked", selectors, BuildImageCallback);
			__ApplayStyle(ctl, "#" + ctl->Name + ":active", selectors, BuildImageCallback);
			__ApplayStyle(ctl, "#" + ctl->Name + ":hover", selectors, BuildImageCallback);
		}
		//Ϊ������������ʽ xml�ؼ������� scrollbar���Լ��ɵ���Ϊ������������ʽ
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
		if ((fristChild = node->FirstChildElement()))//��Ѱ���ӿؼ�
		{
			EString tagName = fristChild->Value();
			Control* ctl = BuildControl(fristChild);
			this->RecordControl(ctl, tagName);
			LoadControl(fristChild, ctl);
			control->Add(ctl);
			TiXmlElement* nextChild = fristChild->NextSiblingElement();
			while (nextChild)//Ȼ��Ѱ���ֵ�
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
	void UIManager::Load(void* _doc) {
		TiXmlDocument* doc = (TiXmlDocument*)_doc;
		//doc.Parse
		TiXmlElement* element = doc->FirstChildElement();//read frist element
		std::list<TiXmlElement*> controlNodes;
		//�洢��ʽ
		EString styleStr;
		//�ȴ�����ʽ
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
		//Ȼ����ؼ�
		for (auto& element : controlNodes) {
			EString tagName = element->Value();
			Control* control = BuildControl(element);//�ȼ��ظ��ڵ�
			this->RecordControl(control, tagName);
			LoadControl(element, control);//�����ӽڵ�
		}
		//������ʽ
		this->LoadStyle(styleStr.c_str(), styleStr.size());
	}
	void UIManager::RecordControl(Control* ctl, const EString& tagNamee)
	{
		//������Ҫ�����ؼ����д���
		if (dynamic_cast<Spacer*>(ctl) == NULL) {
			this->controls.emplace_front(ctl, tagNamee);
		}
	}
	void UIManager::LoadFromRaw(const char* xmlRaw)
	{
		TiXmlDocument doc;
		auto result = doc.Parse(xmlRaw, NULL, TiXmlEncoding::TIXML_ENCODING_UTF8);
		Load(&doc);
	}

	void UIManager::LoadFromFile(const EString& fileName)
	{
		FILE* file(0);
		_wfopen_s(&file, fileName.unicode().c_str(), L"rb");
		if (file == NULL) return;
		TiXmlDocument doc;
		auto result = doc.LoadFile(file, TiXmlEncoding::TIXML_ENCODING_UTF8);
		Load(&doc);
		::fclose(file);
	}

	void UIManager::AnalysisStyle(const EString& styleStr, std::list<UIManager::Selector>* out) {
		EString style = styleStr;
		//����ո� ˫�����ڵĿո񲻴���
		TrimStyle(style);
		while (true)
		{
			//����css��ע��
			auto pos1 = style.find("/*");
			auto pos2 = style.find("*/", pos1 + 2);
			if (pos1 != size_t(-1) && pos2 != size_t(-1)) {
				style.erase(pos1, pos2 - pos1 + 2);
			}
			else {
				break;
			}
		}
		//����ÿ����ʽ
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
		//������ʽ����
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
			//���ǵ����ѡ����
			auto names = name.Split(",");
			for (auto& name : names) {
				//���������
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
		window->SetLayout(GetRoot());
	}
	void UIManager::SetupUI(Control* parentCtl)
	{
		parentCtl->Add(GetRoot());
	}

	void UIManager::LoadXml(const EString& fileName) {
		std::string data;
		if (GetResource(fileName, &data)) {
			LoadFromRaw((const char*)data.c_str());
		}
	}
	void UIManager::LoadXml(const char* memData, size_t memSize)
	{
		LoadFromRaw(memData);
	}
	void UIManager::LoadStyle(const EString& fileName)
	{
		std::string data;
		if (!GetResource(fileName, &data)) {
			return;
		}
		this->LoadStyle(data.c_str(), data.size());
	}
	void UIManager::LoadStyle(const char* memData, size_t memSize)
	{
		EString data;
		data.append(memData, memSize);
		std::list<UIManager::Selector> styles;
		this->AnalysisStyle(data, &styles);
		if (!styles.empty()) {
			for (auto& it : controls) {
				//Ӧ����ʽ
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
	Control* UIManager::FindControl(const EString& ctlName) {
		if (ctlName.empty())return NULL;
		for (auto& it : controls) {
			if (it.ctl->Name == ctlName) {
				return it.ctl;
			}
		}
		return NULL;
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
		//ģ��jquery����Ԫ��ƥ��
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