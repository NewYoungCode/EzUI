#pragma once
#include "Control.h"
#include "Timer.h"
#include <usp10.h>
#pragma comment (lib, "usp10.lib")
namespace EzUI {
	class UI_EXPORT Edit :
		public Control
	{
	private:
		std::wstring buf;//文字缓冲
		bool _focus = false;//是否具有焦点中
		SCRIPT_STRING_ANALYSIS _Analysis;	//字符串分析结果
		void Analysis();
	protected:
		Point point_Start;
		Point point_End;
		Rect selectRect;//选中的字符矩形
		Rect careRect;//光标位置
		int FontHeight = 0;
		DxTextLayout* textLayout = NULL;
		DxTextFormat* textFormat = NULL;
	protected:
		virtual void OnSize(const Size& size)override;
		virtual void OnForePaint(PaintEventArgs& e) override;
		virtual void OnChar(WPARAM wParam, LPARAM lParam) override;
		virtual void OnKeyDown(WPARAM wParam)override;
		virtual void OnMouseDown(MouseButton mbtn, const Point& point)override;
		virtual void OnMouseMove(const Point& point) override;
		virtual void OnMouseUp(MouseButton mbtn, const Point& point)override;
		virtual void OnKillFocus() override;
	public:
		void SetText(const EString& text);
		EString GetText();
		//virtual ~Edit();
	};
};
