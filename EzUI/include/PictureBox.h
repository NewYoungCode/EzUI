#pragma once
#include "Control.h"
namespace EzUI {
	class UI_EXPORT PictureBox : public Control {
	private:
		bool _exit = false;
		std::mutex _mtx;
		Image* _img = NULL;
		std::thread* _gifTask = NULL;
	protected:
		virtual void OnRemove()override;
	public:
		PictureBox();
		PictureBox(Control* parent);
		virtual ~PictureBox();
		void SetImage(Image* image);
		virtual void OnForePaint(PaintEventArgs& arg)override;
		virtual void SetAttribute(const EString& key, const EString& value)override;
	};
};