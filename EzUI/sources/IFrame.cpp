#include "IFrame.h"
namespace EzUI {
	IFrame::IFrame()
	{
	}
	void IFrame::SetAttribute(const EString& attrName, const EString& attrValue)
	{
		/*if (attrName == "src") {
			umg.LoadFile(attrValue);
			umg.SetupUI(this);
		}*/
		__super::SetAttribute(attrName, attrValue);
	}
	IFrame::~IFrame()
	{
	}
};