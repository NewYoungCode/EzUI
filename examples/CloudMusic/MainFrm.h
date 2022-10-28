#pragma once

#include "EzUI.h"
#include "Form.h"
#include "Label.h"
#include "HLayout.h"
#include "VLayout.h"
#include "TextBox.h"
#include "VList.h"
#include "TileList.h"
#include "Button.h"

#include "Application.h"

using namespace EzUI;

class MainFrm :public Form
{
private:
	Image *yl_img;
	Image* hy_img;
	Image* lb_img;
	Image* yx_img;
	Image* std_img;
	Image* ax;
	Image* img;

	VLayout laout;//垂直布局器
	Label title;//标题
	TextBox edit;//搜索框
	Control top;//顶部部分
	Control redBar;//红色的线条
	HLayout centerLayout;//中心部分
	VList menu;//左侧菜单
	VList musicList;//音乐列表
	Label btn1;
	Label btn2;

	Control sp2;
	Control prog;//进度条

	Label btn4;
	Label btn3;
	Label lb;
	Control space3;
	Label yx;
	Control space1;
	Label yz_std;
	Label Label2;
	Label btn5;
	Label btn6;
	HLayout bottom;
	Control bottom_left;
	Label head;//歌手头像
	Label songName;//歌曲名称
	Label singer;//歌手
	Label aixin;//爱心
	HLayout bottom_center;//进度条 按钮部分
	Control sp1;//占位控件
	Label Label1;
	HLayout bottom_right;
	Control space2;
	Label yl;
	Label hy;
	Control space4;
	Control space5;

	
public:
	MainFrm(int width, int height);
	//WS_OVERLAPPEDWINDOW
	//void OnPaint(HDC winHDC, const Rect& rePaintRect)override;
	void OnDestroy();
};

