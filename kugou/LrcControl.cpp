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
		{ 
			if (LrcNow == &lrc)
			{
				arg.Graphics.SetColor(Color(211, 174, 87));
				arg.Graphics.SetFont(GetFontFamily().utf16(), GetFontSize()+3);
				arg.Graphics.DrawString( lrc.text.utf16(), rectangle, TextAlign::MiddleCenter);
			}
			else
			{
				arg.Graphics.SetColor(GetForeColor());
				arg.Graphics.SetFont(GetFontFamily().utf16(), GetFontSize());
				arg.Graphics.DrawString(lrc.text.utf16(), rectangle, TextAlign::MiddleCenter);
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
	if (timer) {
		timer->Stop();
		delete timer;
	}
	for (auto& it : LrcList) {
		delete it;
	}
}

LrcControl::LrcControl()
{
	//ActiveStyle.BackgroundColor = Color(200,100,200,100);
	timer = new Timer;
	timer->Interval = 2;
	timer->Tick = [=](Timer*) {
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
		int postion = (fen * 60 * 1000 + miao * 1000);//????????
		LrcList.push_back(new Lrc(postion, text, Point(0, VerticalCenter)));
		VerticalCenter += (FontHeight + marginVertical);//??????????
	}

	if (LrcList.size() > 0)
	{
		LrcNow = LrcList[0];//
		timer->Start();
	}
}