#pragma once
#include "global.h"
#include "lrcPanel.h"
#include "VlcPlayer.h"

//用于桌面MV/歌词显示
class DesktopLrcFrm :public BorderlessWindow {
private:
	VlcPlayer* _player;
	LrcPanel _lrc;
public:
	DesktopLrcFrm(VlcPlayer* player);
	void LoadLrc(const UIString& lrcData);
	void ChangePostion(int postion);
};