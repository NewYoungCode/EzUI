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
		Point point_Start;//��ʼѡ�е�λ��
		Point point_End;//����λ��
		Rect selectRect;//ѡ�е��ַ�����
		Rect careRect;//���λ��
		Size FontBox;//����ռ�ÿռ�
		TextFormat* textFormat = NULL;//�����ʽ
		TextLayout* textLayout = NULL;//���岼��
		byte __i = 0;//���ڹ����˸
		Windows::Timer timer;//���ڹ����˸
		Point A;//A��
		BOOL A_isTrailingHit;//�����1��ʾ���ַ��ĺ���
		int A_TextPos = 0;//����˵ڼ����ַ�
		Point B;//B��
		BOOL B_isTrailingHit;//�����1��ʾ���ַ��ĺ���
		int B_TextPos = 0;//����˵ڼ����ַ�
		int TextPos = 0;//��ǰ���ֵ��±�
		BOOL isTrailingHit = TRUE;//�±�ƫ�� �����1��ʾ��� 0��ʾǰ��
		int x = 0;//���ڹ���
	private:
		void Insert(const std::wstring& str);//��������
		bool DeleteRange();//ɾ��ѡ������
		bool GetSelectedRange(int* outPos, int* outCount);//��ȡ��ǰ��ѡ�е����� �����±�͸���
		bool Copy();//���Ƶ����а�
		bool SelectedAll();//ȫѡ
		void OnBackspace();//�˸��Ҫ������
		void BuildCare();//�������
		Point ConvertPoint(const Point& pt);//����ת��
	protected:
		virtual bool OnSize(const Size& size)override;
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
		Edit();
		virtual ~Edit();
		virtual void SetAttribute(const EString& key, const EString& value)override;
		void SetText(const EString& text);
		EString GetText();
	};
};
