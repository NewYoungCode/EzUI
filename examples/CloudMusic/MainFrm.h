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

	VLayout laout;//��ֱ������
	Label title;//����
	TextBox edit;//������
	Control top;//��������
	Control redBar;//��ɫ������
	HLayout centerLayout;//���Ĳ���
	VList menu;//���˵�
	VList musicList;//�����б�
	Label btn1;
	Label btn2;

	Control sp2;
	Control prog;//������

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
	Label head;//����ͷ��
	Label songName;//��������
	Label singer;//����
	Label aixin;//����
	HLayout bottom_center;//������ ��ť����
	Control sp1;//ռλ�ؼ�
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

