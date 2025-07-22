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
			m_umg.LoadXmlFile(attrValue);
			m_umg.SetupUI(this);
			if (this->GetControls().size() > 0) {
				Control* root = this->GetControl(this->GetControls().size() - 1);
				root->SetDockStyle(DockStyle::Fill);
			}
		}
		__super::SetAttribute(attrName, attrValue);
	};
	bool IFrame::OnNotify(Control* sender, EventArgs& args) {
		return false;
	}
}