#include "UIManager.h"

#include "tinyxml.h"
#include "tinystr.h"

namespace EzUI {
	//去除空格或者其他符号 双引号内的空格不会去除
	inline void __EraseChar(EString& str, char _char) {
		char* bufStr = new char[str.size() + 1] { 0 };
		size_t pos = 0;
		size_t count = 0;
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
		delete[] bufStr;
	}
	inline EString __Attribute(TiXmlElement* node, const char* szstr) {
		auto str = node->Attribute(szstr);
		if (str == NULL) return "";
		return str;
	}
	inline float __ToFloat(EString numStr) {
		//去掉px字样
		Text::Replace(&numStr, "px", "", true);
		return std::stof(numStr.c_str());
	}
};

namespace EzUI {

	void SetStyle(Control* ctl, ControlStyle* style, ControlStyle::Type styleType, const EString& key, EString value, const std::function<void(Image*)>& callback)
	{
		do
		{
			if (styleType == ControlStyle::Type::Static) {
				if (key == "width") {
					ctl->SetFixedWidth(__ToFloat(value));
					break;
				}
				if (key == "height") {
					ctl->SetFixedHeight(__ToFloat(value));
					break;
				}
			}

			if (key == "cursor") {
				if (value == "pointer") {
					style->Cursor = LoadCursor(EzUI::Cursor::HAND);
				}
				else if (value == "help") {
					style->Cursor = LoadCursor(EzUI::Cursor::HELP);
				}
				else if (value == "n-resize") {
					//南北箭头 纵向
					style->Cursor = LoadCursor(EzUI::Cursor::SIZENS);
				}
				else if (value == "e-resize") {
					//东西箭头 水平
					style->Cursor = LoadCursor(EzUI::Cursor::SIZEWE);
				}
				else if (value == "move") {
					//四个方向的箭头都有
					style->Cursor = LoadCursor(EzUI::Cursor::SIZEALL);
				}
				break;
			}
			if (key == "background-color") {
				style->BackColor = Color::Make(value);
				break;
			}
			if (key == "background-image") {
				value = value.replace("\"", "");//删除双引号;
				style->BackImage = Image::Make(value);
				if (callback) {
					callback(style->BackImage);
				}
				break;
			}
			if (key == "fore-image") {
				value = value.replace("\"", "");//删除双引号;
				style->ForeImage = Image::Make(value);
				if (callback) {
					callback(style->ForeImage);
				}
				break;
			}
			if (key == "color" || key == "fore-color") {
				style->ForeColor = Color::Make(value);
				break;
			}
			if (key == "border-color") {
				style->Border.Color = Color::Make(value);
				break;
			}
			if (key == "border-style") {
				if (value == "solid") {
					style->Border.BorderStyle = StrokeStyle::Solid;
				}
				else if (value == "dashed") {
					style->Border.BorderStyle = StrokeStyle::Dash;
				}
				break;
			}
			if (key == "border-radius") {
				style->Border.Radius = __ToFloat(value);
				break;
			}
			if (key == "border-top-left-radius") {
				style->Border.TopLeftRadius = __ToFloat(value);
				break;
			}
			if (key == "border-top-right-radius") {
				style->Border.TopRightRadius = __ToFloat(value);
				break;
			}
			if (key == "border-bottom-right-radius") {
				style->Border.BottomRightRadius = __ToFloat(value);
				break;
			}
			if (key == "border-bottom-left-radius") {
				style->Border.BottomLeftRadius = __ToFloat(value);
				break;
			}
			if (key == "font-size") {
				style->FontSize = __ToFloat(value);
				break;
			}
			if (key == "font-family") {
				value = value.replace("\"", "");//删除双引号;
				style->FontFamily = value.unicode();
				break;
			}
			if (key == "border") {
				auto width = __ToFloat(value);
				style->Border.Left = width;
				style->Border.Top = width;
				style->Border.Right = width;
				style->Border.Bottom = width;
				break;
			}
			if (key == "border-left") {
				style->Border.Left = __ToFloat(value);
				break;
			}
			if (key == "border-top") {
				style->Border.Top = __ToFloat(value);
				break;
			}
			if (key == "border-right") {
				style->Border.Right = __ToFloat(value);
				break;
			}
			if (key == "border-bottom") {
				style->Border.Bottom = __ToFloat(value);
				break;
			}
		} while (false);
	}
	void SetStyleSheet(Control* ctl, ControlStyle::Type styleType, const EString& styleStr, const std::function<void(Image*)>& callback)
	{
		//确定每一个样式的类型
		ControlStyle* cSytle = NULL;
		if (styleType == ControlStyle::Type::Static)cSytle = &ctl->Style;
		if (styleType == ControlStyle::Type::Hover)cSytle = &ctl->HoverStyle;
		if (styleType == ControlStyle::Type::Active)cSytle = &ctl->ActiveStyle;
		if (styleType == ControlStyle::Type::Checked) {
			auto* ckBox = dynamic_cast<CheckBox*>(ctl);
			if (ckBox) {
				cSytle = &ckBox->CheckedStyle;
			}
		}
		//分割每一行样式
		auto attrs = styleStr.split(";");
		for (auto& it : attrs) {
			size_t pos = it.find(":");
			if (pos == -1)continue;
			EString key = it.substr(0, pos);
			EString value = it.substr(pos + 1);
			SetStyle(ctl, cSytle, styleType, key, value, callback);//去应用每一行样式
		}
	}
	int_t MathStyle(Control* ctl, const EString& selectorName, const std::list<UIManager::Selector>& selectors, const std::function<void(Image*)>& BuildImageCallback)
	{
		int_t mathCount = 0;
		for (auto& it : selectors) {
			if (it.selectorName == selectorName) {
				++mathCount;
				auto& styleType = it.styleType;
				auto& styleStr = it.styleStr;
				SetStyleSheet(ctl, styleType, styleStr, BuildImageCallback);
			}
		}
		//返回匹配成功的个数
		return mathCount;
	}
	void ApplyStyle(Control* ctl, const  EString& selectName, const std::list<UIManager::Selector>& selectors, const std::function<void(Image*)>& BuildImageCallback)
	{
		MathStyle(ctl, selectName, selectors, BuildImageCallback);
		MathStyle(ctl, selectName + ":checked", selectors, BuildImageCallback);
		MathStyle(ctl, selectName + ":active", selectors, BuildImageCallback);
		MathStyle(ctl, selectName + ":hover", selectors, BuildImageCallback);
		//是否有滚动条 有滚动条则应用滚动条样式
		ScrollBar* scrollBar = ctl->GetScrollBar();
		if (scrollBar) {
			EString scrollBarSelectName = EString("%s::-webkit-scrollbar").format(selectName.c_str());
			MathStyle(scrollBar, scrollBarSelectName, selectors, BuildImageCallback);
			MathStyle(scrollBar, scrollBarSelectName + ":active", selectors, BuildImageCallback);
			MathStyle(scrollBar, scrollBarSelectName + ":hover", selectors, BuildImageCallback);
		}
	}
	void UIManager::ApplyStyle(Control* ctl, const std::list<UIManager::Selector>& selectors, const EString& tagName) {
		{//加载样式 使用标签选择器
			if (!tagName.empty()) {
				EString selectName = tagName;
				EzUI::ApplyStyle(ctl, selectName, selectors, this->BuildImageCallback);
			}
		}
		{//加载样式 使用属性选择器
			for (auto& attr : ctl->GetAttributes()) {
				EString selectName = EString("[%s=%s]").format(attr.first.c_str(), attr.second.c_str());
				EzUI::ApplyStyle(ctl, selectName, selectors, BuildImageCallback);
			}
		}
		{//加载样式 使用类选择器  
			EString classStr = ctl->GetAttribute("class");
			Text::Replace(&classStr, " ", ",");//去除所有空格转换为逗号
			auto classes = classStr.split(",");//分割类选择器
			for (const auto& className : classes) { //一个控件可能有多个类名
				EString selectName = EString(".%s").format(className.c_str());
				EzUI::ApplyStyle(ctl, selectName, selectors, BuildImageCallback);
			}
		}
		//加载样式 使用ID/name选择器 
		if (!(ctl->Name.empty())) {
			EString selectName = EString("#%s").format(ctl->Name.c_str());
			EzUI::ApplyStyle(ctl, selectName, selectors, BuildImageCallback);
		}
		{//加载内联样式 优先级最高
			EString sytle_static = ctl->GetAttribute("style");//内联样式语法
			if (!sytle_static.empty()) { //内联样式只允许描述静态效果
				__EraseChar(sytle_static, ' ');//去除空格
				EzUI::SetStyleSheet(ctl, ControlStyle::Type::Static, sytle_static, BuildImageCallback);
			}
		}
	}

	void UIManager::AnalysisStyle(const EString& styleStr, std::list<UIManager::Selector>* out) {
		EString style = styleStr;
		//处理空格 双引号内的空格不处理
		__EraseChar(style, ' ');
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
			auto names = name.split(",");
			for (auto& name : names) {
				//添加至集合
				UIManager::Selector selector;
				selector.selectorName = name;
				selector.styleStr = str;
				if (style_type == "hover") {
					selector.styleType = ControlStyle::Type::Hover;
				}
				else if (style_type == "active") {
					selector.styleType = ControlStyle::Type::Active;
				}
				else if (style_type == "checked") {
					selector.styleType = ControlStyle::Type::Checked;
				}
				else {
					selector.styleType = ControlStyle::Type::Static;
				}
				out->push_back(selector);
			}
		}
	}
	Control* UIManager::BuildControl(void* _node) {
		TiXmlElement* node = (TiXmlElement*)_node;
		Control* ctl = NULL;
		std::string tagName(node->ValueTStr().c_str());
		Text::Tolower(&tagName);
		ctl = this->OnBuildControl(tagName);
		if (ctl == NULL) {
			EString text = EString("unknow element \"%s\"").format(tagName.c_str());
			::MessageBoxA(NULL, text.c_str(), "UIManager Erro", MB_OK);
			ctl = new Control;
		}
		//设置控件属性
		TiXmlAttribute* attr = node->FirstAttribute();
		do
		{
			if (!attr)break;
			EString attrName = attr->Name();
			EString attrValue(attr->Value());
			ctl->SetAttribute(attrName, attrValue);
		} while ((attr = attr->Next()));
		return ctl;
	}
	void UIManager::LoadControl(void* _node, Control* control) {
		TiXmlElement* node = (TiXmlElement*)_node;

		TiXmlElement* fristChild = NULL;
		if ((fristChild = node->FirstChildElement()))//先寻找子控件
		{
			EString tagName = fristChild->Value();
			Control* ctl = BuildControl(fristChild);
			this->RegisterControl(ctl, tagName);
			LoadControl(fristChild, ctl);
			control->Add(ctl);
			TiXmlElement* nextChild = fristChild->NextSiblingElement();
			while (nextChild)//然后寻找兄弟
			{
				EString tagName = nextChild->Value();
				Control* ctl2 = BuildControl(nextChild);
				this->RegisterControl(ctl2, tagName);
				LoadControl(nextChild, ctl2);
				control->Add(ctl2);
				nextChild = nextChild->NextSiblingElement();
			}
		}
	}
	void UIManager::RegisterControl(Control* ctl, const EString& tagNamee)
	{
		//弹簧需要交给控件自行处理
		if (dynamic_cast<Spacer*>(ctl) == NULL) {
			this->controls.emplace_front(ctl, tagNamee);
		}
	}
	Control* UIManager::OnBuildControl(const EString& tagName) {
		Control* ctl = NULL;
		do
		{
			if (tagName == "control" || tagName == "layout" || tagName == "box") {
				ctl = new Control;
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
			if (tagName == "combox") {
				ctl = new ComBox;
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

		return ctl;
	}
	UIManager::UIManager()
	{
		BuildImageCallback = [this](Image* img)->void {
			this->freeImages.push_back((Image*)img);
			};
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
			this->RegisterControl(control, tagName);
			LoadControl(element, control);//加载子节点
		}
		//加载样式
		this->SetStyleSheet(styleStr);
	}
	void UIManager::LoadStyleFile(const EString& fileName)
	{
		std::string data;
		if (!GetResource(fileName, &data)) {
			return;
		}
		this->SetStyleSheet(data);
	}
	void UIManager::SetStyleSheet(const EString& styleContent)
	{
		EString data = styleContent;
		std::list<UIManager::Selector> styles;
		//分析出样式
		this->AnalysisStyle(data, &styles);
		//保存样式到全局
		this->_styles.splice(this->_styles.end(), styles);  // 把 styles 的所有元素移动到 this->_styles 末尾
		if (!this->_styles.empty()) {
			for (auto& it : this->controls) {
				//对所有加载进来的控件应用样式
				this->ApplyStyle(it.ctl, this->_styles, it.tagName);
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
			delete(*img);
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
	UIManager* IFrame::GetUIManager()
	{
		return &umg;
	}
};