#include "MainFrm.h"
#include <UIManager.h>

class SongItem :public HLayout {
public:
	Label lb1;
	Label lb2;
	Label lb3;
	Label lb4;
	SongItem() {
		lb1.SetText("1");
		lb2.SetText(utf8("张学友"));
		lb3.SetText(utf8("会呼吸的痛"));
		lb4.SetText(L"▶");
		AddControl(&lb1);
		AddControl(&lb2);
		AddControl(&lb3);
		AddControl(&lb4);
		AddControl(new HSpacer(200));
		SetFixedHeight(35);
		Cursor = Cursor::HAND;
		Dock = DockStyle::Horizontal;
		ActiveStyle.ForeColor = Color::Pink;
		lb1.ActiveStyle.ForeColor = Color::Red;
		HoverStyle.BackgroundColor = Color(200, 200, 200, 200);
	}
	void OnMouseUp(MouseButton mbtn, const Point& point)override {
		__super::OnMouseUp(mbtn, point);
	}

};

MainFrm::MainFrm(int width, int height) :Form(width, height)
{
	//::SetParent(::CreateWindowW(L"Button", L"hello word", WS_VISIBLE |WS_POPUP, 100, 30, 120, 35, _hWnd, NULL, GetModuleHandle(NULL), NULL),_hWnd);
	laout.Style.BackgroundColor = { 35,35,38 };
	top.SetSize({ laout.Width(),60 });
	//Image *imgx = new Image("D:\\hd(3).png");
	//top.Style.BackgroundImage = (imgx);
	top.SetFixedHeight(60);
	top.Style.BackgroundColor = { 33,33,36 };
	laout.AddControl(&top);
	top.Dock = DockStyle::Horizontal;
	top.Action = ControlAction::MoveWindow;//鼠标捕捉 移动窗口

	title.SetText(utf8("网抑云音乐"));
	title.Style.ForeColor = (Color::White);
	title.Style.FontSize = (14);
	title.SetRect({ 14,15,131,35 });

	top.HoverStyle.BackgroundColor = Color::Pink;

	top.AddControl(&title);

	edit.SetRect({ 198,14,400,30 });

	edit.Style.BackgroundColor = { 41,41,44 };
	edit.Style.SetBorder({ 41,41,44 }, 1);
	edit.Style.ForeColor = (Color::White);
	edit.Style.Radius = (30);
	top.AddControl(&edit);

	redBar.SetSize({ laout.Width(),2 });
	redBar.SetFixedHeight(2);
	redBar.Style.BackgroundColor = { 173,32,32 };
	laout.AddControl(&redBar);
	redBar.Dock = DockStyle::Horizontal;

	centerLayout.SetSize({ laout.Width(),10 });
	centerLayout.Style.BackgroundColor = { 43,43,43 };
	laout.AddControl(&centerLayout);
	centerLayout.Dock = DockStyle::Horizontal;

	menu.SetSize({ 200,centerLayout.Height() });
	menu.SetFixedWidth(200);
	menu.Style.ForeColor = (Color::White);
	menu.Style.SetBorder(Color(68, 68, 68), 1);
	menu.ScrollBar->Style.BackgroundColor = { 100, 63,63,63 };
	menu.ScrollBar->Style.ForeColor = { 50, 250,250,250 };
	menu.ScrollBar->Style.Radius = (10);
	centerLayout.AddControl(&menu);
	menu.Dock = DockStyle::Vertical;

	musicList.SetSize({ 200,centerLayout.Height() });
	centerLayout.AddControl(&musicList);
	musicList.Dock = DockStyle::Vertical;

	musicList.Style.FontSize = (11);
	musicList.Style.ForeColor = (Color::White);

	musicList.ScrollBar->Style.BackgroundColor = { 100, 63,63,63 };
	musicList.ScrollBar->Style.ForeColor = { 50, 250,250,250 };
	musicList.ScrollBar->Style.Radius = (10);
	musicList.ScrollBar->ActiveStyle.ForeColor = { 50, 200,200,200 };
	/*musicList.HoverStyle.ForeColor = Color::Red;
	musicList.ActiveStyle.ForeColor = Color::Blue;
	musicList.ActiveStyle.FontSize = 9;*/
	musicList.ActiveStyle.BackgroundColor = Color(100, 1, 200, 100);
	//musicList.Margin = 10;
	for (size_t i = 0; i < 999; i++)
	{
		SongItem* lb = new SongItem;
		lb->lb1.SetText(std::to_string(i));

		if (i % 2) {
			lb->Style.BackgroundColor = { 55,55,55 };
		}
		else {
			lb->Style.BackgroundColor = { 43,43,43 };
		}
		//lb->MouseClick = [=](Control* sender, MouseButton btn, const Point&)->void {
		//	if (btn == MouseButton::Left) {
		//		::MessageBox(Hwnd(), TEXT("点击"), TEXT(""), 0);

		//	}
		//};
		musicList.AddControl(lb);
	}

	btn1.SetText(utf8("发现音乐"));
	btn1.SetSize({ 200,63 });
	menu.AddControl(&btn1);

	btn2.SetText(utf8("博客"));
	btn2.SetSize({ 200,63 });
	menu.AddControl(&btn2);

	btn3.SetText(utf8("视频"));
	btn3.SetSize({ 200,63 });
	menu.AddControl(&btn3);

	btn4.SetText(utf8("朋友"));
	btn4.SetSize({ 200,63 });
	menu.AddControl(&btn4);

	btn5.SetText(utf8("直播"));
	btn5.SetSize({ 200,63 });
	menu.AddControl(&btn5);

	btn6.SetText(utf8("诗人FM"));
	btn6.SetSize({ 200,63 });
	menu.AddControl(&btn6);

	bottom.SetSize({ laout.Width(),73 });
	bottom.SetFixedHeight({ 72 });
	bottom.Style.BackgroundColor = { 33,33,36 };
	//bottom.Style.SetBorder({ 59,59,62 }, 1);
	laout.AddControl(&bottom);
	bottom.Dock = DockStyle::Horizontal;

	bottom_left.SetSize({ 222 ,100 });
	bottom_left.SetFixedWidth(222);
	bottom_left.Dock = DockStyle::Vertical;
	bottom_left.Style.ForeColor = (Color::White);
	bottom.AddControl(&bottom_left);

	head.SetRect({ 13,13,50,50 });
	head.Style.Radius = (10);

	head.Style.BackgroundImage = (img);
	head.Action = ControlAction::Max;

	bottom_left.AddControl(&head);

	songName.SetRect({ 73,15 , 155, 23 });
	songName.SetText(L"Smile Again한국어");
	songName.Style.FontSize = (10);
	songName.TextAlign = (TextAlign::MiddleLeft);
	bottom_left.AddControl(&songName);

	singer.SetRect({ 73,39 , 165, 23 });
	singer.SetText(utf8("G.NA/徐恩光/柳贤镇"));
	singer.TextAlign = (TextAlign::MiddleLeft);
	bottom_left.AddControl(&singer);

	aixin.Style.BackgroundImage = (ax);
	aixin.SetRect({ 156,15,22,22 });
	bottom_left.AddControl(&aixin);

	bottom_center.SetSize({ 1,74 });
	bottom_center.Style.ForeColor = (Color::White);
	bottom.AddControl(&bottom_center);

	sp1.SetSize({ 1,1 });
	bottom_center.AddControl(&sp1);

	Label1.SetSize({ 70,15 });
	Label1.SetLocation({ 0,50 });
	Label1.SetFixedWidth(70);
	Label1.SetText(utf8("00:00"));
	bottom_center.AddControl(&Label1);

	prog.SetSize({ 1,3 });
	prog.SetLocation({ 0,55 });
	prog.SetFixedWidth(360);
	prog.Style.BackgroundColor = { 236,65,65 };
	bottom_center.AddControl(&prog);

	Label2.SetSize({ 70,15 });
	Label2.SetLocation({ 0,50 });
	Label2.SetFixedWidth(70);
	Label2.SetText(utf8("03:50"));
	bottom_center.AddControl(&Label2);

	sp2.SetSize({ 1,1 });
	bottom_center.AddControl(&sp2);

	bottom_right.SetSize({ 219,74 });
	bottom_right.SetFixedWidth(219);
	bottom.AddControl(&bottom_right);

	yz_std.SetRect({ 0,13,48,48 });
	yz_std.SetFixedWidth(48);

	yz_std.Style.ForeImage = (std_img);
	bottom_right.AddControl(&yz_std);

	space1.SetSize({ 1,1 });
	bottom_right.AddControl(&space1);

	yx.SetRect({ 0,26,20,20 });
	yx.SetFixedWidth(20);

	yx.Style.ForeImage = (yx_img);
	bottom_right.AddControl(&yx);

	space1.SetSize({ 1,1 });
	bottom_right.AddControl(&space2);

	yl.SetRect({ 0,26,21,21 });
	yl.SetFixedWidth(21);
	yl.Style.ForeImage = (yl_img);
	bottom_right.AddControl(&yl);

	space3.SetSize({ 1,1 });
	bottom_right.AddControl(&space3);

	hy.SetRect({ 0,24,22,22 });
	hy.SetFixedWidth(22);

	hy.Style.ForeImage = (hy_img);
	bottom_right.AddControl(&hy);

	space4.SetSize({ 1,1 });
	bottom_right.AddControl(&space4);

	lb.SetRect({ 0,24,22,22 });
	lb.SetFixedWidth(22);

	lb.Style.ForeImage = (lb_img);
	bottom_right.AddControl(&lb);

	space5.SetSize({ 1,1 });
	bottom_right.AddControl(&space5);

	//设置布局显示窗口
	//laout.Style.SetBorder(Color::Gray-100, 1);
	SetLayout(&laout);
}

void MainFrm::OnSize(const Size& sz)
{
	__super::OnSize(sz);
}

void MainFrm::OnPaint(HDC winHDC, const Rect& rePaintRect)
{
	__super::OnPaint(winHDC, rePaintRect);
}


void MainFrm::OnDestroy()
{
	Application::exit();
}


