#pragma once
#include "global.h"
/// <summary>
/// ��౾�ظ����б��е�Item
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
/// �����б��е�Item
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