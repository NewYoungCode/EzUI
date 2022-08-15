#pragma once
#include "ui.h"
#include "Form.h"
#include "Label.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "Edit.h"
#include "VList.h"
#include "TileLayout.h"
#include "Button.h"
class MainFrm:public BorderlessWindow
{
private:
	VLayout laout;//��ֱ������
	Button title;//����
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
	void OnPaint(HDC hdc, const Rect& rect);
	void OnDestroy();
	void OnKeyDown(WPARAM wparam) override;
	bool OnNotify(Control* sender, const EventArgs& args) override;
};

