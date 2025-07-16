#pragma once
#include "Control.h"

namespace ezui {
	class UI_EXPORT IScrollBar :public Control {
	protected:
		//鼠标是否已经按下
		bool m_mouseDown = false;
		//上一次鼠标命中的坐标
		int_t m_lastPoint = 0;
		//滚动条当前的坐标
		double m_sliderPos = 0;
		//滚动条的长度
		int_t m_sliderLength = 0;
		//滚动条每滚动一次的比率
		double m_rollRate = 0;
		//父容器内的坐标偏移
		int_t m_offset = 0;
		//父容器的内容长度
		int_t m_contentLength = 0;
		//父容器可见长度(容器自身长度)
		int_t m_viewLength = 0;
		//溢出容器的长度
		int_t m_overflowLength = 0;

		//int_t _old_viewLength = 0;
		//int_t _old_contentLength = 0;
		//int_t _old_offset = 0;
	public:
		//滚动条计算出偏移之后的回调函数
		std::function<void(int_t)> OffsetCallback = NULL;
		//滚动事件 arg1:发送者 arg2:滚动百分比 arg3:滚动类型
		std::function<void(IScrollBar*, float, Event)> Scroll = NULL;
	protected:
		virtual void OnBackgroundPaint(PaintEventArgs& arg)override;
		virtual void OnForePaint(PaintEventArgs& args) override;
		virtual void OnMouseUp(const MouseEventArgs& arg)override;
		virtual void OnMouseLeave(const MouseEventArgs& arg) override;
		virtual void OnMouseWheel(const MouseEventArgs& arg)override;
		virtual void GetInfo(int_t* viewLength, int_t* contentLength, int_t* scrollBarLength) = 0;
		void ScrollTo(int_t offset, const Event& type);
		void SyncInfo();
	public:
		//滚动到指定控件可见位置
		virtual void ScrollTo(Control* ctl) = 0;
		//按照百分比滚动 0.0f~1.0f
		void ScrollTo(float scrollRate);
		//获取当前滚动到的位置 进度的百分比
		float ScrollPos();
		//获取滑块的矩形
		virtual Rect GetSliderRect() = 0;//
		virtual void ParentSize(const Size& parentSize) = 0;
		//滚动条是否已经绘制且显示
		bool IsDraw();
		//重置滚动条数据到起点(不执行重绘)
		void Reset();
		//滚动条是否能够滚动
		bool Scrollable();
		//当父控件发生内容发生改变 请调用刷新滚动条
		void RefreshScroll();
		IScrollBar();
		virtual ~IScrollBar();
	};
};