#pragma once
#include "Control.h"
#include "Timer.h"

namespace ezui {
	class UI_EXPORT PictureBox : public Control {
	private:
		Image* _srcImg = NULL;
		Image* _img = NULL;
		Timer _timer;
	protected:
		virtual void OnRemove()override;
		virtual void OnForePaint(PaintEventArgs& arg)override;
	public:
		PictureBox();
		virtual ~PictureBox();
		void SetImage(Image* image);
		virtual void SetAttribute(const UIString& key, const UIString& value)override;
	};
};
