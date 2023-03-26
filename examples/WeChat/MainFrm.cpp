#include "MainFrm.h"
MainFrm::MainFrm() :Form(1000, 670) {

	UIManager ui("xml/MainFrm.htm");
	this->SetLayout(ui.GetRoot());
	//现在开始给MainFrm.htm文件编写界面

	Control* friend_view= this->FindControl("friend_view");
	friend_view->ScrollBar->SetFixedWidth(8);

}

MainFrm::~MainFrm() {

}