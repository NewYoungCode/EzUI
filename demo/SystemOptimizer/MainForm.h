#pragma once

#include "EzUI/EzUI.h"

#include <memory>
#include <vector>

class MainForm : public ezui::BorderlessWindow {
public:
    enum class OptimizationId {
        GameBar,
        ThisPc,
        Hibernate,
        RedirectCache,
        ClassicContextMenu
    };

    MainForm();
    ~MainForm() override;

protected:
    void OnClose(bool& allowClose) override;

private:
    struct OptimizationItem {
        OptimizationId id;
        ezui::UIString checkBoxName;
        ezui::UIString title;
    };

    void BindControls();
    void UpdateRedirectInputState();
    void UpdateSelectionSummary();
    void ApplySelected();
    void SetBusy(bool busy);
    void SetLog(const ezui::UIString& text);
    void SetSummaryText(const ezui::UIString& text);
    bool IsChecked(const ezui::UIString& checkBoxName) const;
    std::vector<OptimizationId> GetSelectedItems() const;

private:
    ezui::Button* applyButton_;
    ezui::TextBox* redirectInput_;
    ezui::TextBox* logBox_;
    ezui::Label* summaryLabel_;
    ezui::Label* statusPill_;
    std::vector<OptimizationItem> items_;
    std::unique_ptr<ezui::Thread> worker_;
    bool busy_;
};
