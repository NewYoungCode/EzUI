#pragma once
#include "Control.h"

namespace ezui {
	//可拖动滑条控件
	class EZUI_API Slider :public Control {
		float m_minValue = -8388608;      // 最小值
		float m_maxValue = 8388608;      // 最大值
		float m_value = 0;         // 当前值
	private:
		void SetValueFromPoint(const Point& location);
	protected:
		virtual void OnPaint(PaintEventArgs* args) override;
		virtual void OnMouseMove(MouseEventArgs* args) override;
		virtual void OnMouseDown(MouseEventArgs* args) override;
		//是否为垂直滑块
		bool IsVertical();
		//背景轨道绘制
		virtual void OnTrackPaint(PaintEventArgs* args);
		//滑块绘制
		virtual void OnThumbPaint(PaintEventArgs* args);
		//获取滑块的大小
		virtual SizeF GetThumbSize();
		//获取默认轨道矩形
		virtual RectF GetTrackRect();
		//获取滑块矩形
		virtual RectF GetThumbRect();
	public:
		//值发生更改时触发的回调
		std::function<void(Slider* sender, float value)> ValueChanged;
		virtual void SetAttribute(const UIString& attrName, const UIString& attrValue) override;
		//获取当前进度百分比
		float GetRatio();
		void SetValue(float value, bool triggerChangedEvent = false);
		float GetValue();
		void SetMinValue(float value);
		void SetMaxValue(float value);
		Slider(Object* ownerObj = NULL);
		virtual ~Slider();
	};
};
