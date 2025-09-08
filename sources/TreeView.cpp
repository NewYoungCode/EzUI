#include "TreeView.h"
#include "Label.h"
namespace ezui {

	TreeView::TreeView(Object* parentObj) :HListView(parentObj)
	{
		m_vList.SetAutoWidth(true);
		m_vList.SetDockStyle(DockStyle::Vertical);
		this->AddChild(&m_vList);
	}

	void TreeView::AddNode(const UIString& nodeName)
	{
		Label* label = new Label(this);
		label->TextAlign = TextAlign::MiddleLeft;
		label->SetText(nodeName);
		label->SetAutoSize(true);
		m_vList.AddChild(label);
	}

	TreeView::~TreeView()
	{

	}
}

