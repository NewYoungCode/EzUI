#pragma once
#include "global.h"
class Lrc
{
public:
	int postion;
	EString text;
	Point point;
	Lrc(int postionInt,  EString text, Point point)
	{
		this->postion = postionInt;
		this->text = text;
		this->point = point;
	}
};
class LrcControl :
	public Control
{
	float offsetY = 0;
	Lrc* LrcNow = NULL;
	/// <summary>
	 /// 上下居中的Y坐标值
	 /// </summary>
	int VerticalCenter = 0;
	std::vector<Lrc*> LrcList;
	int marginVertical = 40;
	int FontHeight =30;
public:
	virtual ~LrcControl();
	/// <summary>
/// 传入进度时间滚动到所在时间的位置
/// </summary>
/// <param name="postion">时间/秒</param>
	void ChangePostion(int postion);
	void OnTimer() override;
	void OnBackgroundPaint(PaintEventArgs& arg)override;
	void LoadLrc(const EString& lrcData);
	/// <summary>
	/// 清除所有歌词
	/// </summary>
	void Clear();
};

