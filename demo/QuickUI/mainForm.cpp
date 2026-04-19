#include "mainForm.h"

MainForm::MainForm() : Window()
{
	LoadXml("res/mainForm.htm");
	SetMinSize(Size(1180, 760));

	if (auto* searchBox = FindControl<TextBox>("search")) {
		searchBox->TextChanged = [this](TextBox*, const UIString& text) {
			ApplySearchFilter(text);
			};
	}

	ApplySearchFilter("");
}

void MainForm::ApplySearchFilter(const UIString& query)
{
	auto* navList = FindControl<VListView>("navList");
	auto* tab = FindControl<TabControl>("tabMain");
	if (!navList) {
		return;
	}

	UIString needle = query.trim().toLower();
	int totalCount = 0;
	int visibleCount = 0;
	int radioIndex = -1;
	int checkedVisibleIndex = -1;
	int firstVisibleIndex = -1;

	RadioButton* firstVisible = nullptr;
	RadioButton* checkedVisible = nullptr;

	for (auto* child : navList->GetChildren()) {
		auto* radio = dynamic_cast<RadioButton*>(child);
		if (!radio) {
			continue;
		}

		++radioIndex;
		++totalCount;

		const bool match = needle.empty() || radio->GetText().toLower().contains(needle);
		radio->SetVisible(match);

		if (!match && radio->GetCheck()) {
			radio->SetCheck(false);
		}

		if (match) {
			++visibleCount;
			if (!firstVisible) {
				firstVisible = radio;
				firstVisibleIndex = radioIndex;
			}
			if (radio->GetCheck()) {
				checkedVisible = radio;
				checkedVisibleIndex = radioIndex;
			}
		}
	}

	if (!checkedVisible && firstVisible) {
		for (auto* child : navList->GetChildren()) {
			auto* radio = dynamic_cast<RadioButton*>(child);
			if (radio) {
				radio->SetCheck(radio == firstVisible);
			}
		}
		if (tab && firstVisibleIndex >= 0) {
			tab->SetPageIndex(firstVisibleIndex);
			tab->RefreshLayout();
			tab->Invalidate();
		}
	}
	else if (checkedVisible && tab && checkedVisibleIndex >= 0) {
		tab->SetPageIndex(checkedVisibleIndex);
		tab->RefreshLayout();
		tab->Invalidate();
	}

	navList->RefreshLayout();
	navList->Invalidate();
	if (auto* layout = GetLayout()) {
		layout->RefreshLayout();
		layout->Invalidate();
	}

	UpdateSearchMeta(visibleCount, totalCount, needle);
}

void MainForm::UpdateSearchMeta(int visibleCount, int totalCount, const UIString& query)
{
	auto* meta = FindControl<Label>("searchMeta");
	if (!meta) {
		return;
	}

	if (query.empty()) {
		meta->SetText(std::to_string(totalCount) + UIString(L"个控件 · 基于头文件补全文档"));
		return;
	}

	if (visibleCount <= 0) {
		meta->SetText(UIString(L"没有匹配项 · 试试 Button / Layout / View"));
		return;
	}

	UIString text = UIString(L"匹配") + std::to_string(visibleCount) + "/" + std::to_string(totalCount) + UIString(L"关键字") + query;
}

void MainForm::OnNotify(Control* sender, EventArgs* args)
{
	__super::OnNotify(sender, args);
}

void MainForm::OnClose(bool& bClose)
{
	Application::Exit(0);
}

MainForm::~MainForm()
{
}


