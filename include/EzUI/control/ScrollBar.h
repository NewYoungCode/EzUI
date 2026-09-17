#pragma once
#include "Control.h"
#include "core/Animation.h"
namespace ezui {
	//滚动对齐方式
	EZUI_SCOPED_ENUM_BEGIN(ScrollAlignment, int16_t) {
		Auto,//自动
		Top,//出现在顶部
		Bottom,//出现在底部
		Left,//出现在左边
		Right//出现在右边
	} EZUI_SCOPED_ENUM_END(ScrollAlignment)
	//滚动条基类
	class EZUI_API ScrollBar :public Control {
		friend class VScrollBar;
		friend class HScrollBar;
		friend class Control;
		friend class Window;
		friend class TreeView;
	private:
		bool m_autoHide;//滚动条自动隐藏
		//鼠标是否已经按下
		bool m_mouseDown;
		// 当前滚动速度
		float m_velocity;
		//上一次鼠标命中的坐标
		int m_lastPoint;
		//滚动条当前的坐标
		double m_sliderPos;
		//滚动条的长度
		int m_sliderLength;
		//滚动条每滚动一次的比率
		double m_rollRate;
		//父容器内的坐标偏移
		int m_offset;
		//父容器的内容长度
		int m_contentLength;
		//父容器可见长度(容器自身长度)
		int m_viewLength;
		//溢出容器的长度
		int m_overflowLength;
		//滚动条最后一次滚动的值
		int m_lastPos;
		//滚动条最后一次最大滚动的值
		int m_lastMaxNum;
		//自动隐藏的超时时间 为0时表示不自动隐藏 单位毫秒
		uint16_t m_autoHideTimeout;
		//自动隐藏动画的持续时间
		uint16_t m_autoHideAnimationDuration;
	private:
		Animation* m_scrollDampingAnim;//鼠标滚动阻尼动画
		Animation* m_hoverFadeAnim;//鼠标悬停渐显渐隐动画
		bool m_needHide;
		DebounceTimer* m_hideDelayTimer;//自动隐藏的延时器
	private:
		void Init();
		//鼠标进入容器的动画
		void AnimateContainerOnHover();
		//鼠标离开容器的动画
		void AnimateContainerOnLeave();
	public:
		//滚动条计算出偏移之后的回调函数(内部使用)
		std::function<void(int)> OffsetCallback;
		//滚动回调事件 (sender: 触发者 value:滚动条当前值)
		std::function<void(ScrollBar* sender, int value)> ValueChanged;
	protected:
		virtual void OnBackgroundPaint(PaintEventArgs* arg)EZUI_OVERRIDE;
		virtual void OnForePaint(PaintEventArgs* args) EZUI_OVERRIDE;
		virtual void OnMouseDown(MouseEventArgs* arg)EZUI_OVERRIDE;
		virtual void OnMouseUp(MouseEventArgs* arg)EZUI_OVERRIDE;
		virtual void OnMouseLeave(MouseEventArgs* arg) EZUI_OVERRIDE;
		virtual void OnMouseWheel(MouseEventArgs* arg)EZUI_OVERRIDE;
		virtual void GetScrollMetrics(int* viewLength, int* contentLength, int* scrollBarLength) = 0;
		virtual void OnParentSize(const Size& parentSize) = 0;
		void OffsetTo(int offset);
		//刷新滚动条状态,根据内容长度和视口长度重新计算滑块参数
		void Recalc();
		//重置滚动条数据到起点(不执行重绘)
		void Reset();
	public:
		ScrollBar(Object* ownerObject = NULL);

		virtual ~ScrollBar();

		//滚动到指定控件可见位置
		virtual void ScrollTo(Control* ctrl, ScrollAlignment scrollAlignment = ScrollAlignment::Auto) = 0;

		//滚动到某个值
		void ScrollTo(int scrollValue);

		//获取当前已滚动的值
		virtual int GetValue();

		//获取最大可滚动的值
		virtual int GetMaxValue();

		//获取滑块的矩形
		virtual Rect GetSliderRect() = 0;//

		//当父控件发生内容发生改变 请调用刷新滚动条
		void RefreshScroll();

		//设置滚动条自动隐藏(当autoHideTimeout>0时,在鼠标停止滚动一段时间后会自动隐藏滚动条,当autoHideTimeout=0时,滚动条一直显示)
		void SetAutoHide(uint16_t autoHideTimeoutMs, uint16_t animationDurationMs = 300);

		//滚动条是否能够滚动
		bool Scrollable();

		//滚动条是否为显示状态(当滚动条不能滚动时候会返回false)
		bool IsShown();
	};
};
