#pragma once
#include "ui.h"

#include "EzUI.h"
#include "Form.h"
#include "Label.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "Edit.h"
#include "VList.h"
#include "TileLayout.h"
#include "Button.h"

#include "Edit.h"

#include "Painter.h"

using namespace ui;

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
	Edit edit;//������
	Layout top;//��������
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
	Layout bottom_left;
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
	void OnSize(const Size& sz) override;
	void OnPaint(HDC winHDC, const Rect& rePaintRect)override;
	void OnDestroy();
};

