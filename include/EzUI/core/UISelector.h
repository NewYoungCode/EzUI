#pragma once
#include "UILoader.h"

namespace ezui {
	//控件链式选择器
	class EZUI_API UISelector
	{
	private:
		ControlCollection m_controls;//锁定的控件
		ControlCollection m_filterCtrls;   // 所有被排除掉的控件
	private:
		void SaveFiltered(Control* ctrl);
	public:
		//选择多个控件
		UISelector(const ControlCollection& controls);

		//选择单个控件
		UISelector(Control* control);

		virtual ~UISelector();

		//排除单个控件
		UISelector& Not(Control* fiterCtrl);

		//排除指定下标的控件
		UISelector& Not(int pos);

		//排除一个集合中的全部控件
		UISelector& Not(const ControlCollection& fiterControls);

		//过滤控件(callback返回false则过滤该控件)
		UISelector& Filter(const std::function<bool(Control* control)>& callback);

		//通过属性筛选
		UISelector& Where(const UIString& attrName, const UIString& attrValue);

		//通过控件类型进行筛选
		template<typename T>
		UISelector& Where() {
			ControlCollection filtered;
			filtered.reserve(m_controls.size());
			for (auto it = m_controls.begin(); it != m_controls.end(); ++it) {
				Control* ctl = *it;
				if (dynamic_cast<T*>(ctl)) {
					filtered.push_back(ctl);
				}
			}
			m_controls = std::move(filtered);
			return *this;
		}

		//循环最终选定的控件
		UISelector& Each(const std::function<void(Control* control)>& eachFunc);

		//标记重绘
		UISelector& Invalidate();

		/// <summary>
		/// 为批量控件的指定状态设置样式
		/// </summary>
		/// <param name="styleStr">样式字符串，例如 "font-size: 13px; color: #ffffff;"。</param>
		/// <param name="state">控件状态，例如 VisualState::Hover。</param>
		UISelector& SetStyleSheet(const UIString& styleStr, VisualState state = VisualState::Normal);

		//批量控件设置属性
		UISelector& SetAttribute(const UIString& key, const UIString& value);

		//批量控件绑定事件
		UISelector& AddEventHandler(Event eventType, const EventHandler& eventCallback);

		//获取最终选定的控件集合
		const ControlCollection& Controls() const;

		//通过下标获取控件
		Control* operator[](int pos) const;

		//第一个控件
		Control* First() const;

		//最后一个控件
		Control* Last() const;

		//获取被过滤掉的控件集合
		const ControlCollection& Filters() const;

		//类型转换(返回新类型集合)
		template<typename T>
		Collection<T*> Cast() const {
			Collection<T*> outCollection;
			for (auto itor = m_controls.begin(); itor != m_controls.end(); ++itor) {
				T* out = dynamic_cast<T*>(*itor);
#ifdef DEBUG
				EZUI_ASSERT(!*itor || out, L"UISelector control type mismatch");
#endif
				if (out) {
					outCollection.push_back(out);
				}
			}
			return outCollection;
		}

	};
	//用于简便操作
	typedef UISelector $;
};
