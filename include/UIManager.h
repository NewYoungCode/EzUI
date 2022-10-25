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
	private:
		std::map<EString, EString> styles;//默认样式集合
		std::map<EString, EString> styles_active;//按下样式集合
		std::map<EString, EString> styles_hover;//鼠标悬浮样式集合
		std::map<EString, EString> class_styles;//默认样式集合 类样式
		std::map<EString, EString> class_styles_active;//按下样式集合 类样式
		std::map<EString, EString> class_styles_hover;//鼠标悬浮样式集合 类样式
		std::list<Control*> controls;//加载的控件集合
		void LoadControl(void* node, Control* control);
		Control* BuildControl(void* node);
		void LoadStyle(Control* ctl, ControlState styleType);
		void AnalysisStyle(const EString& styleStr);
		void LoadFromFile(const EString& filename);
		void LoadFromRaw(const EString& xmlRaw);
	protected:
		virtual void OnBuildControl(const EString& nodeName, Control** outCtl);
		virtual void OnSetAttribute(Control* ctl, const EString& attrName, const EString& attrValue);
	public:
		UIFunc<void(const EString& nodeName, Control** outCtl)> EventBuilControl;//用于自定义控件
		UIFunc<void(Control* ctl, const EString& attrName, const EString& attrValue)> EventSetAttribute;//用于自定义控件
		UIManager(const EString& fileOrRes);
		Controls& GetControls();
		Control* GetRoot();
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