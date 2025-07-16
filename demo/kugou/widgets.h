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

//登录窗口
class LoginFrm :public LayeredWindow {
	UIManager umg;
protected:
	virtual bool OnNotify(Control* sender, EventArgs& args)override;
public:
	//保存用户名
	UIString m_userName;
	LoginFrm(HWND owner);
};