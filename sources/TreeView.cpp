#include "TreeView.h"
#include "Label.h"
namespace ezui {

	TreeView::TreeView(Object* ownerObject) :HListView(ownerObject)
	{
		m_vList.SetAutoWidth(true);
		m_vList.SetDockStyle(DockStyle::Vertical);
		this->Add(&m_vList);
	}

	void TreeView::AddNode(const UIString& nodeName)
	{
		Label* label = new Label(this);
		label->TextAlign = TextAlign::MiddleLeft;
		label->SetText(nodeName);
		label->SetAutoSize(true);
		m_vList.Add(label);
	}

	TreeView::~TreeView()
	{

	}
}

