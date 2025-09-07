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
	struct XmlNode;
	//UI加载器
	class UI_EXPORT UILoader :public Object {
	private:
		std::vector<Control*> m_rootNode;//根节点列表
		std::list<XmlNode> m_controls;
		void LoadControl(void* node, Control* control);
		Control* BuildControl(void* node);//内部函数
		//记录XML中的控件到管理器 管理器释放的时候 由管理器加载的控件将自动释放
		void AttachControl(Control* ctl, const UIString& tagNamee);
	protected:
		//当解析到一个节点需要创建控件的时候发生
		virtual Control* OnBuildControl(const UIString& nodeName);
	public:
		UILoader(Object* ownerObject = NULL);
		virtual ~UILoader();
		//设置UI
		void SetupUI(Window* window);
		//设置UI
		void SetupUI(Control* parentCtl);
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