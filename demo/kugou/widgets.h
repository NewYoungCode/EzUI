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
	virtual void OnChildPaint(PaintEventArgs& args)override;
	LocalItem(const EString& _songName, const EString& _songTime = "03:56");
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