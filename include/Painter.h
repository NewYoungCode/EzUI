#pragma once
#include "EzUI.h"
#include "CPURender.h"
namespace EzUI {
	using Painter = CPURender;
	// 摘要: 
	// 为 OnPaint 事件提供数据。
	struct PaintEventArgs  {
		using _Painter_ = Painter;//预防重命名
		Painter& Painter;//画家
		Rect InvalidRectangle;//WM_PAINT里面的无效区域
		HWND HWnd;//父窗口句柄
		PaintEventArgs(_Painter_& painter) :Painter(painter) {
			//EventType = Event::OnPaint;
		}
		virtual ~PaintEventArgs() {}
	};
};