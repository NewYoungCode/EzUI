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
#include "ComBox.h"
#include "Selector.h"
namespace EzUI {
	//主窗口中的内联页面类
	class UI_EXPORT IFrame;
	class UI_EXPORT UIManager {
	public:
		struct Selector
		{
			ControlStyle::Type styleType;
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
		std::list<UIManager::Selector> _styles;
		void LoadControl(void* node, Control* control);
		Control* BuildControl(void* node);//内部函数
		void RecordControl(Control* ctl, const EString& tagNamee);//记录xml中的控件
		void AnalysisStyle(const EString& styleStr, std::list<UIManager::Selector>* out);//分析样式
		void ApplayStyle(Control* ctl, const std::list<UIManager::Selector>& selectors, const EString& tagName);
		//应用样式(为控件应用所有样式)
	protected:
		virtual Control* OnBuildControl(const EString& nodeName);//当解析到一个节点的时候发生
	public:
		UIManager();
		virtual ~UIManager();
		void SetupUI(Window* window);
		void SetupUI(Control* parentCtl);
		//从文件中加载布局
		void LoadXmlFile(const EString& fileName);
		//从字符串中加载布局
		void LoadXml(const EString& xmlContent);
		//从字符串中加载样式
		void LoadStyle(const EString& styleContent);
		//从文件中加载样式
		void LoadStyleFile(const EString& fileName);
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
		virtual void SetAttribute(const EString& attrName, const EString& attrValue)override {
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
	};

};