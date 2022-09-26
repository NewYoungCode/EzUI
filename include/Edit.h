#pragma once
#include "Control.h"
#include "Timer.h"
#include <usp10.h>
#pragma comment (lib, "usp10.lib")
namespace EzUI {
	class UI_EXPORT Edit :
		public Control
	{
	private:
		std::wstring buf;//���ֻ���
		bool _focus = false;//�Ƿ���н�����
		SCRIPT_STRING_ANALYSIS _Analysis;	//�ַ����������
		void Analysis();
	protected:
		Point point_Start;
		Point point_End;
		Rect selectRect;//ѡ�е��ַ�����
		Rect careRect;//���λ��
		int FontHeight = 0;
		DxTextLayout* textLayout = NULL;
		DxTextFormat* textFormat = NULL;
	protected:
		virtual void OnSize(const Size& size)override;
		virtual void OnForePaint(PaintEventArgs& e) override;
		virtual void OnChar(WPARAM wParam, LPARAM lParam) override;
		virtual void OnKeyDown(WPARAM wParam)override;
		virtual void OnMouseDown(MouseButton mbtn, const Point& point)override;
		virtual void OnMouseMove(const Point& point) override;
		virtual void OnMouseUp(MouseButton mbtn, const Point& point)override;
		virtual void OnKillFocus() override;
	public:
		void SetText(const EString& text);
		EString GetText();
		//virtual ~Edit();
	};
};
