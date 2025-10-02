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
#include "UIStyle.h"

namespace ezui {
	//UI加载器
	class UI_EXPORT UILoader :public Object {
	private:
		//记录xml中的标签以及控件
		struct XmlNode {
			Control* m_ctrl;
			UIString m_tagName;
		public:
			XmlNode(Control* ctrl, const UIString& tagName) :m_ctrl(ctrl), m_tagName(tagName) {}
		};
	private:
		bool m_first = true;//内部用
		UIString m_styleStr;//内部用
		std::vector<Control*> m_rootNode;//根节点列表
		void LoadControl(void* node, Control* control);
		Control* BuildControl(void* node);//内部函数
	protected:
		//当解析到一个节点需要创建控件的时候发生
		virtual Control* OnBuildControl(const UIString& nodeName);
	public:
		UILoader(Object* ownerObject = NULL);
		virtual ~UILoader();
		//设置UI
		void SetupUI(Window* window);
		//从文件中加载布局
		void LoadXml(const UIString& fileName);
		//从内存加载布局
		void LoadXml(const char* data, size_t dataCount);
		//获取根控件
		Control* GetRoot();
		//释放加载进来的控件
		void Clear();
	};
	//注册基础控件
	extern UI_EXPORT void InitControls();
	//注册自定义控件
	extern UI_EXPORT void RegisterControl(const UIString& ctrlName, const std::function<Control* ()>& create_cb);
	//注册自定义控件
	template<typename T>
	void RegisterControl(const UIString& ctrlName) {
		RegisterControl(ctrlName, []() -> Control* { return new T; });
	}
};