#pragma once
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

namespace ezui {
	//主窗口中的内联页面类
	class UI_EXPORT UIManager {
	public:
		struct Style
		{
			ControlState m_styleType;
			UIString m_selectorName;
			UIString m_styleStr;
		};
		struct XmlNode {
			Control* m_ctl;
			UIString m_tagName;
		public:
			XmlNode(Control* ctl, const UIString& tagName) :m_ctl(ctl), m_tagName(tagName) {}
		};
	private:
		std::vector<Control*> m_rootNode;//根节点列表
		std::list<XmlNode> m_controls;
		std::list<UIManager::Style> m_styles;
		void LoadControl(void* node, Control* control);
		Control* BuildControl(void* node);//内部函数
		//记录XML中的控件到管理器 管理器释放的时候 由管理器加载的控件将自动释放
		void RegisterControl(Control* ctl, const UIString& tagNamee);
		void AnalysisStyle(const UIString& styleStr, std::list<UIManager::Style>* out);//分析样式
		void ApplyStyle(Control* ctl, const std::list<UIManager::Style>& selectors, const UIString& tagName);
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
		void LoadXml(const UIString& fileName);
		//从内存加载布局
		void LoadXml(const char* fileData, size_t fileSize);
		//设置样式表
		void SetStyleSheet(const UIString& styleContent);
		//从文件中加载样式
		void LoadStyle(const UIString& fileName);
		//获取根节点控件
		Control* GetRoot(int index = 0);
		//释放由本此对象创建的控件
		void Free(Control** ctl);
	};
	//注册自定义控件
	void RegisterControl(const UIString& ctrlName, const std::function<Control* ()>& create_cb);
	//注册自定义控件
	template<typename T>
	void RegisterControl(const UIString& ctrlName) {
		RegisterControl(ctrlName, []() -> Control* { return new T; });
	}
};