#pragma once
#include "EzUI.h"
#include "Edit.h"
#include "Form.h"
#include "Application.h"
#include "UIManager.h"
#include "HLayout.h"
#include "Label.h"
#include "VList.h"

#include "os.h"
#include "WebClient.h"
#include "JsonCpp.h"

using namespace EzUI;
class SongItem :public HBox {
public:
	Label songName;
	Label time;

	SongItem(const EString& _songName) {
		songName.SetText(_songName);
		songName.SetTextAlign(TextAlign::MiddleLeft);
		songName.MousePassThrough = Event::OnHover|Event::OnMouseDoubleClick;

		time.SetFixedWidth(50);
		time.SetText("03:56");
		time.SetTextAlign(TextAlign::MiddleRight);

		this->SetFixedHeight(33);
		this->Dock = DockStyle::Horizontal;
		HoverStyle.BackgroundColor = Color(100,230, 230, 230);

		AddControl(new HSpacer(15));
		AddControl(&songName);
		AddControl(&time);
		AddControl(new HSpacer(15));

	}
};