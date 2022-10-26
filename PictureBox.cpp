#include "PictureBox.h"
namespace EzUI {
	PictureBox::~PictureBox() {
		if (_gifTask) {
			_exit = true;
			_gifTask->join();
			delete _gifTask;
		}
	}
	void PictureBox::OnRemove() {
		__super::OnRemove();
		if (_gifTask) {
			_exit = true;
			_gifTask->join();
			delete _gifTask;
			_gifTask = NULL;
		}
	}
	void PictureBox::SetImage(Image* image) {
		_mtx.lock();
		_img = image;
		_mtx.unlock();
		if (image->FrameCount() > 1) {
			if (_gifTask == NULL) {
				_gifTask = new std::thread([=]() -> void {
					for (; !_exit;) {
						_mtx.lock();
						size_t pause = 0;
						if (_img) {
							pause = _img->NextFrame();
						}
						_mtx.unlock();
						bool rePaint = true;

						if (!IsVisible()) {
							rePaint = false;
						}
						if (Parent && rePaint) {
							auto itor = std::find(Parent->VisibleControls.begin(), Parent->VisibleControls.end(), this);
							if (itor == Parent->VisibleControls.end()) {
								rePaint = false;
							}
						}
						if (rePaint) {
							this->Invalidate();
						}
						Sleep(pause);
					}
					});
			}
		}
	}
	void PictureBox::OnForePaint(PaintEventArgs& arg) {
		if (_img) {
			std::unique_lock<std::mutex> autoLock(_mtx);
			arg.Painter.DrawImage(_img, Rect(0, 0, Width(), Height()), _img->SizeMode, _img->Padding);
		}
		__super::OnForePaint(arg);
	}
	void PictureBox::SetAttribute(const EString& key, const EString& value) {
		__super::SetAttribute(key, value);
		if (key == "src" || key == "img") {
			Image* img = new Image(value);
			SetImage(img);
		}
	}


};
