#pragma once
#include "EzUI.h"
#include "CPURender.h"
namespace EzUI {
	using Painter = CPURender;
	// ժҪ: 
	// Ϊ OnPaint �¼��ṩ���ݡ�
	struct PaintEventArgs  {
		using _Painter_ = Painter;//Ԥ��������
		Painter& Painter;//����
		Rect InvalidRectangle;//WM_PAINT�������Ч����
		HWND HWnd;//�����ھ��
		PaintEventArgs(_Painter_& painter) :Painter(painter) {
			//EventType = Event::OnPaint;
		}
		virtual ~PaintEventArgs() {}
	};
};