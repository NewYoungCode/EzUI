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
		Control* BuildControl(void* node);//�ڲ�����
		void Load(void* doc);//�ڲ�����
		void RecordControl(Control* ctl, const EString& tagNamee);//��¼xml�еĿؼ�
		void AnalysisStyle(const EString& styleStr, std::list<UIManager::Selector>* out);//������ʽ
		void ApplayStyle(Control* ctl, const std::list<UIManager::Selector>& selectors, const EString& tagName);//Ӧ����ʽ(Ϊ�ؼ�Ӧ��������ʽ)
	protected:
		void LoadFromRaw(const char* utf8str);//���ڴ��м���
		void LoadFromFile(const EString& fileName);//���ļ��м���
		virtual Control* OnBuildControl(const EString& nodeName);//��������һ���ڵ��ʱ����
		virtual void OnSetAttribute(Control* ctl, const EString& attrName, const EString& attrValue);//���ؼ��������Ե�ʱ����
	public:
		UIManager();
		virtual ~UIManager();
		void SetupUI(Window* window);
		void SetupUI(Control* parentCtl);
		std::function<Control* (const EString& nodeName)> EventBuilControl;//�����Զ���ؼ�
		std::function<void(Control* ctl, const EString& attrName, const EString& attrValue)> EventSetAttribute;//�����Զ���ؼ�
		void LoadXml(const EString& fileName);//�Զ��жϲ��Ҽ�������
		void LoadXml(const char* memData, size_t memSize);//�Զ��жϲ��Ҽ�������
		void LoadStyle(const EString& fileName);//����css
		void LoadStyle(const char* memData, size_t memSize);//����css
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