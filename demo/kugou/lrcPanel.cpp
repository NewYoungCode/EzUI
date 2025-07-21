#include "lrcPanel.h"
void LrcPanel::ChangePostion(int postion)
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

void LrcPanel::Task()
{
	if (this->GetRect().IsEmptyArea() || LrcNow == NULL) {
		return;
	}
	offsetY = LrcNow->point.Y - VerticalCenter;
	if (std::abs(offsetY) <= 1)
	{
		return;
	}

	auto v = std::abs(offsetY);
	int v2 = v * 0.025;
	if (v2 == 0) {
		v2 = 1;
	};

	for (auto& item : LrcList)
	{
		Lrc* lrc = item;

		if (offsetY < 0)
		{
			lrc->point.Y += v2;
		}
		else if (offsetY > 0)
		{
			lrc->point.Y -= v2;
		}
	}

	if (this->PublicData) {
		BeginInvoke([=]() {
			Invalidate();
			});
	}

}
void LrcPanel::OnBackgroundPaint(PaintEventArgs& arg) {

	__super::OnBackgroundPaint(arg);
	for (auto&& item : LrcList)
	{
		Lrc& lrc = *item;
		Rect rectangle(lrc.point.X, lrc.point.Y, Width(), (int)FontHeight);
		auto w = UIString(lrc.text).unicode();
		Rect drawRec(GetRect());
		if (drawRec.Contains(rectangle))
		{
			float fontSize = 14 * this->GetScale();
			if (LrcNow == &lrc)
			{
				arg.Graphics.SetColor(Color(211, 174, 87));
				arg.Graphics.SetFont(GetFontFamily(), fontSize + 4);
				arg.Graphics.DrawString(lrc.text.unicode(), rectangle, TextAlign::MiddleCenter);
			}
			else
			{
				arg.Graphics.SetColor(GetForeColor());
				arg.Graphics.SetFont(GetFontFamily(), fontSize);
				arg.Graphics.DrawString(lrc.text.unicode(), rectangle, TextAlign::MiddleCenter);
			}
		}
	}
}

void LrcPanel::Clear()
{
	timer->Stop();
	LrcNow = NULL;
	for (auto& it : LrcList) {
		delete it;
	}
	LrcList.clear();
	VerticalCenter = Height() / 2 - (marginVertical + FontHeight) / 2;
}

LrcPanel::~LrcPanel()
{
	if (timer) {
		timer->Stop();
		delete timer;
	}
	for (auto& it : LrcList) {
		delete it;
	}
}

LrcPanel::LrcPanel()
{
	timer = new Timer;
	timer->Interval = 2;
	timer->Tick = [=](Timer*) {
		Task();
		};
}

void LrcPanel::LoadLrc(const UIString& lrcData)
{
	Clear();
	auto lrc = lrcData.split("\n");
	auto gbk = lrcData.ansi();
	for (auto&& it : lrc) {
		if (it.empty()) continue;
		int pos1 = it.find("[");
		int pos2 = it.find("]");
		if (pos1 == 0 && pos2 == 9) {}
		else {
			continue;
		}
		UIString text = it.substr(pos2 + 1);
		int fen = std::atoi(it.substr(1, 2).c_str());
		float miao = std::atof(it.substr(4, 5).c_str());
		int postion = (fen * 60 * 1000 + miao * 1000);
		LrcList.push_back(new Lrc(postion, text, Point(0, VerticalCenter)));
		VerticalCenter += (FontHeight + marginVertical);
	}

	if (LrcList.size() > 0)
	{
		LrcNow = LrcList[0];//
		timer->Start();
	}
}