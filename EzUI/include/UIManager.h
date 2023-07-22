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
	class UI_EXPORT UIManager {
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
	private:
		std::function<void(Image*)> BuildImageCallback;
		std::list<Control*> freeControls;
		std::list<Image*> freeImages;
		std::list<UIManager::Selector> Selectors;//样式集合
		std::list<Control*> controls;
		void LoadControl(void* node, Control* control);
		Control* BuildControl(void* node);
		void LoadStyle(Control* ctl, const EString& selectorName);
		void AnalysisStyle(const EString& styleStr);
		void LoadFromRaw(const char* xmlRaw);
	protected:
		virtual Control* OnBuildControl(const EString& nodeName);
		virtual void OnSetAttribute(Control* ctl, const EString& attrName, const EString& attrValue);
	public:
		UIManager();
		virtual ~UIManager();
		void SetupUI(Window* window);
		void SetupUI(Control* parentCtl);
		std::function<Control* (const EString& nodeName)> EventBuilControl;//用于自定义控件
		std::function<void(Control* ctl, const EString& attrName, const EString& attrValue)> EventSetAttribute;//用于自定义控件
		void LoadFile(const EString& fileName);
		Control* GetNodeByName(const EString& nodeName = "");
		Control* GetNode(size_t pos = 0);
		void Free(Control**ctl);
		void Free(Image**img);
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