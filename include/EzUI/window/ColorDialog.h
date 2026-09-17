#pragma once
#include "PopupWindow.h"

namespace ezui {
	//颜色选择对话框 请使用ShowModal函数来显示对话框(失去焦点将自动关闭)
	class EZUI_API ColorDialog : public PopupWindow {
	private:
		class View;
		View* m_viewImpl;
	private:
		void SetColor(const Color& color);
		void SetSavedSwatches(std::vector<Color>* swatches);
	protected:
		virtual void OnShow() EZUI_OVERRIDE;
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam) EZUI_OVERRIDE;
	public:
		//颜色值发生更改时的回调函数
		std::function<void(const Color&)> ValueChanged;
	public:
		/**
		 * @brief 颜色选择对话框
		 * @param owner        父窗口句柄，默认为 NULL（无父窗口）
		 * @param initialColor 初始颜色，默认为 RGBA(36, 48, 187, 255)
		 * @param swatches     自定义色板颜色列表指针(最多自定义8个)，默认为 NULL（默认不使用自定义色板）
		 */
		explicit ColorDialog(WindowHandle owner = NULL, const Color& initialColor = Color(36, 48, 187, 255), std::vector<Color>* swatches = NULL);
		virtual ~ColorDialog();
		//获取最终颜色
		Color GetColor() const;
	};
};
