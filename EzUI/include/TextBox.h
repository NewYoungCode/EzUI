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
		std::wstring text;//文字
		Size _fontBox;
		bool _down = false;//是否具有焦点中
		bool _focus = false;//是否具有焦点中
		Point point_Start;//开始选中的位置
		Point point_End;//结束位置
		std::list<Rect> selectRects;//选中的字符矩形
		Rect careRect;//光标位置
		Font* font = NULL;//字体
		TextLayout* textLayout = NULL;//字体布局
		Point A;//A点
		BOOL A_isTrailingHit;//如果是1表示是字符的后半边
		int A_TextPos = 0;//点击了第几个字符
		Point B;//B点
		BOOL B_isTrailingHit;//如果是1表示是字符的后半边
		int B_TextPos = 0;//点击了第几个字符
		int TextPos = 0;//当前文字的下标 0~text.size()
		int scrollX = 0;//用于左右滚动
		int scrollY = 0;//用于y轴滚动
		int lastX = 0;//上一个x位置
		int lastY = 0;//上一个y位置 
		Windows::Timer timer;//用于光标闪烁
		bool _careShow = false;//用于光标闪烁
		double sliderY = 0;
	private:
		void Init();
		void Analysis();//分析字符串
		void Insert(const std::wstring& str);//插入文字
		bool DeleteRange();//删除选中内容
		bool GetSelectedRange(int* outPos, int* outCount);//获取当前被选中的区域 返回下标和个数
		bool Copy();//复制到剪切板
		bool Paste();//粘贴
		bool SelectedAll();//全选
		void OnBackspace();//退格键要做的事
		void BuildCare();//构建光标
		Point ConvertPoint(const Point& pt);//坐标转换
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
		EString Placeholder;//placeholder懂得都懂 (在没有文字的情况下显示的文字)
		std::wstring PasswordChar;
		Color SelectedColor = Color(100, 255, 0, 0);//选中颜色
		bool ReadOnly = false;//是否只读
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
