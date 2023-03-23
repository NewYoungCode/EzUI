#pragma once
#include "Control.h"
#include "Timer.h"
namespace EzUI {
	class UI_EXPORT TextBox :
		public Control
	{
		UI_GetClassName()
	private:
#ifndef UNICODE
		std::string ansiBuf;
#endif
		std::wstring text;//����
		bool _down = false;//�Ƿ���н�����
		bool _focus = false;//�Ƿ���н�����
		Point point_Start;//��ʼѡ�е�λ��
		Point point_End;//����λ��
		Rect selectRect;//ѡ�е��ַ�����
		Rect careRect;//���λ��
		Size FontBox;//����ռ�ÿռ�
		TextFormat* textFormat = NULL;//�����ʽ
		TextLayout* textLayout = NULL;//���岼��
		Point A;//A��
		BOOL A_isTrailingHit;//�����1��ʾ���ַ��ĺ���
		int A_TextPos = 0;//����˵ڼ����ַ�
		Point B;//B��
		BOOL B_isTrailingHit;//�����1��ʾ���ַ��ĺ���
		int B_TextPos = 0;//����˵ڼ����ַ�
		int TextPos = 0;//��ǰ���ֵ��±� 0~text.size()
		int x = 0;//���ڹ���
		Windows::Timer timer;//���ڹ����˸
		bool _careShow = false;//���ڹ����˸
	private:
		void Analysis();//�����ַ���
		void Insert(const std::wstring& str);//��������
		bool DeleteRange();//ɾ��ѡ������
		bool GetSelectedRange(int* outPos, int* outCount);//��ȡ��ǰ��ѡ�е����� �����±�͸���
		bool Copy();//���Ƶ����а�
		bool Paste();//ճ��
		bool SelectedAll();//ȫѡ
		void OnBackspace();//�˸��Ҫ������
		void BuildCare();//�������
		Point ConvertPoint(const Point& pt);//����ת��
	protected:
		virtual void OnRemove()override;
		virtual void OnSize(const Size& size)override;
		virtual void OnForePaint(PaintEventArgs& e) override;
		virtual void OnChar(WPARAM wParam, LPARAM lParam) override;
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam)override;
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam)override;
		virtual void OnMouseDown(MouseButton mbtn, const Point& point)override;
		virtual void OnMouseMove(const Point& point) override;
		virtual void OnMouseUp(MouseButton mbtn, const Point& point)override;
		virtual void OnKillFocus() override;
		virtual void OnMouseWheel(short zDelta, const Point& point)override;
	public:
		EString Placeholder;//placeholder���ö��� (��û�����ֵ��������ʾ������)
		Color SelectColor = Color(100, 255, 0, 0);//ѡ����ɫ
		Color CareColor = Color::Black;//�����ɫ
	public:
		TextBox();
		virtual ~TextBox();
		const EString GetText();
		EventTextChange TextChange = NULL;
		virtual void SetAttribute(const EString& key, const EString& value)override;
		void SetText(const EString& text);
	};
};
