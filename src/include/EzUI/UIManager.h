#pragma once
#include <sstream>
#include <fstream>
#include "Control.h"
#include "HLayout.h"
#include "Label.h"
#include "VLayout.h"
#include "TileList.h"
#include "Button.h"
#include "VList.h"
#include "HList.h"
#include "RadioButton.h"
#include "CheckBox.h"
#include "TextBox.h"
#include "TabLayout.h"
#include "PictureBox.h"
#include "Window.h"
namespace EzUI {
	class IFrame;
	class UI_EXPORT UIManager {
	public:
		enum class Style {
			Static,
			Checked,
			Hover,
			Active
		};
		struct Selector
		{
			UIManager::Style styleType;
			EString selectorName;
			EString styleStr;
		};
		struct XmlNode {
			Control* ctl;
			EString tagName;
		public:
			XmlNode(Control* ctl, const EString& tagName) :ctl(ctl), tagName(tagName) {}
		};
	private:
		std::function<void(Image*)> BuildImageCallback;
		std::list<Image*> freeImages;
		std::list<XmlNode> controls;
		void LoadControl(void* node, Control* control);
		Control* BuildControl(void* node);//内部函数
		void Load(void* doc);//内部函数
		void RecordControl(Control* ctl, const EString& tagNamee);//记录xml中的控件
		void AnalysisStyle(const EString& styleStr, std::list<UIManager::Selector>* out);//分析样式
		void ApplayStyle(Control* ctl, const std::list<UIManager::Selector>& selectors, const EString& tagName);//应用样式(为控件应用所有样式)
	protected:
		void LoadFromRaw(const char* utf8str);//从内存中加载
		void LoadFromFile(const EString& fileName);//从文件中加载
		virtual Control* OnBuildControl(const EString& nodeName);//当解析到一个节点的时候发生
		virtual void OnSetAttribute(Control* ctl, const EString& attrName, const EString& attrValue);//当控件设置属性的时候发生
	public:
		UIManager();
		virtual ~UIManager();
		void SetupUI(Window* window);
		void SetupUI(Control* parentCtl);
		std::function<Control* (const EString& nodeName)> EventBuilControl;//用于自定义控件
		std::function<void(Control* ctl, const EString& attrName, const EString& attrValue)> EventSetAttribute;//用于自定义控件
		void LoadXml(const EString& fileName);//自动判断并且加载内容
		void LoadXml(const char* memData, size_t memSize);//自动判断并且加载内容
		void LoadStyle(const EString& fileName);//加载css
		void LoadStyle(const char* memData, size_t memSize);//加载css
		Control* FindControl(const EString& ctlName);
		Control* GetRoot();
		void Free(Control** ctl);
		void Free(Image** img);
	};

	class IFrame :public Control {
		UIManager umg;
	public:
		IFrame() {}
		virtual ~IFrame() {}
		virtual void SetAttribute(const EString& attrName, const EString& attrValue)override {
			if (attrName == "src") {
				umg.LoadXml(attrValue);
				umg.SetupUI(this);
				if (this->GetControls().size() > 0) {
					Control* root = this->GetControl(this->GetControls().size() - 1);
					root->SetDockStyle(DockStyle::Fill);
				}
			}
			__super::SetAttribute(attrName, attrValue);
		};
	};

	//选择器
	class UI_EXPORT _Selector
	{
	private:
		Control* ctl = NULL;
		Control* notCtl = NULL;
		std::vector<Control*> ctls;
		_Selector& NextName(const EString& key) { return *this; };
		_Selector& NextId(const EString& key) { return *this; };
	public:
		_Selector(const std::vector<Control*>& controls);
		_Selector(const std::list<Control*>& controls);
		_Selector(Control* control);
		_Selector(Control* control, const EString& mathStr);
		virtual ~_Selector();
		_Selector& Css(const EString& styleStr);
		_Selector& CssHover(const EString& styleStr);
		_Selector& CssActive(const EString& styleStr);
		_Selector& Attr(const EString& key, const EString& value);
		_Selector& Refresh();
		_Selector& Not(Control* fiterCtl);
	};
#define $ _Selector
};