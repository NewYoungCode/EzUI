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
	inline bool __IsPx(const UIString& num, float& outNum) {
		size_t pos = num.find("px");
		if (pos != 0 && pos == num.size() - 2) {
			outNum = std::stof(num.substr(0, pos).c_str());
			return true;
		}
		return false;
	}
	inline float __ToFloat(const UIString& numStr) {
		float value;
		if (__IsPx(numStr, value)) {
			return value;
		}
		return 0;//解析失败
	}

	inline Image* __MakeImage(UIString value) {
		value = value.replace("\"", "");//删除双引号;
		auto pos1 = value.find("(");
		auto pos2 = value.find(")");
		if (pos1 != size_t(-1) && pos2 != size_t(-1)) {
			//background-image:url(res/images/xxx.png)的方式
			value = value.substr(pos1 + 1, pos2 - pos1 - 1);
		}
		return Image::Make(value);
	}

	inline void __MakeBorder(const UIString str, Border& bd, const std::function<void(float)>& callback) {
		auto values = str.split(" ");
		for (auto& v : values) {
			float num;
			if (v == "dashed") {
				bd.Style = StrokeStyle::Dash;
				continue;
			}
			else if (v == "solid") {
				bd.Style = StrokeStyle::Solid;
				continue;
			}
			else if (v == "none") {
				bd.Style = StrokeStyle::None;
				continue;
			}
			else if (__IsPx(v, num)) {
				callback(num);
				continue;
			}
			else
			{
				bool isGood = false;
				Color color = Color::Make(v, &isGood);
				if (isGood) {
					bd.Color = color;
				}
			}
		}
	}
};

namespace ezui {

	void SetStyle(Control* ctl, ControlStyle* style, ControlStyle::Type styleType, const UIString& key, UIString value, const std::function<void(Image*)>& callback)
	{
		do
		{
			if (styleType == ControlStyle::Type::Static) {
				if (key == "width") {
					if (value.count(".") > 0) {
						ctl->SetRateWidth(std::stof(value.c_str()));
					}
					else {
						ctl->SetFixedWidth(__ToFloat(value));
					}
					break;
				}
				if (key == "height") {
					if (value.count(".") > 0) {
						ctl->SetRateHeight(std::stof(value.c_str()));
					}
					else {
						ctl->SetFixedHeight(__ToFloat(value));
					}
					break;
				}
				if (key == "pointer-events") {
					if (value == "none") {
						//忽略鼠标事件 将直接穿透
						ctl->EventPassThrough = ctl->EventPassThrough | Event::OnMouseEvent;
					}
					else if (value == "auto") {
						ctl->EventPassThrough = Event::None;
					}
					break;
				}
			}

			if (key == "cursor") {
				if (value == "pointer") {
					style->Cursor = LoadCursor(ezui::Cursor::HAND);
				}
				else if (value == "help") {
					style->Cursor = LoadCursor(ezui::Cursor::HELP);
				}
				else if (value == "n-resize") {
					//南北箭头 纵向
					style->Cursor = LoadCursor(ezui::Cursor::SIZENS);
				}
				else if (value == "e-resize") {
					//东西箭头 水平
					style->Cursor = LoadCursor(ezui::Cursor::SIZEWE);
				}
				else if (value == "move") {
					//四个方向的箭头都有
					style->Cursor = LoadCursor(ezui::Cursor::SIZEALL);
				}
				break;
			}
			if (key == "background-color") {
				style->BackColor = Color::Make(value);
				break;
			}
			if (key == "background-image") {
				style->BackImage = __MakeImage(value);
				if (callback) {
					callback(style->BackImage);
				}
				break;
			}
			if (key == "background-size" || key == "background-image-size") {
				if (value == "auto" && style->BackImage) {
					style->BackImage->SizeMode = ImageSizeMode::OriginalSize;
				}
				break;
			}
			if (key == "fore-image") {
				style->ForeImage = __MakeImage(value);
				if (callback) {
					callback(style->ForeImage);
				}
				break;
			}
			if (key == "fore-image-size") {
				if (value == "auto" && style->ForeImage) {
					style->ForeImage->SizeMode = ImageSizeMode::OriginalSize;
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
					style->Border.Style = StrokeStyle::Solid;
				}
				else if (value == "dashed") {
					style->Border.Style = StrokeStyle::Dash;
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
				__MakeBorder(value, style->Border, [style](float num) {
					style->Border.Left = num;
					style->Border.Top = num;
					style->Border.Right = num;
					style->Border.Bottom = num;
					});
				break;
			}
			if (key == "border-left") {
				__MakeBorder(value, style->Border, [style](float num) {
					style->Border.Left = num;
					});
				break;
			}
			if (key == "border-top") {
				__MakeBorder(value, style->Border, [style](float num) {
					style->Border.Top = num;
					});
				break;
			}
			if (key == "border-right") {
				__MakeBorder(value, style->Border, [style](float num) {
					style->Border.Right = num;
					});
				break;
			}
			if (key == "border-bottom") {
				__MakeBorder(value, style->Border, [style](float num) {
					style->Border.Bottom = num;
					});
				break;
			}
		} while (false);
	}
	void SetStyleSheet(Control* ctl, ControlStyle::Type styleType, const UIString& styleStr, const std::function<void(Image*)>& callback)
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
			UIString key = it.substr(0, pos);
			UIString value = it.substr(pos + 1);
			key = key.trim();//去除前后空格
			value = value.trim();//去除前后空格
			SetStyle(ctl, cSytle, styleType, key, value, callback);//去应用每一行样式
		}
	}
	int_t MathStyle(Control* ctl, const UIString& selectorName, const std::list<UIManager::Style>& selectors, const std::function<void(Image*)>& BuildImageCallback)
	{
		int_t mathCount = 0;
		for (auto& it : selectors) {
			if (it.m_selectorName == selectorName) {
				++mathCount;
				auto& styleType = it.m_styleType;
				auto& styleStr = it.m_styleStr;
				SetStyleSheet(ctl, styleType, styleStr, BuildImageCallback);
			}
		}
		//返回匹配成功的个数
		return mathCount;
	}
	void ApplyStyle(Control* ctl, const  UIString& selectName, const std::list<UIManager::Style>& selectors, const std::function<void(Image*)>& BuildImageCallback)
	{
		MathStyle(ctl, selectName, selectors, BuildImageCallback);
		MathStyle(ctl, selectName + ":checked", selectors, BuildImageCallback);
		MathStyle(ctl, selectName + ":active", selectors, BuildImageCallback);
		MathStyle(ctl, selectName + ":hover", selectors, BuildImageCallback);
		//是否有滚动条 有滚动条则应用滚动条样式
		ScrollBar* scrollBar = ctl->GetScrollBar();
		if (scrollBar) {
			UIString scrollBarSelectName = UIString("%s::-webkit-scrollbar").format(selectName.c_str());
			MathStyle(scrollBar, scrollBarSelectName, selectors, BuildImageCallback);
			MathStyle(scrollBar, scrollBarSelectName + ":active", selectors, BuildImageCallback);
			MathStyle(scrollBar, scrollBarSelectName + ":hover", selectors, BuildImageCallback);
		}
	}
	void UIManager::ApplyStyle(Control* ctl, const std::list<UIManager::Style>& selectors, const UIString& tagName) {
		{//加载样式 使用标签选择器
			if (!tagName.empty()) {
				UIString selectName = tagName;
				ezui::ApplyStyle(ctl, selectName, selectors, this->BuildImageCallback);
			}
		}
		{//加载样式 使用属性选择器
			for (auto& attr : ctl->GetAttributes()) {
				UIString selectName = UIString("[%s=%s]").format(attr.first.c_str(), attr.second.c_str());
				ezui::ApplyStyle(ctl, selectName, selectors, BuildImageCallback);
			}
		}
		{//加载样式 使用类选择器  
			UIString classStr = ctl->GetAttribute("class");
			ui_text::Replace(&classStr, " ", ",");//去除所有空格转换为逗号
			auto classes = classStr.split(",");//分割类选择器
			for (const auto& className : classes) { //一个控件可能有多个类名
				UIString selectName = UIString(".%s").format(className.c_str());
				ezui::ApplyStyle(ctl, selectName, selectors, BuildImageCallback);
			}
		}
		//加载样式 使用ID/name选择器 
		if (!(ctl->Name.empty())) {
			UIString selectName = UIString("#%s").format(ctl->Name.c_str());
			ezui::ApplyStyle(ctl, selectName, selectors, BuildImageCallback);
		}
		{//加载内联样式 优先级最高
			UIString sytle_static = ctl->GetAttribute("style");//内联样式语法
			if (!sytle_static.empty()) { //内联样式只允许描述静态效果
				ezui::SetStyleSheet(ctl, ControlStyle::Type::Static, sytle_static.trim(), BuildImageCallback);
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
			if (pos1 != size_t(-1) && pos2 != size_t(-1)) {
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
			UIString name = style.substr(0, pos3);
			size_t pos4 = name.find(":");
			UIString style_type;
			UIString str = style.substr(pos3 + 1, pos2 - pos3 - 1);
			if (pos4 != size_t(-1)) {
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
					selector.m_styleType = ControlStyle::Type::Hover;
				}
				else if (style_type == "active") {
					selector.m_styleType = ControlStyle::Type::Active;
				}
				else if (style_type == "checked") {
					selector.m_styleType = ControlStyle::Type::Checked;
				}
				else {
					selector.m_styleType = ControlStyle::Type::Static;
				}
				out->push_back(selector);
			}
		}
	}
	Control* UIManager::BuildControl(void* _node) {
		TiXmlElement* node = (TiXmlElement*)_node;
		Control* ctl = NULL;
		std::string tagName(node->ValueTStr().c_str());
		ui_text::Tolower(&tagName);
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
	Control* UIManager::OnBuildControl(const UIString& tagName) {
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

		return ctl;
	}
	UIManager::UIManager()
	{
		BuildImageCallback = [this](Image* img)->void {
			if (img) {
				this->m_freeImages.push_back((Image*)img);
			}
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
		if (GetRoot()) {
			parentCtl->Add(GetRoot());
			GetRoot()->SetDockStyle(DockStyle::Fill);
		}
	}

	void UIManager::LoadXmlFile(const UIString& fileName) {
		std::string data;
		if (GetResource(fileName, &data)) {
			LoadXmlData(data);
		}
	}
	void UIManager::LoadXmlData(const UIString& xmlContent)
	{
		TiXmlDocument doc;
		auto result = doc.Parse(xmlContent.c_str(), NULL, TiXmlEncoding::TIXML_ENCODING_UTF8);
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
				styleStr = element->GetText();
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
	void UIManager::LoadStyleFile(const UIString& fileName)
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
	void UIManager::Free(Image** img)
	{
		auto itor = std::find(m_freeImages.begin(), m_freeImages.end(), *img);
		if (itor != m_freeImages.end()) {
			delete* itor;
			m_freeImages.erase(itor);
		}
		else
		{
			delete(*img);
		}
		*img = NULL;
	}
	UIManager::~UIManager() {
		for (auto& it : m_controls) {
			delete it.m_ctl;
		}
		for (auto& it : m_freeImages) {
			delete it;
		}
	}
	Control* UIManager::GetRoot() {
		if (!m_controls.empty()) {
			return m_controls.rbegin()->m_ctl;
		}
		return NULL;
	}

};