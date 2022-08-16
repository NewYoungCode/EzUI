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
		EString m_FontName;		//当前字体
		float m_FontSize = 0;			//字体大小
		int nDenominator = 72;
		bool down = false;
		HWND m_hWnd;
		HDC m_hDc;
		RECT m_rcBound;			//边框矩形
		UINT m_nFontX;
		UINT m_nFontY;
		StdString m_strBuff;				//编辑框文本缓存区
		UINT m_uCaretHeight;		//光标的高
		UINT m_uCaretWidth;		//光标的宽
		UINT m_uCaretX;
		UINT m_uCaretY;				//光标的x, y坐标
		bool m_bDrag;				//拖动
		bool m_bVisible;			//是否可见
		EEditState m_eState;				//当前状态
		UINT m_nCaret;			//光标在字符中的位置
		UINT m_nPrveCaret;		//光标上次的位置
		UINT m_nBuffSize;			//文本个数
		bool m_bIsCheck;			//是否选中
		RECT m_rcCheck;			//选中区域
		SCRIPT_STRING_ANALYSIS m_Analysis = NULL;	//字符串分析结果
	protected:
		virtual void OnLoad()override;
		virtual void OnPaint(PaintEventArgs& pe)override;
		virtual void OnBackgroundPaint(PaintEventArgs& painter)override;//背景绘制
		virtual void OnChar(WPARAM wParam, LPARAM lParam)override;
		virtual void OnKeyDown(WPARAM wParam)override;
		virtual void OnMouseDown(MouseButton mbtn, const Point& pt)override;
		virtual void OnMouseUp(MouseButton mbtn, const Point& pt)override;
		virtual void OnMouseMove(const Point& pt)override;
		virtual void OnMouseLeave() override;
	private:
		void Analyse(); //分析字符串
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