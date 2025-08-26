#include "IFrame.h"
namespace ezui {
	IFrame::IFrame(Object* parentObject) :Control(parentObject)
	{
	}
	IFrame::~IFrame()
	{
	}
	UIManager* IFrame::GetUIManager()
	{
		return &m_umg;
	}
	void IFrame::SetAttribute(const UIString& attrName, const UIString& attrValue) {
		if (attrName == "src") {
			LoadXml(attrValue);
		}
		__super::SetAttribute(attrName, attrValue);
	};
	void IFrame::LoadXml(const UIString& fileName) {
		m_umg.LoadXml(fileName);
		m_umg.SetupUI(this);
		if (this->GetControls().size() > 0) {
			Control* root = this->GetControl(this->GetControls().size() - 1);
			root->SetDockStyle(DockStyle::Fill);
		}
	}
	Control* IFrame::Add(Control* childCtrl)
	{
		//IFrame下只允许有一个控件并且会随着IFrame拉伸
		this->Clear();
		auto* ctrl = __super::Add(childCtrl);
		childCtrl->SetDockStyle(DockStyle::Fill);
		return ctrl;
	}
	void IFrame::Remove(Control* childCtl, bool freeCtrl)
	{
		__super::Remove(childCtl, freeCtrl);
	}
	void IFrame::SetLayout(Control* ctrl) {
		this->Add(ctrl);
	}
	bool IFrame::OnNotify(Control* sender, EventArgs& args) {
		return false;
	}
}