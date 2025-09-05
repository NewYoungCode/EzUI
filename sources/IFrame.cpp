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
	}
	void IFrame::LoadXml(const char* fileData, size_t fileSize) {
		m_umg.LoadXml(fileData, fileSize);
		m_umg.SetupUI(this);
	}
	Control* IFrame::Add(Control* childCtrl)
	{
		//IFrame下只允许有一个控件并且会随着IFrame拉伸
		this->Clear();
		if (childCtrl) {
			auto* ctrl = __super::Add(childCtrl);
			childCtrl->SetDockStyle(DockStyle::Fill);
			return ctrl;
		}
		return NULL;
	}
	void IFrame::Remove(Control* childCtl, bool freeCtrl)
	{
		__super::Remove(childCtl, freeCtrl);
	}
	void IFrame::SetLayout(Control* ctrl) {
		this->Add(ctrl);
	}
	Control* IFrame::GetLayout() {
		return this->GetControl(0);
	}
	void IFrame::OnNotify(Control* sender, EventArgs& args) {
		if (this->NotifyHandler) {
			this->NotifyHandler(sender, args);
		}
		else {
			ezui::DefaultNotify(sender, args);
		}
	}
}