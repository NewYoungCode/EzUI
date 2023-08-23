#include "PictureBox.h"
namespace EzUI {
	PictureBox::PictureBox() {}
	PictureBox::~PictureBox() {
		_timer.Stop();
	}
	void PictureBox::OnRemove() {
		__super::OnRemove();
		_timer.Stop();
	}
	void PictureBox::SetImage(Image* image) {
		_img = image;
	}
	void PictureBox::OnDpiChange(const DpiChangeEventArgs& args) {
		if (args.Scale != this->GetScale() && _img) {
			this->_img->Margin.Scale(args.Scale / this->GetScale());
		}
		__super::OnDpiChange(args);
	}
	void PictureBox::OnForePaint(PaintEventArgs& arg) {
		if (_img) {
			arg.Graphics.DrawImage(_img, Rect(0, 0, Width(), Height()));
			if (_img->FrameCount() > 1) {
				_timer.Interval = _img->NextFrame();
				_timer.Tick = [=](Windows::Timer* timer) {
					this->Invalidate();
					timer->Stop();
					};
				_timer.Start();
			}
			else {
				_timer.Stop();
			}
		}
		__super::OnForePaint(arg);
	}
	void PictureBox::SetAttribute(const EString& key, const EString& value) {
		__super::SetAttribute(key, value);
		if (key == "src" || key == "img") {
			Image* img = Image::FromFile(value);
			SetImage(img);
		}
	}
};
