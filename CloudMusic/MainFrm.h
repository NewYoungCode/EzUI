#pragma once
#include "ui.h"
#include "Form.h"
#include "Lable.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "Edit.h"
#include "VList.h"
#include "TileLayout.h"
class MainFrm:public Form
{
private:
	VLayout laout;//垂直布局器
	Lable title;//标题
	EditWnd edit;//搜索框
	Layout top;//顶部部分
	Control redBar;//红色的线条
	HLayout centerLayout;//中心部分
	VList menu;//左侧菜单
	VList musicList;//音乐列表
	Lable btn1;
	Lable btn2;

	Control sp2;
	Control prog;//进度条
	
	Lable btn4;
	Lable btn3;
	Lable lb;
	Control space3;
	Lable yx;
	Control space1;
	Lable yz_std;
	Lable lable2;
	Lable btn5;
	Lable btn6;
	HLayout bottom;
	Layout bottom_left;
	Lable head;//歌手头像
	Lable songName;//歌曲名称
	Lable singer;//歌手
	Lable aixin;//爱心
	HLayout bottom_center;//进度条 按钮部分
	Control sp1;//占位控件
	Lable lable1;
	HLayout bottom_right;
	Control space2;
	Lable yl;
	Lable hy;
	Control space4;
	Control space5;

	Image yl_img=(L"D:\\wyy\\yl.png");
	Image hy_img=(L"D:\\wyy\\hy.png");
	Image lb_img=(L"D:\\wyy\\lb.png");
	Image yx_img=(L"D:\\wyy\\yx.png");
	Image std_img=(L"D:\\wyy\\std.png");
	Image ax=(L"D:\\wyy\\ax.png");
	Image img=(L"D:\\wyy\\hd2.png");
public :
	MainFrm(int width, int height);

	//WS_OVERLAPPEDWINDOW

	void OnDestroy();
};

