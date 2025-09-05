#include "PictureBox.h"
namespace ezui {
	PictureBox::PictureBox(Object* parentObject) :Control(parentObject)
	{
		Init();
	}
	void PictureBox::Init() {
		m_timer = new Timer(this);
		m_timer->Tick = [this](Timer* timer) {
			timer->Stop();
			HWND hWnd = this->Hwnd();
			BeginInvoke([this, hWnd, timer]() {
				if (!::IsWindow(hWnd))return;
				if (this->Image) {
					m_timer->Interval = this->Image->NextFrame();
					this->Invalidate();
				};
				});
			};
	}
	PictureBox::~PictureBox() {
	}
	void PictureBox::OnForePaint(PaintEventArgs& arg) {
		if (Image) {
			arg.Graphics.DrawImage(Image, RectF(0, 0, (float)Width(), (float)Height()));
			if (Image->FrameCount() > 1) {
				m_timer->Start();
			}
		}
		__super::OnForePaint(arg);
	}
	void PictureBox::SetAttribute(const UIString& key, const UIString& value) {
		__super::SetAttribute(key, value);
		if (key == "src") {
			Image = this->Attach(Image::Make(value));
		}
	}
};
