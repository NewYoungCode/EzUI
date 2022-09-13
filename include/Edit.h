#pragma once
#include "Control.h"
#include <usp10.h>
#pragma comment (lib, "usp10.lib")
namespace EzUI {
	class UI_EXPORT Edit :
		public Control
	{
	private:
		std::wstring buf;//���ֻ���
		bool _focus = false;//�Ƿ���н�����
		void MoveCaret(int left);//�ƶ�������
		SCRIPT_STRING_ANALYSIS _Analysis;	//�ַ����������
		void Analysis();
	protected:
		virtual void OnBackgroundPaint(PaintEventArgs& e) override;
		virtual void OnChar(WPARAM wParam, LPARAM lParam) override;
		virtual void OnKeyDown(WPARAM wParam)override;
		virtual void OnMouseDown(MouseButton mbtn, const Point& point)override;
		virtual void OnKillFocus() override;
	public:
		void SetText(const EString& text);
		EString GetText();
	};
};
