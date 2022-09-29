#pragma once
#include "Control.h"
#include "Timer.h"

namespace EzUI {
	class UI_EXPORT Edit :
		public Control
	{
	private:
		std::wstring text;//���ֻ���
		bool _focus = false;//�Ƿ���н�����
		void Analysis();
	protected:
		Point point_Start;
		Point point_End;
		Rect selectRect;//ѡ�е��ַ�����
		Rect careRect;//���λ��
		int FontHeight = 0;
		TextLayout* textLayout = NULL;
		TextFormat* textFormat = NULL;
	public:
		int TextPos = 0;

		Point A;
		BOOL A_isTrailingHit;//�����1��ʾ���ַ��ĺ���
		int A_TextPos = 0;//����˵ڼ����ַ�

		Point B;
		BOOL B_isTrailingHit;//�����1��ʾ���ַ��ĺ���
		int B_TextPos = 0;//����˵ڼ����ַ�
	protected:
		virtual bool OnSize(const Size& size)override;
		virtual void OnForePaint(PaintEventArgs& e) override;
		virtual void OnChar(WPARAM wParam, LPARAM lParam) override;
		virtual void OnKeyDown(WPARAM wParam)override;
		virtual void OnMouseDown(MouseButton mbtn, const Point& point)override;
		virtual void OnMouseMove(const Point& point) override;
		virtual void OnMouseUp(MouseButton mbtn, const Point& point)override;
		virtual void OnKillFocus() override;
	protected:
		void BuildCare(int TextPos);
	public:
		virtual ~Edit();
		void SetText(const EString& text);
		EString GetText();
	};
};
