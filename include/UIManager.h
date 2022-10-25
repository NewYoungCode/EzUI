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
		std::map<EString, EString> styles;//Ĭ����ʽ����
		std::map<EString, EString> styles_active;//������ʽ����
		std::map<EString, EString> styles_hover;//���������ʽ����
		std::map<EString, EString> class_styles;//Ĭ����ʽ���� ����ʽ
		std::map<EString, EString> class_styles_active;//������ʽ���� ����ʽ
		std::map<EString, EString> class_styles_hover;//���������ʽ���� ����ʽ
		std::list<Control*> controls;//���صĿؼ�����
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
		UIFunc<void(const EString& nodeName, Control** outCtl)> EventBuilControl;//�����Զ���ؼ�
		UIFunc<void(Control* ctl, const EString& attrName, const EString& attrValue)> EventSetAttribute;//�����Զ���ؼ�
		UIManager(const EString& fileOrRes);
		Controls& GetControls();
		Control* GetRoot();
	};

	//ѡ����
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