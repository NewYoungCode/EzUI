#include "MainFrm.h"
MainFrm::MainFrm() :Form(1000, 670) {

	UIManager ui("xml/MainFrm.htm");
	this->SetLayout(ui.GetRoot());
	//���ڿ�ʼ��MainFrm.htm�ļ���д����

	Control* friend_view= this->FindControl("friend_view");
	friend_view->ScrollBar->SetFixedWidth(8);

}

MainFrm::~MainFrm() {

}