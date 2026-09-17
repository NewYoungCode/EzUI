#pragma once
#include "UICore.h"
#include "UIStyle.h"
#include "control/Control.h"

namespace ezui {
	//UI加载器
	class EZUI_API UILoader :public Object {
		friend class Control;
	private:
		//记录xml中的标签以及控件
		struct XmlNode {
			Control* m_ctrl;
			UIString m_tagName;
		public:
			XmlNode(Control* ctrl, const UIString& tagName) :m_ctrl(ctrl), m_tagName(tagName) {}
		};
	private:
#ifdef DEBUG
		UIString m_fileName;
#endif // DEBUG
		UIString& m_styleStr;//内部用
		Collection<Control*>& m_rootNode;//根节点列表
		std::multimap<Control*, UIString> m_inlineStyles;//记录的内联样式
		void LoadControl(void* node, Control* control);
		Control* BuildControl(void* node);//内部函数
		void ApplyStyleRecursive(const std::list<ezui::StyleRule>& styles, Control* ctrl);
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
		void CleanUp();

		using Object::Detach;

		//解除loader和rootCtrl的绑定关系
		void Detach(Control* ctrl);
	};

	namespace detail {
		//注册基础控件
		extern EZUI_API void InitControls();
		//注册自定义控件
		extern EZUI_API void RegisterControl(const UIString& ctrlName, const std::function<Control* ()>& create_cb);
	};

	// 注册自定义控件。
	// 注意：控件类 T 必须提供默认构造函数（即无参构造函数），
	// 因为框架会通过 new T() 自动实例化控件对象。
	template<typename T>
	void RegisterControl(const UIString& ctrlName) {
		detail::RegisterControl(ctrlName, []() -> Control* { return new T; });
	}
};