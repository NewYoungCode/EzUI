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
		std::list<UIManager::Selector> Selectors;//��ʽ����
		std::list<Control*> controls;
		void LoadControl(void* node, Control* control);
		Control* BuildControl(void* node);
		void LoadStyle(Control* ctl, const EString& selectorName);
		void AnalysisStyle(const EString& styleStr);
		void Load(void* doc);//�ڲ�����
	protected:
		virtual Control* OnBuildControl(const EString& nodeName);
		virtual void OnSetAttribute(Control* ctl, const EString& attrName, const EString& attrValue);
	public:
		UIManager();
		virtual ~UIManager();
		void SetupUI(Window* window);
		void SetupUI(Control* parentCtl);
		std::function<Control* (const EString& nodeName)> EventBuilControl;//�����Զ���ؼ�
		std::function<void(Control* ctl, const EString& attrName, const EString& attrValue)> EventSetAttribute;//�����Զ���ؼ�
		void LoadFromRaw(const char* utf8str);//���ڴ��м���
		void LoadFromFile(const EString& fileName);//���ļ��м���
		void LoadFile(const EString& fileName);//�Զ��жϲ��Ҽ�������

		Control* GetNodeByName(const EString& nodeName = "");
		Control* GetNode(size_t pos = 0);
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
				umg.LoadFile(attrValue);
				umg.SetupUI(this);
				if (this->GetControls().size() > 0) {
					Control* root = this->GetControl(this->GetControls().size() - 1);
					root->SetDockStyle(DockStyle::Fill);
				}
			}
			__super::SetAttribute(attrName, attrValue);
		};
	};

	//ѡ����
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