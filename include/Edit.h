#pragma once
#include "Control.h"
#include "Timer.h"

namespace EzUI {
	class UI_EXPORT Edit :
		public Control
	{
	private:
		std::wstring text;//����
		EString Placeholder;//placeholder���ö���
		bool _down = false;//�Ƿ���н�����
		bool _focus = false;//�Ƿ���н�����
		void Analysis();//�����ַ���
	protected:
		Point point_Start;//��ʼѡ�е�λ��
		Point point_End;//����λ��
		Rect selectRect;//ѡ�е��ַ�����
		Rect careRect;//���λ��
		int FontHeight = 0;//�ַ��߶�
		Size FontBox;
		TextFormat* textFormat = NULL;//�����ʽ
		TextLayout* textLayout = NULL;//���岼��
		byte __i = 0;//���ڹ����˸
		Windows::Timer timer;//���ڹ����˸
	protected:
		Point A;
		BOOL A_isTrailingHit;//�����1��ʾ���ַ��ĺ���
		int A_TextPos = 0;//����˵ڼ����ַ�

		Point B;
		BOOL B_isTrailingHit;//�����1��ʾ���ַ��ĺ���
		int B_TextPos = 0;//����˵ڼ����ַ�
	protected:
		int TextPos = 0;
		BOOL isTrailingHit = TRUE;//�����1��ʾ���
		int x = 0;
	protected:
		virtual bool OnSize(const Size& size)override;
		virtual void OnForePaint(PaintEventArgs& e) override;
		virtual void OnChar(WPARAM wParam, LPARAM lParam) override;
		virtual void OnKeyDown(WPARAM wParam)override;
		virtual void OnMouseDown(MouseButton mbtn, const Point& point)override;
		virtual void OnMouseMove(const Point& point) override;
		virtual void OnMouseUp(MouseButton mbtn, const Point& point)override;
		virtual void OnKillFocus() override;
		virtual void OnMouseWheel(short zDelta, const Point& point)override;
	private:
		void Insert(const std::wstring& str);
		bool DeleteRange();
		bool GetSelectedRange(int* outPos, int* outCount);
		bool Copy();
		bool SelectedAll();
		void OnBackspace();
		Point ConvertPoint(const Point& pt);
	public:
		Edit();
		virtual ~Edit();
		virtual void SetAttribute(const EString& key, const EString& value);
		void SetText(const EString& text);
		EString GetText();
	};
};
