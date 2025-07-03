#include "IFrame.h"
namespace ezui {
	IFrame::IFrame()
	{
	}
	IFrame::~IFrame()
	{
	}
	UIManager* IFrame::GetUIManager()
	{
		return &_umg;
	}
	void IFrame::SetAttribute(const UIString& attrName, const UIString& attrValue) {
		if (attrName == "src") {
			_umg.LoadXmlFile(attrValue);
			_umg.SetupUI(this);
			if (this->GetControls().size() > 0) {
				Control* root = this->GetControl(this->GetControls().size() - 1);
				root->SetDockStyle(DockStyle::Fill);
			}
		}
		__super::SetAttribute(attrName, attrValue);
	};
}