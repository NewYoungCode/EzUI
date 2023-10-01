#pragma once
#include "global.h"
class Lrc
{
public:
	int postion;
	EString text;
	Point point;
	Lrc(int postionInt, const EString& text, Point point)
	{
		this->postion = postionInt;
		this->text = text;
		this->point = point;
	}
	~Lrc() {
		int a = 0;
	}
};

class LrcControl :
	public Control
{
	float offsetY = 0;
	Lrc* LrcNow = NULL;
	Timer* timer = NULL;
	int VerticalCenter = 0;
	std::vector<Lrc*> LrcList;
	int marginVertical = 40;
	int FontHeight = 30;
public:
	LrcControl();
	virtual ~LrcControl();
	void Task();
	void ChangePostion(int postion);

	void OnBackgroundPaint(PaintEventArgs& arg)override;
	void LoadLrc(const EString& lrcData);
	void Clear();
};

