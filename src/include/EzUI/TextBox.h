#pragma once
#include "Control.h"
#include "VScrollBar.h"
#include "Timer.h"
namespace EzUI {
	class UI_EXPORT TextBox :
		public Control
	{
	private:
		VScrollBar _vScrollbar;
		int _lastWidth = 0;
		int _lastHeight = 0;
		bool _multiLine = false;
		std::wstring _text;//����
		Size _fontBox;
		bool _down = false;//�Ƿ���н�����
		bool _focus = false;//�Ƿ���н�����
		Point _point_Start;//��ʼѡ�е�λ��
		Point _point_End;//����λ��
		std::list<Rect> _selectRects;//ѡ�е��ַ�����
		Rect _careRect;//���λ��
		Font* _font = NULL;//����
		TextLayout* _textLayout = NULL;//���岼��
		Point _A;//A��
		BOOL _A_isTrailingHit;//�����1��ʾ���ַ��ĺ���
		int _A_TextPos = 0;//����˵ڼ����ַ�
		Point _B;//B��
		BOOL _B_isTrailingHit;//�����1��ʾ���ַ��ĺ���
		int _B_TextPos = 0;//����˵ڼ����ַ�
		int _textPos = 0;//��ǰ���ֵ��±� 0~text.size()
		int _scrollX = 0;//�������ҹ���
		int _scrollY = 0;//����y�����
		int _lastX = 0;//��һ��xλ��
		int _lastY = 0;//��һ��yλ�� 
		Timer _timer;//���ڹ����˸
		bool _careShow = false;//���ڹ����˸
	private:
		void Init();
		void _Insert(const std::wstring& str);//��������
		bool DeleteRange();//ɾ��ѡ������
		bool GetSelectedRange(int* outPos, int* outCount);//��ȡ��ǰ��ѡ�е����� �����±�͸���
		bool Copy();//���Ƶ����а�
		bool Paste();//ճ��
		bool SelectedAll();//ȫѡ
		void OnBackspace();//�˸��Ҫ������
		void BuildCare();//�������
		void BuildSelectedRect();
		Point ConvertPoint(const Point& pt);//����ת��
	protected:
		virtual void OnRemove()override;
		virtual void SetAutoWidth(bool flag)override;
		virtual void SetAutoHeight(bool flag)override;
		virtual void OnForePaint(PaintEventArgs& e) override;
		virtual void OnKeyChar(const KeyboardEventArgs& arg) override;
		virtual void OnKeyDown(const KeyboardEventArgs& arg)override;
		virtual void OnMouseDown(const MouseEventArgs& arg)override;
		virtual void OnMouseWheel(const MouseEventArgs& arg)override;
		virtual void OnMouseMove(const MouseEventArgs& arg) override;
		virtual void OnMouseUp(const MouseEventArgs& arg)override;
		virtual void OnKillFocus(const KillFocusEventArgs& arg) override;
		virtual void OnLayout();
		void Offset(int moveY);
	public:
		size_t MaxCount = -1;//�����������
		EString Placeholder;//placeholder���ö��� (��û�����ֵ��������ʾ������)
		std::wstring PasswordChar;
		bool ReadOnly = false;//�Ƿ�ֻ��
		std::function<void(const EString&)> TextChanged = NULL;
	public:
		TextBox();
		virtual ~TextBox();
		virtual void SetAttribute(const EString& key, const EString& value)override;
		virtual Rect GetCareRect()override;
		void Analysis();//�����ַ���
		void Insert(const EString& str);//�ڵ�ǰ����в�������
		const EString GetText();
		virtual ScrollBar* GetScrollBar()override;
		void SetText(const EString& text);
		bool IsMultiLine();
		void SetMultiLine(bool multiLine);
	};
};
