#pragma once
#include "EzUI/EzUI.h"
#include "EzUI/TextBox.h"
#include "EzUI/BorderlessWindow.h"
#include "EzUI/Application.h"
#include "EzUI/UIManager.h"
#include "EzUI/HLayout.h"
#include "EzUI/Label.h"
#include "EzUI/VList.h"
#include "EzUI/HLayout.h"
#include "EzUI/VLayout.h"
#include "EzUI/Button.h"
#include "EzUI/TabLayout.h"
#include "EzUI/NotifyIcon.h"
#include "EzUI/BorderlessWindow.h"
#include "EzUI/LayeredWindow.h"
#include "EzUI/UIManager.h"
#include "EzUI/Task.h"

#include "resource.h"

#include "WebClient.h"
#include "JsonCpp.h"
#include "HttpUtility.h"
#include "base64.h"
#include "ConfigIni.h"

#ifdef NDEBUG
#pragma comment(lib,"Common.lib")
#else
#pragma comment(lib,"CommonD.lib")
#endif

//�Զ���֪ͨ����Ϣ
#define WM_NOTIFYICON1 WM_APP+1

using namespace EzUI;

struct Song {
	EString hash;//������ϣֵ
	EString SongName;//��������
	EString SingerName;//��������
	EString MvHash;//mv��ϣֵ
	int Duration;//����ʱ�� ��λ:��
	EString AlbumID;//
	EString AlbumName;//
	int QualityLevel;//�����ȶ�
};

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

namespace global {
	extern int pageSize;
	extern int page;
	extern bool nextPage;
	extern HWND _workerw;

	//��������ת�ַ�����ʾ
	extern EString toTimeStr(long dur);
	//http�Կṷapi������
	extern int HttpGet(const EString& url, EString& resp);
	//���ݹؼ��ֽ��и�������
	extern std::vector<Song> SearchSongs(const EString& keyword);
	//ʹ�ø�����AlbumIdѰ�Ҳ��Ҷ�Ӧ�ĸ��
	extern EString GetSongLrc(const EString& hash, const EString& AlbumID = "");
	//��ȡ������ ������������ʾ
	extern HWND GetWorkerW();
};
