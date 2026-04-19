#pragma once
#include <functional>
#include <vector>
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
			int Width = 120;
			HAlign Align = HAlign::Left;
		};
	private:
		std::vector<Column> m_columns;
		std::vector<std::vector<UIString>> m_rows;
		std::vector<int> m_layoutColumnWidths;
		std::vector<int> m_layoutColumnOffsets;
		VScrollBar* m_vScrollBar = NULL;
		HScrollBar* m_hScrollBar = NULL;
		int m_totalColumnWidth = 0;
		int m_headerHeight = 36;
		int m_rowHeight = 32;
		int m_cellPadding = 10;
		int m_gridLineWidth = 1;
		int m_defaultColumnWidth = 120;
		int m_minColumnWidth = 48;
		int m_scrollX = 0;
		int m_scrollY = 0;
		int m_selectedRow = -1;
		int m_hoverRow = -1;
		bool m_headerVisible = true;
		UIString m_emptyText = "No data";
		Color m_gridColor = Color(220, 224, 231);
		Color m_headerBackColor = Color(245, 247, 250);
		Color m_headerForeColor = Color(45, 52, 60);
		Color m_rowBackColor = Color::White;
		Color m_altRowBackColor = Color(249, 250, 252);
		Color m_hoverRowBackColor = Color(241, 246, 255);
		Color m_selectedRowBackColor = Color(229, 241, 251);
		Color m_selectedRowForeColor = Color(24, 36, 50);
	private:
		void Init();
		void EnsureColumnCount(size_t count);
		void UpdateMetrics();
		int GetHeaderHeightInternal() const noexcept;
		Rect GetDataViewport() const noexcept;
		int HitTestRow(const Point& point) const;
		int HitTestColumn(const Point& point) const;
		Column MakeDefaultColumn(int index) const;
		void ApplyHeaders(const UIString& value);
		void ApplyColumnWidths(const UIString& value);
		void ApplyColumnAligns(const UIString& value);
		void ApplyData(const UIString& value);
	protected:
		virtual void OnLayout() override;
		virtual void OnScroll(int offsetX, int offsetY) override;
		virtual void OnForePaint(PaintEventArgs* args) override;
		virtual void OnChildPaint(PaintEventArgs* args) override;
		virtual void OnMouseDown(MouseEventArgs* args) override;
		virtual void OnMouseMove(MouseEventArgs* args) override;
		virtual void OnMouseLeave(MouseEventArgs* args) override;
		virtual void OnDpiChanged(DpiChangedEventArgs* args) override;
	public:
		std::function<void(DataGridView* sender, int rowIndex)> SelectedRowChanged = NULL;
		std::function<void(DataGridView* sender, int rowIndex, int columnIndex)> CellClick = NULL;

		DataGridView(Object* ownerObject = NULL);
		virtual ~DataGridView();

		virtual VScrollBar* GetVScrollBar() override;
		virtual HScrollBar* GetHScrollBar() override;
		virtual void RemoveAll(bool freeAll = false) override;

		void ClearColumns();
		int AddColumn(const Column& column);
		int AddColumn(const UIString& headerText, int width = 120, const UIString& name = "", HAlign align = HAlign::Left);
		int GetColumnCount() const noexcept;
		Column* GetColumn(int index);
		const Column* GetColumn(int index) const;

		void RemoveAllRows();
		void SetRows(const std::vector<std::vector<UIString>>& rows);
		int AddRow(const std::vector<UIString>& values);
		int GetRowCount() const noexcept;
		const std::vector<UIString>* GetRow(int index) const;

		void SetCellText(int rowIndex, int columnIndex, const UIString& text);
		UIString GetCellText(int rowIndex, int columnIndex) const;

		void SetHeaderVisible(bool visible);
		void SetHeaderHeight(int headerHeight);
		void SetRowHeight(int rowHeight);
		void SetCellPadding(int cellPadding);
		void SetGridLineWidth(int gridLineWidth);
		void SetSelectedRow(int rowIndex);
		int GetSelectedRow() const noexcept;
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
		virtual void SetAttribute(const UIString& key, const UIString& value) override;
	};
};
