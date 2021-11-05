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
	VLayout laout;//��ֱ������
	Lable title;//����
	EditWnd edit;//������
	Layout top;//��������
	Control redBar;//��ɫ������
	HLayout centerLayout;//���Ĳ���
	VList menu;//���˵�
	VList musicList;//�����б�
	Lable btn1;
	Lable btn2;

	Control sp2;
	Control prog;//������
	
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
	Lable head;//����ͷ��
	Lable songName;//��������
	Lable singer;//����
	Lable aixin;//����
	HLayout bottom_center;//������ ��ť����
	Control sp1;//ռλ�ؼ�
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

