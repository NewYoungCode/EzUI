#pragma once
#include "Control.h"
#include "Timer.h"
#include "VScrollBar.h"
namespace EzUI {
	class UI_EXPORT TextBox :
		public ScrollableControl
	{
	private:
		VScrollBar _vsb;
		int lastWidth = 0;
		int lastHeight = 0;
		bool multiLine = false;
		std::wstring text;//����
		Size _fontBox;
		bool _down = false;//�Ƿ���н�����
		bool _focus = false;//�Ƿ���н�����
		Point point_Start;//��ʼѡ�е�λ��
		Point point_End;//����λ��
		std::list<Rect> selectRects;//ѡ�е��ַ�����
		Rect careRect;//���λ��
		Font* font = NULL;//����
		TextLayout* textLayout = NULL;//���岼��
		Point A;//A��
		BOOL A_isTrailingHit;//�����1��ʾ���ַ��ĺ���
		int A_TextPos = 0;//����˵ڼ����ַ�
		Point B;//B��
		BOOL B_isTrailingHit;//�����1��ʾ���ַ��ĺ���
		int B_TextPos = 0;//����˵ڼ����ַ�
		int TextPos = 0;//��ǰ���ֵ��±� 0~text.size()
		int scrollX = 0;//�������ҹ���
		int scrollY = 0;//����y�����
		int lastX = 0;//��һ��xλ��
		int lastY = 0;//��һ��yλ�� 
		Windows::Timer timer;//���ڹ����˸
		bool _careShow = false;//���ڹ����˸
		double sliderY = 0;
	private:
		void Init();
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
		virtual void OnForePaint(PaintEventArgs& e) override;
		virtual void OnKeyChar(WPARAM wParam, LPARAM lParam) override;
		virtual void BuildSelectedRect();
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam)override;
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam)override;
		virtual void OnMouseDown(MouseButton mbtn, const Point& point)override;
		virtual void OnMouseMove(const Point& point) override;
		virtual void OnMouseUp(MouseButton mbtn, const Point& point)override;
		virtual void OnKillFocus(Control* ctl) override;
		virtual void OnLayout();
		void Offset(int moveY);
	public:
		EString Placeholder;//placeholder���ö��� (��û�����ֵ��������ʾ������)
		std::wstring PasswordChar;
		Color SelectedColor = Color(100, 255, 0, 0);//ѡ����ɫ
		bool ReadOnly = false;//�Ƿ�ֻ��
		EventTextChange TextChange = NULL;
	public:
		TextBox();
		virtual ~TextBox();
		virtual void SetAttribute(const EString& key, const EString& value)override;
		virtual Rect GetCareRect()override;
		const EString GetText();
		virtual ScrollBar* GetScrollBar()override;
		void SetText(const EString& text);
		bool IsMultiLine();
		void SetMultiLine(bool multiLine);
	};
};
