#include "PictureBox.h"
namespace ezui {
	PictureBox::PictureBox() {
		Init();
	}
	PictureBox::PictureBox(Control* parentObject):Control(parentObject)
	{
		Init();
	}
	void PictureBox::Init() {
		m_timer.Tick = [this](Timer* timer) {
			timer->Stop();
			this->Invalidate();
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
	void PictureBox::SetImage(Image* image) {
		m_img = image;
	}
	void PictureBox::OnForePaint(PaintEventArgs& arg) {
		if (m_img) {
			arg.Graphics.DrawImage(m_img, RectF(0, 0, (float)Width(), (float)Height()));
			if (m_img->FrameCount() > 1) {
				m_timer.Interval = m_img->NextFrame();
				m_timer.Start();
			}
			else {
				m_timer.Stop();
			}
		}
		__super::OnForePaint(arg);
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
