#pragma once
#include "Control.h"
#include "Timer.h"
namespace EzUI {
	class UI_EXPORT PictureBox : public Control {
	private:
		Image* _img = NULL;
		Windows::Timer _timer;
	protected:
		virtual void OnRemove()override;
		virtual void OnForePaint(PaintEventArgs& arg)override;
		virtual void OnDpiChange(const DpiChangeEventArgs& args)override;
	public:
		PictureBox();
		virtual ~PictureBox();
		void SetImage(Image* image);
		virtual void SetAttribute(const EString& key, const EString& value)override;
	};
};
