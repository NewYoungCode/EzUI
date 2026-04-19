#pragma once
#include "global.h"

//歌词面板控件
class LrcPanel : public Control {
private:
	struct Lrc
	{
		int postion;
		UIString text;
		Point point;
	public:
		Lrc(int postionInt, const UIString& text, const Point& point)
		{
			this->postion = postionInt;
			this->text = text;
			this->point = point;
		}
	};
private:
	float offsetY = 0;
	Lrc* LrcNow = NULL;
	Timer* timer = NULL;
	int VerticalCenter = 0;
	std::vector<Lrc*> LrcList;
	int marginVertical = 40;
	int FontHeight = 30;
protected:
	void OnBackgroundPaint(PaintEventArgs* arg)override;
public:
	LrcPanel(Object* owner = NULL);
	virtual ~LrcPanel();
	void ChangePostion(int postion);
	void LoadLrc(const UIString& lrcData);
	void ClearLrc();
};
