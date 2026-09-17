#pragma once
#include "global.h"
/// <summary>
/// 左侧本地歌曲列表中的Item
/// </summary>
class LocalItem :public HBox {
public:
	Label songName;
	Label time;
	Label del;
	virtual ~LocalItem();
	LocalItem(const UIString& _songName, const UIString& _songTime = "03:56");
};
/// <summary>
/// 搜索列表中的Item
/// </summary>
class SearchItem :public HBox {
	Label songName;
	Label AlbumName;
	Label mv;
	Label time;
	Song song;
	Label del;
public:
	virtual ~SearchItem();
	SearchItem(const Song& s);
};

class SearchStatusItem :public Label {
	UIString message;
	bool highlighted;
public:
	SearchStatusItem(const UIString& text, bool highlight = false);
};

// 底部播放器进度条
class PlayerProgressBar :public Slider {
public:
	PlayerProgressBar(Object* ownerObj = NULL);
protected:
	virtual void OnTrackPaint(PaintEventArgs* args) override;
	virtual void OnThumbPaint(PaintEventArgs* args) override;
	virtual SizeF GetThumbSize() override;
	virtual RectF GetTrackRect() override;
	virtual RectF GetThumbRect() override;
};

//登录窗口
class LoginFrm :public BorderlessWindow {
protected:
	virtual void OnNotify(Control* sender, EventArgs* args)override;
public:
	//保存用户名
	UIString m_userName;
	LoginFrm(HWND owner);
};
