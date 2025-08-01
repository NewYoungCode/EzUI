#include "PictureBox.h"
namespace ezui {
	PictureBox::PictureBox(Object* parentObject) :Control(parentObject)
	{
		Init();
	}
	void PictureBox::Init() {
		m_timer.Tick = [this](Timer* timer) {
			Invoke([&]() {
				timer->Stop();
				m_timer.Interval = m_img->NextFrame();
				this->Invalidate();
				});
			};
	}
	PictureBox::~PictureBox() {
		m_timer.Stop();
		if (m_srcImg) {
			delete m_srcImg;
		}
	}
	void PictureBox::OnRemove() {
		__super::OnRemove();
		m_timer.Stop();
	}
	void PictureBox::OnForePaint(PaintEventArgs& arg) {
		if (m_img) {
			arg.Graphics.DrawImage(m_img, RectF(0, 0, (float)Width(), (float)Height()));
			if (m_img->FrameCount() > 1) {
				if (m_timer.IsStopped()) {
					m_timer.Start();
				}
			}
			else {
				m_timer.Stop();
			}
		}
		__super::OnForePaint(arg);
	}
	void PictureBox::SetImage(Image* image) {
		m_img = image;
	}
	void PictureBox::SetAttribute(const UIString& key, const UIString& value) {
		__super::SetAttribute(key, value);
		if (key == "src" || key == "img") {
			if (m_srcImg)delete m_srcImg;
			m_srcImg = Image::Make(value);
			SetImage(m_srcImg);
		}
	}
};
