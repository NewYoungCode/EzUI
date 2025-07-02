#pragma once
#include <sstream>
#include <fstream>
#include "Control.h"
#include "Spacer.h"
#include "HLayout.h"
#include "Label.h"
#include "VLayout.h"
#include "TileListView.h"
#include "Button.h"
#include "VListView.h"
#include "HListView.h"
#include "RadioButton.h"
#include "CheckBox.h"
#include "TextBox.h"
#include "TabLayout.h"
#include "PictureBox.h"
#include "Window.h"
#include "ComboBox.h"
namespace EzUI {
	//主窗口中的内联页面类
	class UI_EXPORT IFrame;
	class UI_EXPORT UIManager {
	public:
		struct Selector
		{
			ControlStyle::Type styleType;
			UIString selectorName;
			UIString styleStr;
		};
		struct XmlNode {
			Control* ctl;
			UIString tagName;
		public:
			XmlNode(Control* ctl, const UIString& tagName) :ctl(ctl), tagName(tagName) {}
		};
	private:
		std::function<void(Image*)> BuildImageCallback;
		std::list<Image*> freeImages;
		std::list<XmlNode> controls;
		std::list<UIManager::Selector> _styles;
		void LoadControl(void* node, Control* control);
		Control* BuildControl(void* node);//内部函数
		//记录XML中的控件到管理器 管理器释放的时候 由管理器加载的控件将自动释放
		void RegisterControl(Control* ctl, const UIString& tagNamee);
		void AnalysisStyle(const UIString& styleStr, std::list<UIManager::Selector>* out);//分析样式
		void ApplyStyle(Control* ctl, const std::list<UIManager::Selector>& selectors, const UIString& tagName);
		//应用样式(为控件应用所有样式)
	protected:
		//当解析到一个节点的时候发生
		virtual Control* OnBuildControl(const UIString& nodeName);
	public:
		UIManager();
		virtual ~UIManager();
		void SetupUI(Window* window);
		void SetupUI(Control* parentCtl);
		//从文件中加载布局
		void LoadXmlFile(const UIString& fileName);
		//从字符串中加载布局
		void LoadXml(const UIString& xmlContent);
		//设置样式表
		void SetStyleSheet(const UIString& styleContent);
		//从文件中加载样式
		void LoadStyleFile(const UIString& fileName);
		//获取根节点控件
		Control* GetRoot();
		//释放由本此对象创建的控件
		void Free(Control** ctl);
		//释放由本此对象创建的图片
		void Free(Image** img);
	};

	class IFrame :public IIFrame {
		UIManager umg;
	public:
		IFrame() {}
		virtual ~IFrame() {}
		virtual void SetAttribute(const UIString& attrName, const UIString& attrValue)override {
			if (attrName == "src") {
				umg.LoadXmlFile(attrValue);
				umg.SetupUI(this);
				if (this->GetControls().size() > 0) {
					Control* root = this->GetControl(this->GetControls().size() - 1);
					root->SetDockStyle(DockStyle::Fill);
				}
			}
			__super::SetAttribute(attrName, attrValue);
		};
		//获取UI管理器
		UIManager* GetUIManager();
	};

};