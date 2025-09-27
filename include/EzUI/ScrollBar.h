#pragma once
#include "Control.h"
#include "Animation.h"
namespace ezui {
	class UI_EXPORT ScrollBar :public Control {
		friend class VScrollBar;
		friend class HScrollBar;
		friend class Control;
	private:
		//鼠标是否已经按下
		bool m_mouseDown = false;
		//上一次鼠标命中的坐标
		int m_lastPoint = 0;
		//滚动条当前的坐标
		double m_sliderPos = 0;
		//滚动条的长度
		int m_sliderLength = 0;
		//滚动条每滚动一次的比率
		double m_rollRate = 0;
		//父容器内的坐标偏移
		int m_offset = 0;
		//父容器的内容长度
		int m_contentLength = 0;
		//父容器可见长度(容器自身长度)
		int m_viewLength = 0;
		//溢出容器的长度
		int m_overflowLength = 0;

		//int _old_viewLength = 0;
		//int _old_contentLength = 0;
		//int _old_offset = 0;
		float m_velocity = 0;  // 当前滚动速度
		Animation m_amt;//动画
	private:
		void Init();
	public:
		//滚动条计算出偏移之后的回调函数
		std::function<void(int)> OffsetCallback = NULL;
		//滚动事件 arg1:发送者 arg2:滚动百分比 arg3:滚动类型
		std::function<void(ScrollBar*, float, Event)> Scroll = NULL;
	protected:
		virtual void OnBackgroundPaint(PaintEventArgs& arg)override;
		virtual void OnForePaint(PaintEventArgs& args) override;
		virtual void OnMouseDown(const MouseEventArgs& arg)override;
		virtual void OnMouseUp(const MouseEventArgs& arg)override;
		virtual void OnMouseLeave(const MouseEventArgs& arg) override;
		virtual void OnMouseWheel(const MouseEventArgs& arg)override;
		virtual void GetInfo(int* viewLength, int* contentLength, int* scrollBarLength) = 0;
		virtual void OnParentSize(const Size& parentSize) = 0;
		void ScrollTo(int offset, const Event& type);
		//刷新滚动条状态,根据内容长度和视口长度重新计算滑块参数
		void Recalc();
	public:
		//滚动到指定控件可见位置
		virtual void ScrollTo(Control* ctl) = 0;
		//按照百分比滚动 0.0f~1.0f
		void ScrollTo(float scrollRate);
		//获取当前滚动到的位置 进度的百分比
		float ScrollPos();
		//获取滑块的矩形
		virtual Rect GetSliderRect() = 0;//
		//滚动条是否已经绘制且显示
		bool IsDraw();
		//重置滚动条数据到起点(不执行重绘)
		void Reset();
		//滚动条是否能够滚动
		bool Scrollable();
		//当父控件发生内容发生改变 请调用刷新滚动条
		void RefreshScroll();
		ScrollBar(Object* ownerObject = NULL);
		virtual ~ScrollBar();
	};
};