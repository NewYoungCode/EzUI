#pragma once
#include "Control.h"
#include "VScrollBar.h"
#include "HScrollBar.h"

namespace ezui {
	//数据表格控件(AI生成 请谨慎使用)
	class EZUI_API DataGridView : public Control {
	public:
		struct Column {
			UIString Name;
			UIString HeaderText;
			int Width;
			HAlign Align;
			Column() : Width(120), Align(HAlign::Left) {}
		};
	private:
		std::vector<Column> m_columns;
		std::vector<std::vector<UIString>> m_rows;
		std::vector<int> m_layoutColumnWidths;
		std::vector<int> m_layoutColumnOffsets;
		VScrollBar* m_vScrollBar;
		HScrollBar* m_hScrollBar;
		int m_totalColumnWidth;
		int m_headerHeight;
		int m_rowHeight;
		int m_cellPadding;
		int m_gridLineWidth;
		int m_defaultColumnWidth;
		int m_minColumnWidth;
		int m_scrollX;
		int m_scrollY;
		int m_selectedRow;
		int m_hoverRow;
		bool m_headerVisible;
		UIString m_emptyText;
		Color m_gridColor;
		Color m_headerBackColor;
		Color m_headerForeColor;
		Color m_rowBackColor;
		Color m_altRowBackColor;
		Color m_hoverRowBackColor;
		Color m_selectedRowBackColor;
		Color m_selectedRowForeColor;
	private:
		void Init();
		void EnsureColumnCount(size_t count);
		void UpdateMetrics();
		int GetHeaderHeightInternal() const EZUI_NOEXCEPT;
		Rect GetDataViewport() const EZUI_NOEXCEPT;
		int HitTestRow(const Point& point) const;
		int HitTestColumn(const Point& point) const;
		Column MakeDefaultColumn(int index) const;
		void ApplyHeaders(const UIString& value);
		void ApplyColumnWidths(const UIString& value);
		void ApplyColumnAligns(const UIString& value);
		void ApplyData(const UIString& value);
	protected:
		virtual void OnLayout() EZUI_OVERRIDE;
		virtual void OnScroll(int offsetX, int offsetY) EZUI_OVERRIDE;
		virtual void OnForePaint(PaintEventArgs* args) EZUI_OVERRIDE;
		virtual void OnChildPaint(PaintEventArgs* args) EZUI_OVERRIDE;
		virtual void OnMouseDown(MouseEventArgs* args) EZUI_OVERRIDE;
		virtual void OnMouseMove(MouseEventArgs* args) EZUI_OVERRIDE;
		virtual void OnMouseLeave(MouseEventArgs* args) EZUI_OVERRIDE;
		virtual void OnDpiChanged(DpiChangedEventArgs* args) EZUI_OVERRIDE;
	public:
		std::function<void(DataGridView* sender, int rowIndex)> SelectedRowChanged;
		std::function<void(DataGridView* sender, int rowIndex, int columnIndex)> CellClick;

		DataGridView(Object* ownerObject = NULL);
		virtual ~DataGridView();

		virtual VScrollBar* GetVScrollBar() EZUI_OVERRIDE;
		virtual HScrollBar* GetHScrollBar() EZUI_OVERRIDE;
		/// 移除全部子控件；deleteChildren 为 true 时同时立即删除这些控件。
		virtual void RemoveAll(bool deleteChildren = false) EZUI_OVERRIDE;

		void ClearColumns();
		int AddColumn(const Column& column);
		int AddColumn(const UIString& headerText, int width = 120, const UIString& name = "", HAlign align = HAlign::Left);
		int GetColumnCount() const EZUI_NOEXCEPT;
		Column* GetColumn(int index);
		const Column* GetColumn(int index) const;

		void RemoveAllRows();
		void SetRows(const std::vector<std::vector<UIString>>& rows);
		int AddRow(const std::vector<UIString>& values);
		int GetRowCount() const EZUI_NOEXCEPT;
		const std::vector<UIString>* GetRow(int index) const;

		void SetCellText(int rowIndex, int columnIndex, const UIString& text);
		UIString GetCellText(int rowIndex, int columnIndex) const;

		void SetHeaderVisible(bool visible);
		void SetHeaderHeight(int headerHeight);
		void SetRowHeight(int rowHeight);
		void SetCellPadding(int cellPadding);
		void SetGridLineWidth(int gridLineWidth);
		void SetSelectedRow(int rowIndex);
		int GetSelectedRow() const EZUI_NOEXCEPT;
		void ScrollToRow(int rowIndex);
		void ScrollToColumn(int columnIndex);
		void SetEmptyText(const UIString& text);

		// Supported attributes:
		// columns | headers: "姓名,部门,状态"
		// column-widths: "120,160,100"
		// column-aligns: "left,left,center"
		// data | rows: "张三|研发|在线; 李四|设计|离线"
		// show-header | header-visible: true | false
		// header-height: number
		// row-height: number
		// cell-padding: number
		// grid-line-width: number
		// empty-text: string
		// grid-color / header-back-color / header-fore-color
		// row-back-color / alt-row-back-color / hover-row-back-color
		// selected-row-back-color / selected-row-fore-color
		// All Control attributes are also supported
		virtual void SetAttribute(const UIString& key, const UIString& value) EZUI_OVERRIDE;
	};
};
