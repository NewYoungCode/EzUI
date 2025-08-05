#pragma once
#include "Control.h"
#include "Timer.h"

namespace ezui {
	class UI_EXPORT PictureBox : public Control {
	private:
		Image* m_srcImg = NULL;
		Image* m_img = NULL;
		Timer *m_timer;
	private:
		void Init();
	protected:
		virtual void OnRemove()override;
		virtual void OnForePaint(PaintEventArgs& arg)override;
	public:
		PictureBox(Object* parentObject = NULL);
		virtual ~PictureBox();
		void SetImage(Image* image);
		virtual void SetAttribute(const UIString& key, const UIString& value)override;
	};
};
