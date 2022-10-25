#include "LrcControl.h"
void LrcControl::ChangePostion(int postion)
{
	for (auto _it = LrcList.rbegin(); _it != LrcList.rend(); _it++)
	{
		auto it = *_it;
		if (it->postion <= postion) {
			LrcNow = it;
			break;
		}
	}
	if (this->GetRect().IsEmptyArea()) {
		return;
	}
	if (LrcNow == NULL) {
		return;
	}
	VerticalCenter = this->Height() / 2 - (marginVertical + FontHeight) / 2;
	offsetY = LrcNow->point.Y - VerticalCenter;
}

void LrcControl::Task()
{
	if (this->GetRect().IsEmptyArea() || LrcNow == NULL) {
		return;
	}
	offsetY = LrcNow->point.Y - VerticalCenter;
	if (std::abs(offsetY) <= 1)
	{
		return;
	}
	for (auto&& item : LrcList)
	{
		Lrc* lrc = item;
		if (offsetY < 0)
		{
			lrc->point.Y += 1;
		}
		else
		{
			lrc->point.Y -= 1;
		}
	}
	Invalidate();
	//Refresh();
}
void LrcControl::OnBackgroundPaint(PaintEventArgs& arg) {

	__super::OnBackgroundPaint(arg);
	for (auto&& item : LrcList)
	{
		Lrc& lrc = *item;
		Rect rectangle(lrc.point.X, lrc.point.Y, Width(), (int)FontHeight);
		Rect drawRec(GetRect());
		//arg.Painter.DrawRectangle(Color::Gray, drawRec);
		if (GetRect().Contains(rectangle))
		{ //包含在此区域内的歌词才可以绘制
			if (LrcNow == &lrc)
			{
				//arg.Painter.DrawRectangle(Color::Red, rectangle);
				arg.Painter.DrawString(lrc.text.utf16(), GetFontFamily().utf16(), GetFontSize() + 0, Color(211, 174, 87), rectangle, TextAlign::MiddleCenter);
			}
			else
			{
				//arg.Painter.DrawRectangle(Color::Red, rectangle);
				arg.Painter.DrawString(lrc.text.utf16(), GetFontFamily().utf16(), GetFontSize(), GetForeColor(), rectangle, TextAlign::MiddleCenter);
			}
		}
	}
}

void LrcControl::Clear()
{

	timer->Stop();
	LrcNow = NULL;
	for (auto& it : LrcList) {
		delete it;
	}
	LrcList.clear();
	VerticalCenter = Height() / 2 - (marginVertical + FontHeight) / 2;
}

LrcControl::~LrcControl()
{
	Clear();
}

LrcControl::LrcControl()
{

	//ActiveStyle.BackgroundColor = Color(200,100,200,100);

	timer = new Timer;
	timer->Interval = 2;
	timer->Tick = [=]() {
		Task();
	};
}

void LrcControl::LoadLrc(const EString& lrcData)
{
	Clear();
	auto lrc = lrcData.Split("\n");
	auto gbk = lrcData.ansi();
	for (auto&& it : lrc) {
		if (it.empty()) continue;
		int pos1 = it.find("[");
		int pos2 = it.find("]");
		if (pos1 == 0 && pos2 == 9) {}
		else {
			continue;
		}
		EString text = it.substr(pos2 + 1);
		auto gbk2 = Text::UTF8ToANSI(it);
		int fen = std::atoi(it.substr(1, 2).c_str());
		float miao = std::atof(it.substr(4, 5).c_str());
		int postion = (fen * 60 * 1000 + miao * 1000);//转换成毫秒
		LrcList.push_back(new Lrc(postion, text, Point(0, VerticalCenter)));
		VerticalCenter += (FontHeight + marginVertical);//垂直坐标递增
	}

	if (LrcList.size() > 0)
	{
		LrcNow = LrcList[0];//如果有一句歌词默认是第一句歌词变色
		timer->Start();
	}
}