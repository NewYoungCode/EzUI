#pragma once
#include "Control.h"
namespace EzUI {
	class UI_EXPORT  ILayout {
	public:
		int Margin = 0;//�����ˮƽ�б��Ǿ��� �ұ߾� ,��ֱ�����������±߾�
		std::map<Control*, int> _controlsLocationX;
		std::map<Control*, int> _controlsLocationY;
	public:
		virtual void ResumeLayout() = 0;//���²��� ��������ˢ��
	};
	class UI_EXPORT  Layout :
		public Control, public ILayout
	{
	public:
		Layout();
		virtual void ResumeLayout() override;
		virtual ~Layout();
	};
	using Box = Layout;
};