#pragma once
#include "global.h"
#include "Timer.h"
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
	Timer* timer = NULL;
	/// <summary>
	 /// ���¾��е�Y����ֵ
	 /// </summary>
	int VerticalCenter = 0;
	std::vector<Lrc*> LrcList;
	int marginVertical = 40;
	int FontHeight =30;
public:
	LrcControl();
	virtual ~LrcControl();
	/// <summary>
/// �������ʱ�����������ʱ���λ��
/// </summary>
/// <param name="postion">ʱ��/��</param>
/// 
	void Task();
	void ChangePostion(int postion);

	void OnBackgroundPaint(PaintEventArgs& arg)override;
	void LoadLrc(const EString& lrcData);
	/// <summary>
	/// ������и��
	/// </summary>
	void Clear();
};

