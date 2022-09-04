//#pragma comment(lib, "UxTheme")
//#include <windowsx.h>
//#include <windows.h>
//#include <uxtheme.h>
//#include <iostream>
//#include "global.h"
//
//LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
//void setA(const Rect& clipRect, EBitmap* _bufBitmap, int x, int y, BYTE a) {
//	if (clipRect.Contains(x, y)) { //不允许绘制在OWner窗口区域
//		return;
//	}
//	DWORD* point = (DWORD*)_bufBitmap->point + (x + y * _bufBitmap->Width);//起始地址+坐标偏移
//	((BYTE*)point)[3] = a;//修改A通道数值
//}
//bool SetShadow(const Rect& clipRect, EBitmap* _bufBitmap, int m_Width, int m_Height, size_t iSize) {
//	int width = m_Width < m_Height ? m_Width : m_Height;
//	int radius = 3;//半径
//	int max_size = width / 2 - radius;
//	if (max_size <= 0) {
//		radius = 0;
//		max_size = width / 2;
//	}
//	iSize = (int)iSize < max_size ? iSize : max_size;
//	double piAngle = 3.1415926;
//	int iSizeB = 4 * iSize;
//	double fN = piAngle / iSize / 4.1;//设置四条边外模糊度
//	double lN = 1.0 / iSize;
//	int iAplpha = 0;
//	int Left = iSize + radius,
//		Top = iSize + radius,
//		Right = m_Width - iSize - radius,
//		Bottom = m_Height - iSize - radius;
//	int x = 0, y = 0;
//	for (size_t i = 0; i < iSize; i++)
//	{
//		iAplpha = int(255 - cos(fN * (i)) * 255);
//		for (x = Left; x <= Right; x++)
//		{
//			setA(clipRect,_bufBitmap, x, i, iAplpha);
//			setA(clipRect, _bufBitmap,x, m_Height - i - 1, iAplpha);
//		}
//		for (y = Top; y <= Bottom; y++)
//		{
//			setA(clipRect, _bufBitmap, i, y, iAplpha);
//			setA(clipRect, _bufBitmap, m_Width - i - 1, y, iAplpha);
//		}
//	}
//	double fL = 0;
//	int iSizeR = iSize + radius;
//	for (int y = 0; y < iSizeR; y++)
//	{
//		for (int x = y; x < iSizeR; x++)
//		{
//			fL = sqrt((iSizeR - x) * (iSizeR - x) + (iSizeR - y) * (iSizeR - y));
//			if (fL <= radius) {
//				break;
//			}
//			if (fL > radius && fL < iSizeR) {
//				iAplpha = int(255 - cos(fN * (iSizeR - fL)) * 255);
//			}
//			else {
//				iAplpha = 0;
//			}
//			setA(clipRect, _bufBitmap, x, y, iAplpha);
//			setA(clipRect, _bufBitmap, y, x, iAplpha);
//			setA(clipRect, _bufBitmap, m_Width - x - 1, y, iAplpha);
//			setA(clipRect, _bufBitmap, m_Width - y - 1, x, iAplpha);
//			setA(clipRect, _bufBitmap, x, m_Height - y - 1, iAplpha);
//			setA(clipRect, _bufBitmap, y, m_Height - x - 1, iAplpha);
//			setA(clipRect, _bufBitmap, m_Width - x - 1, m_Height - y - 1, iAplpha);
//			setA(clipRect, _bufBitmap, m_Width - y - 1, m_Height - x - 1, iAplpha);
//		}
//	}
//	return true;
//}
//
//
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//{
//
//	Application app;
//
//	WNDCLASSEX wcex;
//	wcex.cbSize = sizeof(WNDCLASSEX);
//	wcex.style = CS_HREDRAW | CS_VREDRAW;
//	wcex.lpfnWndProc = WndProc;
//	wcex.cbClsExtra = 0;
//	wcex.cbWndExtra = 0;
//	wcex.hInstance = hInstance;
//	wcex.hIcon = NULL;
//	wcex.hCursor = (HICON)LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground = NULL;
//	wcex.lpszMenuName = NULL;
//	wcex.lpszClassName = L"window";
//	wcex.hIconSm = NULL;
//
//	RegisterClassEx(&wcex);
//
//	HWND hWnd = CreateWindowEx(
//		NULL,
//		L"window",
//		NULL,
//		WS_POPUP | WS_MINIMIZEBOX,
//		100,
//		100,
//		600,
//		400,
//		NULL,
//		NULL,
//		hInstance,
//		NULL);
//
//	ShowWindow(hWnd, nCmdShow);
//
//	MSG msg;
//	while (GetMessage(&msg, NULL, 0, 0)) {
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//	}
//	return static_cast<int>(msg.wParam);
//}
//
//LRESULT __ZoomWindow(const HWND& _hWnd, const  LPARAM& lParam) {
//	RECT rc;
//	GetWindowRect(_hWnd, &rc);
//	POINT pt{ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
//	int x = 4;//
//	if (pt.x < rc.left + x)
//	{
//		if (pt.y < rc.top + x)return HTTOPLEFT;//
//		if (pt.y >= rc.bottom - x)return HTBOTTOMLEFT;//
//		return HTLEFT;//
//	}
//	if (pt.x >= rc.right - x)//
//	{
//		if (pt.y < rc.top + x)return HTTOPRIGHT;//
//		if (pt.y >= rc.bottom - x)return HTBOTTOMRIGHT;//
//		return HTRIGHT;//
//	}
//	if (pt.y < rc.top + x)return HTTOP;//
//	if (pt.y >= rc.bottom - x)return HTBOTTOM;//
//	return HTCLIENT;//ָ
//}
//LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	switch (uMsg) {
//	case WM_CREATE:
//		SetWindowTheme(hWnd, L"", L"");
//		return 0;
//	case WM_DESTROY:
//		PostQuitMessage(0);
//		return 0;
//
//	case WM_NCHITTEST: {
//		if (!::IsZoomed(hWnd)) {
//			return __ZoomWindow(hWnd, lParam);
//		}
//		break;
//	}
//	case WM_LBUTTONDOWN: {
//
//		POINT pt;
//		pt.x = GET_X_LPARAM(lParam);
//		pt.y = GET_Y_LPARAM(lParam);
//
//		RECT rect;
//		::GetClientRect(hWnd, &rect);
//		char buf[256]{ 0 };
//		sprintf_s(buf, "%d %d \n %d %d %d %d \n", pt.x, pt.y, rect.left, rect.top, rect.right, rect.bottom);
//		OutputDebugStringA(buf);
//		break;
//	}
//	case WM_NCCALCSIZE:
//	{
//		LPNCCALCSIZE_PARAMS ncParams = (LPNCCALCSIZE_PARAMS)lParam;
//		int borderWidth = 4;
//		ncParams->rgrc[0].top += borderWidth;
//		ncParams->rgrc[0].left += borderWidth;
//		ncParams->rgrc[0].bottom -= borderWidth;
//		ncParams->rgrc[0].right -= borderWidth;
//		return 0;
//		break;
//	}
//	case WM_ERASEBKGND: {
//		return TRUE;
//	}
//	case WM_PAINT:
//	{
//		PAINTSTRUCT pst;
//		BeginPaint(hWnd, &pst);
//		RECT& r = pst.rcPaint;
//		RECT rect2;
//		::GetClientRect(hWnd, &rect2);
//
//		RECT rect;
//		::GetClientRect(hWnd, &rect);
//		char buf[256]{ 0 };
//		sprintf_s(buf, " %d %d %d %d \n",  rect.left, rect.top, rect.right, rect.bottom);
//		OutputDebugStringA(buf);
//
//		Painter pt(pst.hdc);
//		pt.FillRectangle(Color::Pink, rect);
//		EndPaint(hWnd, &pst);
//		return 1;
//	}
//	case WM_NCPAINT:
//	{
//		RECT rect;
//		GetWindowRect(hWnd, &rect);
//		RECT rect2;
//		::GetClientRect(hWnd, &rect2);
//
//		HDC hdc;
//		hdc = GetDCEx(hWnd, (HRGN)wParam, DCX_WINDOW | DCX_INTERSECTRGN);
//		// Paint into this DC 
//		Painter pt(hdc);
//		pt.FillRectangle(Color::Gray, rect);
//
//		ReleaseDC(hWnd, hdc);
//		return 0;
//	}
//	case WM_NCACTIVATE:
//		RedrawWindow(hWnd, NULL, NULL, RDW_UPDATENOW);
//		return 0;
//		break;
//	}
//	return DefWindowProc(hWnd, uMsg, wParam, lParam);
//
//}