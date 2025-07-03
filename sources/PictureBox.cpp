#include "PictureBox.h"
namespace ezui {
	PictureBox::PictureBox() {
		_timer.Tick = [this](Timer* timer) {
			timer->Stop();
			this->Invalidate();
			};
	}
	PictureBox::~PictureBox() {
		_timer.Stop();
		if (_srcImg) {
			delete _srcImg;
		}
	}
	void PictureBox::OnRemove() {
		__super::OnRemove();
		_timer.Stop();
	}
	void PictureBox::SetImage(Image* image) {
		_img = image;
	}
	void PictureBox::OnForePaint(PaintEventArgs& arg) {
		if (_img) {
			arg.Graphics.DrawImage(_img, RectF(0, 0, (float)Width(), (float)Height()));
			if (_img->FrameCount() > 1) {
				_timer.Interval = _img->NextFrame();
				_timer.Start();
			}
			else {
				_timer.Stop();
			}
		}
		__super::OnForePaint(arg);
	}
	void PictureBox::SetAttribute(const UIString& key, const UIString& value) {
		__super::SetAttribute(key, value);
		if (key == "src" || key == "img") {
			if (_srcImg)delete _srcImg;
			_srcImg = Image::Make(value);
			SetImage(_srcImg);
		}
	}
};
