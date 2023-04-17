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
#include "Form.h"
namespace EzUI {
	class UI_EXPORT UIManager {
		enum class Style {
			Static,
			Hover,
			Active
		};
		struct Selector
		{
			UIManager::Style styleType;
			EString selectorName;
			EString styleStr;
		};
	private:
		std::list<UIManager::Selector> Selectors;//样式集合
		std::list<Control*> controls;
		void LoadControl(void* node, Control* control);
		Control* BuildControl(void* node);
		void LoadStyle(Control* ctl, const EString& selectorName);
		void AnalysisStyle(const EString& styleStr);
		void LoadFromRaw(const char* xmlRaw);
		void FreeImage(ControlStyle& style);//删除由此管理器创建的图像
		void FreeControl(Control* ctl);//删除由此管理器创建的控件
	protected:
		virtual void OnBuildControl(const EString& nodeName, Control** outCtl);
		virtual void OnSetAttribute(Control* ctl, const EString& attrName, const EString& attrValue);
	public:
		void SetupUI(Window* window);
		UIFunc<void(const EString& nodeName, Control** outCtl)> EventBuilControl;//用于自定义控件
		UIFunc<void(Control* ctl, const EString& attrName, const EString& attrValue)> EventSetAttribute;//用于自定义控件
		void LoadFile(const EString& fileName);
		virtual ~UIManager();
		Control* GetNodeByName(const EString& nodeName = "");
		Control* GetNode(size_t pos = 0);
	};

	//选择器
	class UI_EXPORT _Selector
	{
	private:
		Control* ctl = NULL;
		Control* notCtl = NULL;
		Controls ctls;
		_Selector& NextName(const EString& key) { return *this; };
		_Selector& NextId(const EString& key) { return *this; };
	public:
		_Selector(const Controls& Controls);
		_Selector(Control* control);
		_Selector& Css(const EString& styleStr);
		_Selector& CssHover(const EString& styleStr);
		_Selector& CssActive(const EString& styleStr);
		_Selector& Attr(const EString& key, const EString& value);
		_Selector& Refresh();
		_Selector& Not(Control* fiterCtl);
	};
#define $ _Selector
};