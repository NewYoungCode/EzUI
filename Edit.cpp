#include "Edit.h"
#pragma comment (lib, "usp10.lib")
namespace EzUI {

	Edit::Edit()
	{
		m_hWnd = NULL;
		m_hDc = NULL;
		m_bVisible = true;
		m_eState = E_Edit_Normal;
		m_bDrag = false;
		m_bIsCheck = false;
		m_nCaret = 0;
		m_nPrveCaret = 0;
		m_uCaretWidth = 1;
	}

	Edit::~Edit()
	{
		if (hFont) {
			DeleteFont(hFont);
		}
		if (m_hDc) {
			ReleaseDC(m_hWnd, m_hDc);
		}
		if (m_Analysis) {
			ScriptStringFree(&m_Analysis);
		}
		DestroyCaret();
	}

	void Edit::SetText(const EString& text)
	{
		_SetText(text.utf16().c_str());
	}
	const EString Edit::GetText()
	{
		return  m_strBuff;
	}

	void Edit::CreateEdit(HWND hWnd, const RECT& rcEdit)
	{
		m_hWnd = hWnd;
		m_rcBound = rcEdit;
		m_nFontX = rcEdit.left + 2;
		m_nFontY = rcEdit.top + 2;
		m_uCaretX = m_nFontX;
		m_uCaretY = m_nFontY;
		m_hDc = GetDC(hWnd);
		SelectObject(m_hDc, GetFont(m_hDc));
		Analyse();
	}

	void Edit::OnLoad() {
		auto r = this->GetClientRect();
		RECT rcEdit{ r.X,r.X,r.GetRight(),r.GetBottom() };
		CreateEdit(_hWnd, rcEdit);
	}

	void Edit::UpCaret()
	{
		TEXTMETRIC tm;
		GetTextMetrics(m_hDc, &tm);
		m_uCaretHeight = tm.tmHeight;
		DestroyCaret();

		CreateCaret(m_hWnd, NULL, m_uCaretWidth, m_uCaretHeight);
		ShowCaret(m_hWnd);
		SetCaretPos(m_uCaretX, m_uCaretY);
	}

	HFONT Edit::GetFont(HDC hdc) {
		auto fontFamily = GetFontFamily();
		auto fontSize = GetFontSize();
		if (fontFamily != m_FontName || (int)fontSize != (int)m_FontSize) {
			m_FontName = fontFamily;
			m_FontSize = fontSize;
			if (hFont) {
				DeleteFont(hFont);
			}
			LOGFONT lf = { 0 };
			GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
			wcsncpy_s(lf.lfFaceName, m_FontName.utf16().c_str(), LF_FACESIZE);
			lf.lfCharSet = DEFAULT_CHARSET;
			lf.lfHeight = -MulDiv(m_FontSize, GetDeviceCaps(hdc, LOGPIXELSY), nDenominator);
			hFont = CreateFontIndirect(&lf);
			return hFont;
		}
		return hFont;
	}

	void Edit::Update(HDC hdc)
	{
		if (m_strBuff.length() > 0)
		{
			if (m_bIsCheck)
			{
				HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 128));
				FillRect(hdc, &m_rcCheck, hBrush);
				DeleteBrush(hBrush);
			}
			//UpCaret();

			SelectObject(hdc, GetFont(hdc));
			SetBkMode(hdc, TRANSPARENT);

			auto fColor = GetForeColor();
			SetTextColor(hdc, RGB(fColor.GetR(), fColor.GetG(), fColor.GetB()));

			auto rect = this->GetClientRect();
			m_nFontX = rect.X + 2;
			m_nFontY = rect.Y + 2;
			m_uCaretX = m_nFontX;
			m_uCaretY = m_nFontY;
			TextOut(hdc, m_nFontX, m_nFontY, m_strBuff.c_str(), m_strBuff.length());
		}
	}
	void Edit::OnPaint(PaintEventArgs& pe) {
		__super::OnPaint(pe);
	}
	void Edit::OnBackgroundPaint(PaintEventArgs& pe) {
		__super::OnBackgroundPaint(pe);
		Update(pe.Painter.DC);
	}

	void Edit::Analyse()
	{
		//分析字符串 字符串有变动时 必须重新分析
		if (m_Analysis) {
			ScriptStringFree(&m_Analysis);
		}
		SCRIPT_STATE			 ScriptState;
		SCRIPT_CONTROL		 ScriptControl;
		ZeroMemory(&ScriptState, sizeof(ScriptState));
		ZeroMemory(&ScriptControl, sizeof(ScriptControl));
		ScriptApplyDigitSubstitution(NULL, &ScriptControl, &ScriptState);

		HRESULT hr = ScriptStringAnalyse(m_hDc,
			m_strBuff.c_str(),
			m_strBuff.length() + 1,
			m_strBuff.length() * 3 / 2 + 16,
			-1,
			SSA_BREAK | SSA_GLYPHS | SSA_FALLBACK | SSA_LINK,
			0,
			&ScriptControl,
			&ScriptState,
			NULL,
			NULL,
			NULL,
			&m_Analysis);
	}
	void Edit::_SetText(LPCWSTR lpText)
	{
		if (lpText)
		{
			m_strBuff = lpText;
			Analyse();
		}
	}

	void Edit::SetEditVisible(bool bVisible)
	{
		m_bVisible = bVisible;
	}
	void Edit::SetEditState(EEditState eEditState)
	{
		m_eState = eEditState;
		Refresh();
	}


	void Edit::OnChar(WPARAM wParam, LPARAM lParam) {
		this->MyMessage(WM_CHAR, wParam, lParam);
	}
	void Edit::OnKeyDown(WPARAM wParam) {
		this->MyMessage(WM_KEYDOWN, wParam, NULL);
	}
	void Edit::OnMouseDown(MouseButton mbtn, const Point& pt) {
		__super::OnMouseDown(mbtn, pt);
		if (mbtn == MouseButton::Left) {
			down = true;
			this->MyMessage(WM_LBUTTONDOWN, NULL, MAKELPARAM(pt.X, pt.Y));
		}
	}
	void Edit::OnMouseUp(MouseButton mbtn, const Point& pt) {
		__super::OnMouseUp(mbtn, pt);
		down = false;
		if (mbtn == MouseButton::Left) {
			this->MyMessage(WM_LBUTTONUP, NULL, MAKELPARAM(pt.X, pt.Y));
		}
	}
	void Edit::OnMouseMove(const Point& pt) {
		__super::OnMouseMove(pt);
		this->MyMessage(WM_MOUSEMOVE, NULL, MAKELPARAM(pt.X, pt.Y));
	}

	void Edit::OnMouseLeave()
	{
		__super::OnMouseLeave();
		//DestroyCaret();
	}

	LRESULT Edit::MyMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_KEYDOWN:
		{
			if (m_eState != E_Edit_Focus)
				return S_FALSE;

			switch (wParam)
			{
			case VK_DELETE:
			{
				if (m_strBuff.length() <= 0 ||
					m_nCaret == m_strBuff.length())
					return S_FALSE;

				if (m_bIsCheck)
				{
					if (m_nCaret > m_nPrveCaret)
					{
						m_strBuff.erase(m_nPrveCaret, m_nCaret - m_nPrveCaret);
						m_nCaret = m_nPrveCaret;
					}
					else
					{
						m_strBuff.erase(m_nCaret, m_nPrveCaret - m_nCaret);
					}

					Analyse();
					int pX = 0;
					ScriptStringCPtoX(m_Analysis,
						m_nCaret,		//字符在字符串中的位置
						false,				//false表示字符前端 true表示后端
						&pX);

					m_uCaretX = pX + m_nFontX;
					SetCaretPos(m_uCaretX, m_uCaretY);

					m_bIsCheck = false;
				}
				else
					m_strBuff.erase(m_nCaret, 1);

				Analyse();
				Refresh();
				return S_OK;
			}
			case VK_LEFT:
			{
				if (m_strBuff.length() <= 0 ||
					m_nCaret <= 0)
					return S_OK;

				if (m_bIsCheck)
					m_bIsCheck = false;

				--m_nCaret;
				int pX = 0;
				ScriptStringCPtoX(m_Analysis, m_nCaret, false, &pX);

				m_uCaretX = pX + m_nFontX;
				SetCaretPos(m_uCaretX, m_uCaretY);
				return S_OK;
			}
			case VK_RIGHT:
			{
				if (m_strBuff.length() <= 0 ||
					m_nCaret >= m_strBuff.length())
					return S_OK;

				if (m_bIsCheck)
					m_bIsCheck = false;

				++m_nCaret;
				int pX = 0;
				ScriptStringCPtoX(m_Analysis, m_nCaret, false, &pX);

				m_uCaretX = pX + m_nFontX;
				SetCaretPos(m_uCaretX, m_uCaretY);
				return S_OK;
			}
			case VK_HOME:
			{
				if (m_strBuff.length() <= 0)
					return S_OK;

				if (m_bIsCheck)
					m_bIsCheck = false;

				m_nCaret = 0;
				int pX = 0;
				ScriptStringCPtoX(m_Analysis, m_nCaret, false, &pX);

				m_uCaretX = pX + m_nFontX;
				SetCaretPos(m_uCaretX, m_uCaretY);
				return S_OK;
			}
			case VK_END:
			{
				if (m_strBuff.length() <= 0)
					return S_OK;

				if (m_bIsCheck)
					m_bIsCheck = false;

				m_nCaret = m_strBuff.length();
				int pX = 0;
				ScriptStringCPtoX(m_Analysis, m_nCaret, false, &pX);

				m_uCaretX = pX + m_nFontX;
				SetCaretPos(m_uCaretX, m_uCaretY);
				return S_OK;
			}
			}
			break;
		}
		case WM_CHAR:
		{
			if (m_eState != E_Edit_Focus)
				return S_FALSE;

			switch (wParam)
			{
			case 22:	//Ctrl + v
			{
				//只接收文本
				if (!IsClipboardFormatAvailable(CF_TEXT))
					break;

				//打开剪贴版
				if (!OpenClipboard(m_hWnd))
					break;

				//获取剪贴板数据
				HANDLE hClipboard = GetClipboardData(CF_TEXT);
				std::string tmp = (CHAR*)GlobalLock(hClipboard);

				StdString szBuff(tmp.length() * 3, ' ');;
				int size = MultiByteToWideChar(CP_ACP, 0, tmp.c_str(), -1, &szBuff[0], szBuff.length());
				szBuff.resize(size - 1);

				int len = szBuff.length();

				if (!szBuff.empty())
				{
					if (m_bIsCheck)
					{
						if (m_nCaret > m_nPrveCaret)
						{
							m_strBuff.erase(m_nPrveCaret, m_nCaret - m_nPrveCaret);
							m_nCaret = m_nPrveCaret;
						}
						else
						{
							m_strBuff.erase(m_nCaret, m_nPrveCaret - m_nCaret);
						}

						Analyse();
						int pX = 0;
						ScriptStringCPtoX(m_Analysis, m_nCaret, false, &pX);

						m_uCaretX = pX + m_nFontX;
						SetCaretPos(m_uCaretX, m_uCaretY);

						m_bIsCheck = false;
					}

					if (m_strBuff.length() == m_nCaret)
					{
						m_strBuff += szBuff;
						m_nCaret += len;
						Analyse();
						int pX = 0;
						ScriptStringCPtoX(m_Analysis, m_nCaret, false, &pX);

						m_uCaretX = pX + m_nFontX;
						SetCaretPos(m_uCaretX, m_uCaretY);
						Refresh();
					}
					else
					{
						m_strBuff.insert(m_nCaret, szBuff);
						m_nCaret += len;
						Analyse();
						int pX = 0;
						ScriptStringCPtoX(m_Analysis, m_nCaret, false, &pX);

						m_uCaretX = pX + m_nFontX;
						SetCaretPos(m_uCaretX, m_uCaretY);
						Refresh();
					}

					//解锁
					GlobalUnlock(hClipboard);
				}
				CloseClipboard();

				return S_OK;
			}
			case 3:		//Ctrl+c
			{
				if (m_strBuff.length() <= 0)
					return S_FALSE;

				std::wstring tmp;
				if (m_bIsCheck)
				{
					if (m_nCaret > m_nPrveCaret)
						tmp = m_strBuff.substr(m_nPrveCaret, m_nCaret - m_nPrveCaret);
					else
						tmp = m_strBuff.substr(m_nCaret, m_nPrveCaret - m_nCaret);
				}
				else
					tmp = m_strBuff;

				//打开剪贴板
				if (!OpenClipboard(m_hWnd))
					break;

				//清空剪贴板
				EmptyClipboard();

				std::string str(tmp.length() * 3, ' ');
				int size = WideCharToMultiByte(CP_OEMCP, 0, tmp.c_str(), -1, &str[0], str.length(), NULL, NULL);
				str.resize(size);

				//分配内存	
				HGLOBAL clipbuff = GlobalAlloc(GMEM_DDESHARE, (str.length() + 1));
				char* buffer = (char*)GlobalLock(clipbuff);

				memcpy(buffer, str.c_str(), (str.length() + 1));
				GlobalUnlock(clipbuff);

				SetClipboardData(CF_TEXT, clipbuff);
				CloseClipboard();

				return S_OK;
			}
			case 13:	//回车
			case 26:	// Ctrl Z
			case 2:		// Ctrl B
			case 14:	// Ctrl N
			case 19:	// Ctrl S
			case 4:		// Ctrl D
			case 6:		// Ctrl F
			case 7:		// Ctrl G
			case 10:	// Ctrl J 换行
			case 11:	// Ctrl K
			case 12:	// Ctrl L
			case 17:	// Ctrl Q
			case 23:	// Ctrl W
			case 5:		// Ctrl E
			case 18:	// Ctrl R
			case 20:	// Ctrl T
			case 25:	// Ctrl Y
			case 21:	// Ctrl U
			case 9:		// Ctrl I
			case 15:	// Ctrl O
			case 16:	// Ctrl P
			case 27:	// Ctrl [
			case 29:	// Ctrl ]
			case 28:	// Ctrl \ 
				break;
			case 8:		//退格
			{
				if (m_strBuff.length() <= 0)
					return S_FALSE;

				if (m_bIsCheck)
				{
					if (m_nCaret > m_nPrveCaret)
					{
						m_strBuff.erase(m_nPrveCaret, m_nCaret - m_nPrveCaret);
						m_nCaret = m_nPrveCaret;
					}
					else
					{
						m_strBuff.erase(m_nCaret, m_nPrveCaret - m_nCaret);
					}

					m_bIsCheck = false;
				}
				else if (m_nCaret != 0)
				{
					if (m_nCaret == m_strBuff.length())
						m_strBuff.pop_back();
					else
						m_strBuff.erase(m_nCaret - 1, 1);

					--m_nCaret;
				}

				Analyse();
				int pX = 0;
				ScriptStringCPtoX(m_Analysis,
					m_nCaret,		//字符在字符串中的位置
					false,				//false表示字符前端 true表示后端
					&pX);

				m_uCaretX = pX + m_nFontX;
				SetCaretPos(m_uCaretX, m_uCaretY);
				Refresh();
				return S_OK;
			}
			default:
			{
				if (m_bIsCheck)
				{
					if (m_nCaret > m_nPrveCaret)
					{
						m_strBuff.erase(m_nPrveCaret, m_nCaret - m_nPrveCaret);
						m_nCaret = m_nPrveCaret;
					}
					else
					{
						m_strBuff.erase(m_nCaret, m_nPrveCaret - m_nCaret);
					}

					Analyse();
					int pX = 0;
					ScriptStringCPtoX(m_Analysis,
						m_nCaret,		//字符在字符串中的位置
						false,				//false表示字符前端 true表示后端
						&pX);

					m_uCaretX = pX + m_nFontX;
					SetCaretPos(m_uCaretX, m_uCaretY);

					m_bIsCheck = false;
				}

				if (m_strBuff.length() == m_nCaret)
				{
					m_strBuff += (TCHAR)wParam;
					++m_nCaret;
					Analyse();
					int pX = 0;
					ScriptStringCPtoX(m_Analysis,
						m_nCaret,
						false,
						&pX);

					m_uCaretX = pX + m_nFontX;
					SetCaretPos(m_uCaretX, m_uCaretY);
					Refresh();
				}
				else
				{
					m_strBuff.insert(m_nCaret, 1, (TCHAR)wParam);
					++m_nCaret;
					Analyse();
					int pX = 0;
					ScriptStringCPtoX(m_Analysis,
						m_nCaret,
						false,
						&pX);

					m_uCaretX = pX + m_nFontX;
					SetCaretPos(m_uCaretX, m_uCaretY);
					Refresh();
				}

				return S_OK;
			}
			}
			break;
		}
		case WM_LBUTTONDOWN:
		{
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);



			auto r = GetClientRect();
			pt.x += r.X;
			pt.y += r.Y;

			m_rcBound.left = r.X;
			m_rcBound.top = r.Y;
			m_rcBound.right = r.GetRight();
			m_rcBound.bottom = r.GetBottom();


			if (PtInRect(&m_rcBound, pt))
			{
				SetCursor(LoadCursor(NULL, IDC_IBEAM));

				if (m_strBuff.length() > 0)
				{
					size_t nX = pt.x, pCP = 0, pnTrail = 0;
					nX -= m_nFontX;
					ScriptStringXtoCP(m_Analysis, nX, (int*)&pCP, (int*)pnTrail);

					if (pnTrail)
						++pCP;
					if (pCP > m_strBuff.length())
						pCP = m_strBuff.length();

					m_nCaret = pCP;
					m_nPrveCaret = m_nCaret;

					int pX = 0;
					ScriptStringCPtoX(m_Analysis, m_nCaret, false, &pX);

					m_uCaretX = pX + m_nFontX;
					SetCaretPos(m_uCaretX, m_uCaretY);

					SetCapture(m_hWnd);
					m_bDrag = true;

					m_bIsCheck = false;
					Refresh();
				}

				if (m_eState != E_Edit_Focus)
				{
					UpCaret();
					SetEditState(E_Edit_Focus);
				}
			}
			else
			{
				if (m_eState != E_Edit_Normal)
				{
					if (m_eState == E_Edit_Focus)
						DestroyCaret();

					if (m_bIsCheck)
						m_bIsCheck = false;

					SetEditState(E_Edit_Normal);
				}
			}

			break;
		}
		case WM_LBUTTONUP:
		{
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);

			if (m_bDrag)
			{
				ReleaseCapture();
				m_bDrag = false;
			}

			if (PtInRect(&m_rcBound, pt))
			{
				SetCursor(LoadCursor(NULL, IDC_IBEAM));

				if (m_eState != E_Edit_Focus)
				{
					SetEditState(E_Edit_Focus);
				}
				UpCaret();
				return S_OK;
			}
			else
			{

			}
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (!down) return 0;
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			auto r = GetClientRect();
			pt.x += r.X;
			pt.y += r.Y;
			if (m_bDrag && m_strBuff.length() > 0)
			{
				size_t nX = pt.x, pCP = 0, pnTrail = 0;
				nX -= m_nFontX;
				ScriptStringXtoCP(m_Analysis, nX, (int*)&pCP, (int*)&pnTrail);
				if (pnTrail)
					++pCP;
				if (pCP > m_strBuff.length())
					pCP = m_strBuff.length();
				m_nCaret = pCP;
				if (m_nCaret == m_nPrveCaret && m_bIsCheck)
				{
					m_bIsCheck = false;
					Refresh();
					return S_OK;
				}
				int pX = 0;
				ScriptStringCPtoX(m_Analysis, m_nCaret, false, &pX);
				m_uCaretX = pX + m_nFontX;
				SetCaretPos(m_uCaretX, m_uCaretY);

				pX = 0;
				ScriptStringCPtoX(m_Analysis, m_nPrveCaret, false, &pX);
				if (pX + m_nFontX > m_uCaretX)
				{
					m_rcCheck.left = m_uCaretX;
					m_rcCheck.right = pX + m_nFontX;
				}
				else
				{
					m_rcCheck.left = pX + m_nFontX;
					m_rcCheck.right = m_uCaretX;
				}
				m_rcCheck.top = m_uCaretY;
				m_rcCheck.bottom = m_uCaretY + m_uCaretHeight;
				m_bIsCheck = true;
				Refresh();
			}
			else if (PtInRect(&m_rcBound, pt))
			{
				if (m_eState == E_Edit_Normal)
					SetEditState(E_Edit_Over);
				SetCursor(LoadCursor(NULL, IDC_IBEAM));
				return S_OK;
			}
			else
			{
				if (m_eState == E_Edit_Over)
					SetEditState(E_Edit_Normal);
			}
			break;
		}
		}
		return S_FALSE;
	}
};