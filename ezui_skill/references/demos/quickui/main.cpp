#include "EzUI/EzUI.h"

using namespace ezui;

class MainForm final : public Window {
public:
	MainForm()
	{
		SetSize(900, 560);
		LoadXml("res/mainForm.htm");

		if (auto* search = FindControl<TextBox>("search")) {
			search->TextChanged = [this](TextBox*, const UIString& text) {
				FilterNavigation(text);
			};
		}
	}

	void OnClose(bool& allowClose) override
	{
		Application::Exit(0);
	}

private:
	void FilterNavigation(const UIString& query)
	{
		auto* navigation = FindControl<VListView>("navigation");
		auto* pages = FindControl<TabControl>("pages");
		if (!navigation || !pages) {
			return;
		}

		const UIString needle = query.trim().toLower();
		RadioButton* firstVisible = nullptr;
		int firstVisibleIndex = -1;
		bool hasCheckedVisible = false;
		int pageIndex = 0;

		for (auto* child : navigation->GetChildren()) {
			auto* item = dynamic_cast<RadioButton*>(child);
			if (!item) {
				continue;
			}

			const bool visible =
				needle.empty() || item->GetText().toLower().contains(needle);
			item->SetVisible(visible);

			if (visible && !firstVisible) {
				firstVisible = item;
				firstVisibleIndex = pageIndex;
			}
			if (visible && item->GetCheck()) {
				hasCheckedVisible = true;
			}
			if (!visible && item->GetCheck()) {
				item->SetCheck(false);
			}
			++pageIndex;
		}

		if (!hasCheckedVisible && firstVisible) {
			firstVisible->SetCheck(true);
			pages->SetPageIndex(firstVisibleIndex);
		}

		navigation->RefreshLayout();
		navigation->Invalidate();
		pages->Invalidate();
	}
};

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int)
{
	Application app(hInstance);
	app.EnableHighDpi();

	MainForm window;
	window.CenterToScreen();
	window.Show();
	return app.Exec();
}
