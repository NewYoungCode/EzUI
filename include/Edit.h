#pragma once
#include "Control.h"
#include <usp10.h>
namespace EzUI {


	class Edit :public Control
	{
		enum  EEditState {
			E_Edit_Normal,
			E_Edit_Over,
			E_Edit_Focus,
			E_Edit_Enable,
		};
		typedef std::wstring StdString;
	public:
		Edit();
		virtual ~Edit();
	private:
		HFONT hFont = NULL;
		EString m_FontName;		//��ǰ����
		float m_FontSize = 0;			//�����С
		int nDenominator = 72;
		bool down = false;
		HWND m_hWnd;
		HDC m_hDc;
		RECT m_rcBound;			//�߿����
		UINT m_nFontX;
		UINT m_nFontY;
		StdString m_strBuff;				//�༭���ı�������
		UINT m_uCaretHeight;		//���ĸ�
		UINT m_uCaretWidth;		//���Ŀ�
		UINT m_uCaretX;
		UINT m_uCaretY;				//����x, y����
		bool m_bDrag;				//�϶�
		bool m_bVisible;			//�Ƿ�ɼ�
		EEditState m_eState;				//��ǰ״̬
		UINT m_nCaret;			//������ַ��е�λ��
		UINT m_nPrveCaret;		//����ϴε�λ��
		UINT m_nBuffSize;			//�ı�����
		bool m_bIsCheck;			//�Ƿ�ѡ��
		RECT m_rcCheck;			//ѡ������
		SCRIPT_STRING_ANALYSIS m_Analysis = NULL;	//�ַ����������
	protected:
		virtual void OnLoad()override;
		virtual void OnPaint(PaintEventArgs& pe)override;
		virtual void OnBackgroundPaint(PaintEventArgs& painter)override;//��������
		virtual void OnChar(WPARAM wParam, LPARAM lParam)override;
		virtual void OnKeyDown(WPARAM wParam)override;
		virtual void OnMouseDown(MouseButton mbtn, const Point& pt)override;
		virtual void OnMouseUp(MouseButton mbtn, const Point& pt)override;
		virtual void OnMouseMove(const Point& pt)override;
		virtual void OnMouseLeave() override;
	private:
		void Analyse(); //�����ַ���
		void UpCaret();
		HFONT GetFont(HDC hdc);
		void _SetText(LPCWSTR lpText);
		LRESULT MyMessage(UINT message, WPARAM wParam, LPARAM lParam);
		void CreateEdit(HWND hWnd, const RECT& rcEdit);
		void Update(HDC hdc);
		void SetEditVisible(bool bVisible);
		void SetEditState(EEditState eEditState);
	public:
		void SetText(const EString& text);
		const EString GetText();
	};
};