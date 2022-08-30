#pragma once
#include "global.h"
#include "VlcPlayer.h"
class MainFrm :
	public LayeredWindow
{
private:
	VlcPlayer player;
	void InitForm();
	VList* localList;
	VList* searchList;
	Edit* searchEdit;
protected:
	Image* bkImage = NULL;
	virtual void OnKeyDown(WPARAM wparam)override;
	bool OnNotify(Control* sender, const EventArgs& args)override;
	void NextPage(int a, int b);
	void SongView();
	void LrcView();
public:
	MainFrm();
	virtual ~MainFrm();
	void OnClose(bool& b) override;

};

